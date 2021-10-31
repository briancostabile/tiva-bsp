/**
 * Copyright 2021 Brian Costabile
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*============================================================================*/
/**
 * @file svc_PwrMon.c
 * @brief Contains channel object and APIs
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Led.h"
#include "bsp_Assert.h"
#include "svc_EhId.h"
#include "svc_PwrMonEh.h"
#include "svc_PwrMon_channel.h"
#include "dev_PwrMon.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_CHANNEL_DATA_PACKET_BUFFER_CNT 2

#define SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV(_val) (((_val) * 1250) / 1000)
#define SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV(_val) (((_val) * 25) / 10)

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef struct svc_PwrMon_ChannelInfo_s
{
    dev_PwrMon_ChannelId_t  chId;
    svc_PwrMonEh_ShuntVal_t shuntVal;
    char                    name[SVC_PWRMONEH_CH_NAME_LEN+1];

    int16_t                 offsetVbus;
    int16_t                 offsetVshunt;

    uint32_t                smplCnt;
    uint32_t                avgCnt;
    int64_t                 mvBusSum;
    int64_t                 uvShuntSum;

    int32_t                 mvBusLast;
    int32_t                 uvShuntLast;

    int32_t                 mvBusAvg;
    int32_t                 uvShuntAvg;
    int32_t                 uaShuntAvg;
} svc_PwrMon_ChannelInfo_t;

// Structure is a preallocated message to be routed through event handler framework
typedef struct BSP_ATTR_PACKED svc_PwrMon_ChannelDataPkt_s {
    uint32_t               sysData;
    svc_PwrMonEh_DataInd_t dataInd;
} svc_PwrMon_ChannelDataPkt_t;

typedef struct svc_PwrMon_ChannelDataPktInfo_s
{
    uint16_t smplSet;
} svc_PwrMon_ChannelDataPktInfo_t;

typedef struct svc_PwrMon_ChannelCtx_s
{
    svc_PwrMon_ChannelDataPkt_t      pktArray[SVC_PWRMON_CHANNEL_DATA_PACKET_BUFFER_CNT];
    svc_PwrMon_ChannelDataPktInfo_t  pktInfoArray[SVC_PWRMON_CHANNEL_DATA_PACKET_BUFFER_CNT];
    svc_PwrMon_ChannelInfo_t         channelInfo[BSP_PLATFORM_PWRMON_NUM_CHANNELS];
    svc_PwrMonEh_ChannelStats_t      stats;
    uint32_t                         seq;
    svc_PwrMonEh_SmplFmt_t           smplFmt;
    svc_PwrMonEh_ChBitmap_t          chBitmap;
    uint8_t                          numCh;
    uint8_t                          pktIdx;
    uint8_t                          pktPrevIdx;
    uint32_t                         pktLen;
} svc_PwrMon_SamplerCtx_t;


/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_SamplerCtx_t svc_PwrMon_channelCtx;

#define SVC_PWRMON_CHANNEL_LED_ACTIVE_COLOR (BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_G | BSP_LED_COLOR_MASK_B)
#define SVC_PWRMON_CHANNEL_LED_ID(_ch) (BSP_PLATFORM_LED_ID_CH0 + (_ch))
#define SVC_PWRMON_CHANNEL_LED_BOOT_TIME_MS 500

/*============================================================================*/
void
svc_PwrMon_channelLedAllOff( void )
{
    for( int i = 0; i < BSP_PLATFORM_PWRMON_NUM_CHANNELS; i++ )
    {
        bsp_Led_setColor( SVC_PWRMON_CHANNEL_LED_ID(i), 0 );
    }
    return;
}

/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_PwrMon_channelInit( void )
{
    memset( &svc_PwrMon_channelCtx, 0, sizeof(svc_PwrMon_channelCtx) );

    // Sweep through and light up the channel LEDs for a boot signal
    svc_PwrMon_channelLedAllOff();
    for( int i = 0; i < BSP_PLATFORM_PWRMON_NUM_CHANNELS; i++ )
    {
        bsp_Led_setColor( SVC_PWRMON_CHANNEL_LED_ID(i), SVC_PWRMON_CHANNEL_LED_ACTIVE_COLOR );
        bsp_Clk_delayMs( SVC_PWRMON_CHANNEL_LED_BOOT_TIME_MS );
        bsp_Led_setColor( SVC_PWRMON_CHANNEL_LED_ID(i), 0 );
    }
    return;
}

/*============================================================================*/
svc_PwrMonEh_ChannelStats_t*
svc_PwrMon_channelStatsPtr( void )
{
    return( &svc_PwrMon_channelCtx.stats );
}

/*============================================================================*/
uint8_t
svc_PwrMon_channelIdToIdx( dev_PwrMon_ChannelId_t chId )
{
    svc_PwrMonEh_ChBitmap_t tmp;
    uint8_t chIdx;

    // Mask out upper bits then count 1s
    tmp = (1 << chId);
    BSP_ASSERT( (tmp & svc_PwrMon_channelCtx.chBitmap) != 0 );
    tmp -= 1;
    tmp &= svc_PwrMon_channelCtx.chBitmap;

    chIdx = 0;
    while( tmp != 0 )
    {
        if( tmp & 0x01 )
        {
            chIdx++;
        }
        tmp >>= 1;
    }

    return( chIdx );
}

/*============================================================================*/
void
svc_PwrMon_channelSmplFmtSet( svc_PwrMonEh_SmplFmt_t smplFmt )
{
    svc_PwrMon_channelCtx.smplFmt = smplFmt;
    return;
}

/*============================================================================*/
void
svc_PwrMon_channelCalibrateAll( void )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_channelCtx;

    // Turn off all Channel LEDs
    svc_PwrMon_channelLedAllOff();

    for( int chId=0; chId < DIM(ctx->channelInfo); chId++ )
    {
        dev_PwrMon_channelOffsetCal( chId,
                                     &(ctx->channelInfo[chId].offsetVbus),
                                     &(ctx->channelInfo[chId].offsetVshunt) );

        SVC_LOG_INFO( "[PwrMon Channel] dev_PwrMon_channelOffsetCal chId:%d offsetVbus:%d offsetVshunt:%d"NL,
                      chId,
                      ctx->channelInfo[chId].offsetVbus,
                      ctx->channelInfo[chId].offsetVshunt );

        bsp_Led_setColor( SVC_PWRMON_CHANNEL_LED_ID(chId),
                          SVC_PWRMON_CHANNEL_LED_ACTIVE_COLOR );
    }
    svc_PwrMon_channelLedAllOff();

    // Save cal in EEPROM so it's not needed every time

    return;
}

/*============================================================================*/
void
svc_PwrMon_channelConfigSet( uint8_t                 numCh,
                             svc_PwrMonEh_ChEntry_t* chTable )
{
    BSP_ASSERT( numCh <= BSP_PLATFORM_PWRMON_NUM_CHANNELS );

    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_channelCtx;

    // Turn off all Channel LEDs
    svc_PwrMon_channelLedAllOff();

    /**********************
     * Copy data from channel table and cal in-use devices
     *********************/
    ctx->numCh = numCh;
    ctx->chBitmap = 0;
    ctx->seq = 0;
    SVC_LOG_INFO( "[PwrMon Channel] svc_PwrMon_channelConfigSet numCh:%d"NL, numCh );

    for( int chIdx=0; chIdx < numCh; chIdx++ )
    {
        strncpy( ctx->channelInfo[chIdx].name, chTable[chIdx].chName, SVC_PWRMONEH_CH_NAME_LEN );
        ctx->channelInfo[chIdx].name[SVC_PWRMONEH_CH_NAME_LEN] = 0;

        ctx->channelInfo[chIdx].chId       = chTable[chIdx].chId;
        ctx->channelInfo[chIdx].shuntVal   = chTable[chIdx].shuntVal;
        ctx->channelInfo[chIdx].smplCnt    = 0;
        ctx->channelInfo[chIdx].avgCnt     = 0;
        ctx->channelInfo[chIdx].mvBusSum   = 0;
        ctx->channelInfo[chIdx].uvShuntSum = 0;

        ctx->chBitmap |= (1 << chTable[chIdx].chId);

        SVC_LOG_INFO( "[PwrMon Channel] dev_PwrMon_channelOffsetCal name:%s chId:%d shunt:%ld"NL,
                      chTable[chIdx].chName,
                      chTable[chIdx].chId,
                      chTable[chIdx].shuntVal );

        bsp_Led_setColor( SVC_PWRMON_CHANNEL_LED_ID(chTable[chIdx].chId),
                          SVC_PWRMON_CHANNEL_LED_ACTIVE_COLOR );
    }
    SVC_LOG_INFO( "[PwrMon Channel] svc_PwrMon_channelConfigSet chBitmap:0x%08X"NL, ctx->chBitmap );

    /**********************
     * Setup DataInd buffer Packet
     *********************/
    // Compute the packet length in bytes
    ctx->pktLen = ( (sizeof(svc_PwrMonEh_DataInd_t) - sizeof(svc_MsgFwk_Hdr_t)) -
                    ((SVC_PWRMONEH_DATA_IND_CHANNELS_MAX - ctx->numCh) * sizeof(svc_PwrMonEh_SmplData_t)) );

    // Setup static portion of headers
    for( int i=0; i<SVC_PWRMON_CHANNEL_DATA_PACKET_BUFFER_CNT; i++ )
    {
        svc_PwrMon_ChannelDataPkt_t*     pktPtr     = &ctx->pktArray[ i ];
        svc_PwrMon_ChannelDataPktInfo_t* pktInfoPtr = &ctx->pktInfoArray[ i ];

        // sysData is typically hidden behind the msgFwk allocation method
        // and is used when forwarding messages off device.
        pktPtr->sysData = 0;

        // Setup the Event Handler message portion. cn increments for every
        // received queue and decrements for every free. Will be 0 once
        // shipped off device
        pktPtr->dataInd.hdr.id    = SVC_PWRMONEH_DATA_IND;
        pktPtr->dataInd.hdr.eh    = SVC_EHID_BROADCAST;
        pktPtr->dataInd.hdr.alloc = false;
        pktPtr->dataInd.hdr.cnt   = 0;
        pktPtr->dataInd.hdr.len   = ctx->pktLen + sizeof(svc_MsgFwk_Hdr_t);

        // Fill in sample packet header information
        pktPtr->dataInd.numCh      = ctx->numCh;
        pktPtr->dataInd.numSmplSet = SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX;
        pktPtr->dataInd.chBitmap   = ctx->chBitmap;

        // These fields change per packet, initialized to 0
        pktPtr->dataInd.seq          = ctx->seq;
        pktPtr->dataInd.smplStartIdx = 0;

        // Reset packet info
        pktInfoPtr->smplSet = 0;
    }

    return;
}

/*============================================================================*/
void
svc_PwrMon_channelProcessSampleSet( uint16_t                   numCh,
                                    uint32_t                   seq,
                                    svc_PwrMonEh_IoBitmap_t    ioBitmap,
                                    svc_PwrMonEh_ChBitmap_t    chBitmap,
                                    svc_PwrMonEh_SampleData_t* dataPtr )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_channelCtx;

    //BSP_ASSERT( numCh == ctx->numCh );

    // Uncomment to assert on errors or comment to
    // allow errors to be detected by host
    //BSP_ASSERT( chBitmap == ctx->chBitmap );

    // Figure out where this sample set goes in the packet
    svc_PwrMon_ChannelDataPkt_t*     pktPtr     = &ctx->pktArray[ ctx->pktIdx ];
    svc_PwrMon_ChannelDataPktInfo_t* pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];

    // If this is the first set in the packet then set the start index
    if( pktInfoPtr->smplSet == 0 )
    {
        pktPtr->dataInd.smplStartIdx = seq;
    }

    // For each channel in the data set, keep running average and copy formatted sample
    // to dataInd packet
    uint16_t offsetIdx = (pktInfoPtr->smplSet * (ctx->numCh + 1)); // add one to make space for header

    // Put in the header
    pktPtr->dataInd.data[offsetIdx].hdr.ioBitmap = ioBitmap;
    pktPtr->dataInd.data[offsetIdx].hdr.chBitmap = chBitmap;
    offsetIdx += 1;
    for( uint8_t chIdx = 0; chIdx < numCh; chIdx++ )
    {
        // Adjust vShunt and vBus by calibration offsets
        dev_PwrMon_Data_t vShuntAdjusted;
        dev_PwrMon_Data_t vBusAdjusted;

        vShuntAdjusted = dataPtr[chIdx].vShunt - svc_PwrMon_channelCtx.channelInfo[chIdx].offsetVshunt;
        vBusAdjusted = dataPtr[chIdx].vBus - svc_PwrMon_channelCtx.channelInfo[chIdx].offsetVbus;

        // Copy over to packet
        size_t dataIdx = (offsetIdx + chIdx);
        if( ctx->smplFmt == SVC_PWRMONEH_SMPL_FMT1_VV )
        {
            pktPtr->dataInd.data[dataIdx].fmt0.vShunt = vShuntAdjusted;
            pktPtr->dataInd.data[dataIdx].fmt0.vBus = vBusAdjusted;
        }
        else if( ctx->smplFmt == SVC_PWRMONEH_SMPL_FMT2_P )
        {
            int64_t mvBus = SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV( vBusAdjusted );
            int64_t uvShunt = SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV( vShuntAdjusted );
            int64_t uaShunt = ((uvShunt * 1000) / svc_PwrMon_channelCtx.channelInfo[chIdx].shuntVal);
            pktPtr->dataInd.data[dataIdx].fmt1.power = (int32_t)((mvBus * uaShunt) / 1000);
        }

        ctx->channelInfo[chIdx].mvBusLast = SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV( vBusAdjusted );
        ctx->channelInfo[chIdx].uvShuntLast = SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV( vShuntAdjusted );

        // Update average counters
        ctx->channelInfo[chIdx].smplCnt++;
        ctx->channelInfo[chIdx].avgCnt++;
        ctx->channelInfo[chIdx].mvBusSum += vBusAdjusted;
        ctx->channelInfo[chIdx].uvShuntSum += vShuntAdjusted;
    }

    // Check to see if the packet is ready to be sent
    pktInfoPtr->smplSet++;
    if( pktInfoPtr->smplSet == SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX )
    {
        BSP_TRACE_PWRMONEH_CHANNEL_DATA_IND_SEND();

        // Reset frame info and point to next packet
        ctx->pktPrevIdx     = ctx->pktIdx;
        ctx->pktIdx         = ((ctx->pktIdx + 1) % SVC_PWRMON_CHANNEL_DATA_PACKET_BUFFER_CNT);
        pktPtr              = &ctx->pktArray[ ctx->pktIdx ];
        pktInfoPtr          = &ctx->pktInfoArray[ ctx->pktIdx ];
        pktInfoPtr->smplSet = 0;

        // Make sure the new buffer is not still being held onto
        ctx->stats.pktErrNum += (ctx->pktArray[ ctx->pktIdx ].dataInd.hdr.cnt != 0) ? 1 : 0;

        // Setup next packet header
        ctx->stats.pktSndNum++;
        pktPtr->dataInd.smplFmt = ctx->smplFmt;
        pktPtr->dataInd.seq = ctx->stats.pktSndNum;

        if( ctx->smplFmt != SVC_PWRMONEH_SMPL_NO_STREAM )
        {
            svc_MsgFwk_msgBroadcast( &(ctx->pktArray[ ctx->pktPrevIdx ].dataInd.hdr) );
        }
    }
    return;
}

/*============================================================================*/
void
svc_PwrMon_channelAvgReset( dev_PwrMon_ChannelId_t chId )
{
    uint8_t chIdx = svc_PwrMon_channelIdToIdx( chId );
    BSP_MCU_CRITICAL_SECTION_ENTER();
    svc_PwrMon_channelCtx.channelInfo[chIdx].avgCnt    = 0;
    svc_PwrMon_channelCtx.channelInfo[chIdx].mvBusSum   = 0;
    svc_PwrMon_channelCtx.channelInfo[chIdx].uvShuntSum = 0;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}

/*============================================================================*/
const char*
svc_PwrMon_channelName( dev_PwrMon_ChannelId_t chId )
{
    uint8_t chIdx = svc_PwrMon_channelIdToIdx( chId );
    return( svc_PwrMon_channelCtx.channelInfo[chIdx].name );
}

/*============================================================================*/
void
svc_PwrMon_channelAvgResetAll( void )
{
    uint8_t chIdx = 0;
    svc_PwrMonEh_ChBitmap_t bitmap = svc_PwrMon_channelCtx.chBitmap;
    while( bitmap )
    {
        if( bitmap & 1 )
        {
            BSP_MCU_CRITICAL_SECTION_ENTER();
            svc_PwrMon_channelCtx.channelInfo[chIdx].avgCnt     = 0;
            svc_PwrMon_channelCtx.channelInfo[chIdx].mvBusSum   = 0;
            svc_PwrMon_channelCtx.channelInfo[chIdx].uvShuntSum = 0;
            BSP_MCU_CRITICAL_SECTION_EXIT();
            chIdx++;
        }
        bitmap >>= 1;
    }
    return;
}

/*============================================================================*/
void
svc_PwrMon_channelAvgGet( dev_PwrMon_ChannelId_t chId,
                          int32_t*               vBusPtr,
                          int32_t*               vShuntPtr,
                          int32_t*               iShuntPtr,
                          int32_t*               pPtr )
{
    int64_t mvBusSum;
    int64_t uvShuntSum;
    uint32_t cnt;

    uint8_t chIdx = svc_PwrMon_channelIdToIdx( chId );
    BSP_MCU_CRITICAL_SECTION_ENTER();
    cnt        = svc_PwrMon_channelCtx.channelInfo[chIdx].avgCnt;
    mvBusSum   = svc_PwrMon_channelCtx.channelInfo[chIdx].mvBusSum;
    uvShuntSum = svc_PwrMon_channelCtx.channelInfo[chIdx].uvShuntSum;
    BSP_MCU_CRITICAL_SECTION_EXIT();

    int64_t mvBusAvg = SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV( (mvBusSum / cnt) );
    int64_t uvShuntAvg = SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV( (uvShuntSum / cnt) );

    // Save last calculations in global context
    svc_PwrMon_channelCtx.channelInfo[chIdx].mvBusAvg = (int32_t)mvBusAvg;
    svc_PwrMon_channelCtx.channelInfo[chIdx].uvShuntAvg = (int32_t)uvShuntAvg;
    svc_PwrMon_channelCtx.channelInfo[chIdx].uaShuntAvg =
            (int32_t)((uvShuntAvg * 1000) / svc_PwrMon_channelCtx.channelInfo[chIdx].shuntVal);

    *vBusPtr   = svc_PwrMon_channelCtx.channelInfo[chIdx].mvBusAvg;
    *vShuntPtr = svc_PwrMon_channelCtx.channelInfo[chIdx].uvShuntAvg;
    *iShuntPtr = svc_PwrMon_channelCtx.channelInfo[chIdx].uaShuntAvg;
    *pPtr      = (*vBusPtr * *iShuntPtr) / 1000;

    return;
}


/*============================================================================*/
uint8_t
svc_PwrMon_channelAvgGetAll( svc_PwrMonEh_ChAvgInfo_t* dataBuffer )
{
    uint8_t chId = 0;
    uint8_t bufIdx = 0;
    svc_PwrMonEh_ChBitmap_t bitmap = svc_PwrMon_channelCtx.chBitmap;
    while( bitmap )
    {
        if( bitmap & 1 )
        {
            // Use temporary variables because SmplData is not aligned
            int32_t vBus;
            int32_t vShunt;
            int32_t iShunt;
            int32_t power;
            svc_PwrMon_channelAvgGet( chId, &vBus, &vShunt, &iShunt, &power );
            dataBuffer[ bufIdx ].chId       = chId;
            dataBuffer[ bufIdx ].mvBusAvg   = vBus;
            dataBuffer[ bufIdx ].uvShuntAvg = vShunt;
            dataBuffer[ bufIdx ].uaShuntAvg = iShunt;
            dataBuffer[ bufIdx ].uwAvg      = power;
            bufIdx++;
        }
        bitmap >>= 1;
        chId++;
    }

    return( svc_PwrMon_channelCtx.numCh );
}

/*============================================================================*/
svc_PwrMonEh_ChBitmap_t
svc_PwrMon_channelBitmapGet( void )
{
    svc_PwrMonEh_ChBitmap_t bitmap;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    bitmap = svc_PwrMon_channelCtx.chBitmap;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return( bitmap );
}

/*============================================================================*/
uint8_t
svc_PwrMon_channelCntGet( void )
{
    uint8_t num;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    num = svc_PwrMon_channelCtx.numCh;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return( num );
}

#endif
