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
 * @file svc_PwrMon_sampler.c
 * @brief Contains functions for the Power Manager Sampling engine
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "bsp_Assert.h"
#include "bsp_TimerGp.h"
#include "svc_PwrMon_channel.h"
#include "svc_PwrMon_sampler.h"
#include "svc_EhId.h"
#include "dev_PwrMon.h"
#include "svc_MsgFwk.h"
#include "svc_PwrMonEh.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_SAMPLER_TIMER_ID BSP_TIMERGP_ID_0

// How many DataInd packets to buffer
#define SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT 2

// If we want to include in the test packet which is the max sized packet with
// known pattern. Used for bandwidth and performance testing
//#define SVC_PWRMON_SAMPLER_TEST_PKT


/*==============================================================================
 *                                Types
 *============================================================================*/

typedef struct svc_PwrMon_SamplerGpiInfo_s
{
    bsp_Gpio_PortId_t  portId;
    bsp_Gpio_BitMask_t mask;
} svc_PwrMon_SamplerGpiInfo_t;

// Structure is a preallocated message to be routed through event handler framework
typedef struct BSP_ATTR_PACKED svc_PwrMon_SamplerPkt_s {
    uint32_t                 sysData;
    svc_PwrMonEh_SampleInd_t smplInd;
} svc_PwrMon_SamplerPkt_t;

typedef struct svc_PwrMon_SamplerFrameInfo_s
{
    svc_PwrMonEh_ChBitmap_t shuntBitmap;
    svc_PwrMonEh_ChBitmap_t busBitmap;
} svc_PwrMon_SamplerPktInfo_t;

typedef struct svc_PwrMon_SamplerCtx_s
{
    svc_PwrMon_SamplerPkt_t      pktArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMon_SamplerPktInfo_t  pktInfoArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMonEh_SamplerStats_t  stats;
    svc_PwrMonEh_ChBitmap_t      chBitmap;
    uint8_t                      numCh;
    uint8_t                      pktIdx;
    uint8_t                      pktPrevIdx;
    bool_t                       firstSmpl;
    uint32_t                     pktLen;
    uint32_t                     seq;
} svc_PwrMon_SamplerCtx_t;


/*============================================================================*/
// Define a max size packet with known pattern to test the Packet Data pipe
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
static const svc_PwrMonEh_SampleInd_t svc_PwrMon_tstPkt = {
    0,
    {
        {
            .id = SVC_PWRMONEH_SAMPLE_IND,
            .eh = SVC_EHID_BROADCAST,
            .alloc = false,
            .cnt = 0,
            .len = sizeof(svc_PwrMon_SamplerPkt_t)
        },
        .numCh = 10,
        .seq = 1234,
        .ioBitmap = 0x0000000F,
        .chBitmap = 0x000003FF,
        { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 },  { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 8 }, { 8, 9 }, { 9, 10 } }
    }
};
#endif

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_SamplerCtx_t BSP_ATTR_ALIGNMENT(4) svc_PwrMon_ctx;
svc_PwrMon_SamplerGpiInfo_t svc_PwrMon_gpiInfoTable[] = BSP_PLATFORM_PWRMON_GPI_TABLE;

#define SVC_PWRMON_SAMPLER_SET_COMPLETE(_ctx)                    \
    ( ((_ctx)->pktInfoArray[(_ctx)->pktIdx].shuntBitmap == 0) && \
      ((_ctx)->pktInfoArray[(_ctx)->pktIdx].busBitmap == 0) )

/*==============================================================================
 *                             Local Functions
 *============================================================================*/
uint32_t
svc_PwrMon_gpiRead( void )
{
    uint32_t gpiVal = 0;
    uint32_t tmp = 0;

    for( uint8_t i=0; i<DIM(svc_PwrMon_gpiInfoTable); i++ )
    {
        tmp = bsp_Gpio_read( svc_PwrMon_gpiInfoTable[i].portId, svc_PwrMon_gpiInfoTable[i].mask );
        gpiVal |= (tmp != 0) ? (1 << i) : 0;
    }

    return( gpiVal );
}

/*============================================================================*/
// Called at the beginning of the fixed time sampling callback, before making all i2c requests
void
svc_PwrMon_smplSetStart( svc_PwrMon_SamplerCtx_t* ctx )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_START_ENTER();
    /**********
    * Stats Update
    ***********/
    // Unconditionally increment the smplNum
    ctx->stats.smplNum++;

    // Check for errors
    if( SVC_PWRMON_SAMPLER_SET_COMPLETE( ctx ) == false )
    {
        // Previous sample set did not complete in time.
        // Todo: Keep track of errors per channel
        ctx->stats.smplErrNum++;
    }

    /**********
    * Packet Update
    ***********/
    // Grab the GPI state
    ctx->pktArray[ ctx->pktIdx ].smplInd.ioBitmap = svc_PwrMon_gpiRead();

    /**********
    * Next Packet setup
    ***********/
    // Reset frame info and point to next packet
    ctx->pktPrevIdx = ctx->pktIdx;
    ctx->pktIdx     = ((ctx->pktIdx + 1) % SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT);


    // Setup next packet header
    ctx->seq++;
    ctx->pktArray[ ctx->pktIdx ].smplInd.seq = ctx->seq;

    BSP_TRACE_PWRMON_SAMPLER_SET_START_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_busVoltageCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].busBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_shuntVoltageCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].shuntBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_EXIT();
    return;
}

/*============================================================================*/
// Called in the sampling callback to trigger reads of all channels
void
svc_PwrMon_smplSetRead( svc_PwrMon_SamplerCtx_t* ctx )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_READ_ENTER();
    // Request all configured channels for next sample Set
    svc_PwrMon_SamplerPkt_t*     pktPtr     = &ctx->pktArray[ ctx->pktIdx ];
    svc_PwrMon_SamplerPktInfo_t* pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];

    // Reset the sample set bitmaps
    pktInfoPtr->busBitmap   = ctx->chBitmap;
    pktInfoPtr->shuntBitmap = ctx->chBitmap;

    // Request next set of samples
    uint16_t chIdx = 0;
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            dev_PwrMon_channelShuntVoltageRead( chId,
                                                (uint8_t*)&pktPtr->smplInd.data[ chIdx ].vShunt,
                                                svc_PwrMon_shuntVoltageCallback,
                                                (void*)(uint32_t)chId );

            dev_PwrMon_channelBusVoltageRead( chId,
                                              (uint8_t*)&pktPtr->smplInd.data[ chIdx ].vBus,
                                              svc_PwrMon_busVoltageCallback,
                                              (void*)(uint32_t)chId );
            chIdx++;
        }
    }
    BSP_TRACE_PWRMON_SAMPLER_SET_READ_EXIT();
    return;
}

/*============================================================================*/
// Called in the sampling callback to trigger reads of all channels
void
svc_PwrMon_smplSetProcess( svc_PwrMon_SamplerCtx_t* ctx )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_PROCESS_ENTER();
    svc_PwrMon_SamplerPkt_t* pktPtr = &ctx->pktArray[ ctx->pktPrevIdx ];
    svc_PwrMon_channelProcessSampleSet( pktPtr->smplInd.numCh,
                                        pktPtr->smplInd.seq,
                                        pktPtr->smplInd.ioBitmap,
                                        pktPtr->smplInd.chBitmap,
                                        pktPtr->smplInd.data );
    BSP_TRACE_PWRMON_SAMPLER_SET_PROCESS_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_sendTestPkt( void )
{
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
    memcpy( &svc_PwrMon_ctx.pktArray[0], &svc_PwrMon_tstPkt, sizeof(svc_PwrMon_tstPkt));
    svc_MsgFwk_msgBroadcast( &(svc_PwrMon_ctx.pktArray[0].smplInd.hdr) );
#endif
}

/*============================================================================*/
// Main interrupt driving the sampling of the Power Monitor ICs
void svc_PwrMon_samplerCallback( bsp_TimerGp_TimerId_t    timerId,
                                 bsp_TimerGp_SubTimerId_t subTimerId,
                                 uint32_t                 mask )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;

    // Wrap up previous samples
    bool_t procSmpls = true;
    if( ctx->firstSmpl == false )
    {
        svc_PwrMon_smplSetStart( ctx );
    }
    else
    {
        procSmpls = false;
        ctx->firstSmpl = false;
    }

    // Request all configured channels for next sample Set
    svc_PwrMon_smplSetRead( ctx );

    // Check if samples should be processed
    if( procSmpls == true )
    {
        svc_PwrMon_smplSetProcess( ctx );
    }
    BSP_TRACE_PWRMON_SAMPLER_SET_EXIT();
    return;
}


/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_samplerInit( void )
{
    BSP_ASSERT(callback != NULL);
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
    memset( &svc_PwrMon_ctx, 0, sizeof(svc_PwrMon_ctx) );

    /* For each GPI, configure as input no pull */
    for( uint8_t i=0; i<DIM(svc_PwrMon_gpiInfoTable); i++ )
    {
        bsp_Gpio_configInput( svc_PwrMon_gpiInfoTable[i].portId,
                              svc_PwrMon_gpiInfoTable[i].mask,
                              FALSE, BSP_GPIO_PULL_NONE );
    }
}

/*============================================================================*/
void svc_PwrMon_samplerStop( void )
{
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
}

/*============================================================================*/
svc_PwrMonEh_SamplerStats_t*
svc_PwrMon_samplerStatsPtr( void )
{
    return( &svc_PwrMon_ctx.stats );
}

/*============================================================================*/
void
svc_PwrMon_samplerStatsReset( void )
{
    memset( &svc_PwrMon_ctx.stats, 0, sizeof(svc_PwrMon_ctx.stats) );
    return;
}

/*============================================================================*/
void svc_PwrMon_samplerStart( svc_PwrMonEh_ChBitmap_t chBitmap )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    memset( &ctx->pktInfoArray[0], 0, sizeof(ctx->pktInfoArray) );
    svc_PwrMon_samplerStatsReset();

    // Reset Frame
    ctx->firstSmpl = true;
    ctx->chBitmap  = chBitmap;
    ctx->seq       = 0;

    // Count the channels
    ctx->numCh = 0;
    while( chBitmap )
    {
        ctx->numCh += (chBitmap & 0x01);
        chBitmap >>= 1;
    }

    // Compute the packet length in bytes
    ctx->pktLen = ( (sizeof(svc_PwrMonEh_SampleInd_t) - sizeof(svc_MsgFwk_Hdr_t)) -
                    ((SVC_PWRMONEH_DATA_IND_CHANNELS_MAX - ctx->numCh) * sizeof(svc_PwrMonEh_SampleData_t)) );

    // Setup static portion of headers
    for( int i=0; i<SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT; i++ )
    {
        svc_PwrMon_SamplerPkt_t* pktPtr = &ctx->pktArray[ i ];

        // sysData is typically hidden behind the msgFwk allocation method
        // and is used when forwarding messages off device.
        pktPtr->sysData = 0;

        // Setup the Event Handler message portion. cn increments for every
        // received queue and decrements for every free. Will be 0 once
        // shipped off device
        pktPtr->smplInd.hdr.id    = SVC_PWRMONEH_SAMPLE_IND;
        pktPtr->smplInd.hdr.eh    = SVC_EHID_BROADCAST;
        pktPtr->smplInd.hdr.alloc = false;
        pktPtr->smplInd.hdr.cnt   = 0;
        pktPtr->smplInd.hdr.len   = ctx->pktLen + sizeof(svc_MsgFwk_Hdr_t);

        // Fill in sample packet header information
        pktPtr->smplInd.numCh    = ctx->numCh;
        pktPtr->smplInd.chBitmap = ctx->chBitmap;

        // These fields change per packet, initialized to 0
        pktPtr->smplInd.seq = ctx->seq;
    }

    // Get each power monitor IC set to the proper register with a dummy read
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        dev_PwrMon_Data_t tmp;

        dev_PwrMon_channelConfig( chId,
                                BSP_PWRMOMN_CONV_TIME_US_140,
                                BSP_PWRMOMN_CONV_TIME_US_140,
                                BSP_PWRMOMN_AVG_MODE_SAMPLES_1,
                                NULL, NULL );

        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            dev_PwrMon_channelShuntVoltageRead( chId, (uint8_t*)&tmp, NULL, NULL );
            dev_PwrMon_channelBusVoltageRead( chId, (uint8_t*)&tmp, NULL, NULL );
        }
    }

    bsp_TimerGp_startCountdown( SVC_PWRMON_SAMPLER_TIMER_ID,
                                BSP_TIMERGP_TYPE_PERIODIC,
                                BSP_TIMERGP_DMA_CTRL_NONE,
                                SVC_PWRMON_SAMPLER_TICK_US,
                                svc_PwrMon_samplerCallback );
}
#endif
