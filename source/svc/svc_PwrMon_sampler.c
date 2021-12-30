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
    uint32_t                     smplCnt;
} svc_PwrMon_SamplerCtx_t;


/*============================================================================*/
// Define a max size packet with known pattern to test the Packet Data pipe
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
static svc_PwrMon_SamplerPkt_t svc_PwrMon_tstPkt = {
    0,
    {
        {
            .id = SVC_PWRMONEH_SAMPLE_IND,
            .eh = SVC_EHID_BROADCAST,
            .alloc = false,
            .cnt = 0,
            .len = sizeof(svc_PwrMonEh_SampleInd_t) - sizeof(svc_PwrMonEh_SampleData_t)
        },
        .numCh = 9,
        .seq = 0,
        .ioBitmap = 0x000F,
        .chBitmap = 0x02FF,
        { {   -1,   -1 },
          {    0,    0 },
          { 1440,  100 },
          { 1440,  200 },
          { 1600,  300 },
          { 2400,  400 },
          { 2640,  500 },
          { 3200,  600 },
          { 3200,  700 } }
    }
};
#endif

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_SamplerCtx_t BSP_ATTR_ALIGNMENT(4) svc_PwrMon_samplerCtx;

#define SVC_PWRMON_SAMPLER_SET_COMPLETE(_ctx)                                   \
    ( ((_ctx)->pktInfoArray[(_ctx)->pktIdx].shuntBitmap == (_ctx)->chBitmap) && \
      ((_ctx)->pktInfoArray[(_ctx)->pktIdx].busBitmap == (_ctx)->chBitmap) )

/*==============================================================================
 *                             Local Functions
 *============================================================================*/
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
    ctx->pktArray[ ctx->pktIdx ].smplInd.ioBitmap = 0;
    // snapshot the channels that completed. This should always match the
    // ctx->chBitmap if things are operating normally
    ctx->pktArray[ ctx->pktIdx ].smplInd.chBitmap = ctx->pktInfoArray[ ctx->pktIdx ].shuntBitmap &
                                                    ctx->pktInfoArray[ ctx->pktIdx ].busBitmap;

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
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_samplerCtx;
    ctx->pktInfoArray[ ctx->pktIdx ].busBitmap |= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_shuntVoltageCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_samplerCtx;
    ctx->pktInfoArray[ ctx->pktIdx ].shuntBitmap |= (1 << (uint32_t)cbData);
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
    pktInfoPtr->busBitmap   = 0;
    pktInfoPtr->shuntBitmap = 0;

    // Request next set of samples
    uint16_t chIdx = 0;
    BSP_GPIO_OUT_SET_HIGH( TPA3 );
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
    BSP_GPIO_OUT_SET_LOW( TPA3 );
    BSP_TRACE_PWRMON_SAMPLER_SET_READ_EXIT();
    return;
}

/*============================================================================*/
// Called in the sampling callback to trigger reads of all channels
void
svc_PwrMon_smplSetProcess( svc_PwrMon_SamplerCtx_t* ctx )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_PROCESS_ENTER();

    svc_PwrMon_SamplerPkt_t* pktPtr;
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
    pktPtr = &svc_PwrMon_tstPkt;
    svc_PwrMon_tstPkt.smplInd.seq++;
#else
    pktPtr = &ctx->pktArray[ ctx->pktPrevIdx ];
#endif
    svc_PwrMon_channelProcessSampleSet( pktPtr->smplInd.numCh,
                                        pktPtr->smplInd.seq,
                                        pktPtr->smplInd.ioBitmap,
                                        pktPtr->smplInd.chBitmap,
                                        pktPtr->smplInd.data );
    BSP_TRACE_PWRMON_SAMPLER_SET_PROCESS_EXIT();
    return;
}

/*============================================================================*/
// Main interrupt driving the sampling of the Power Monitor ICs
void svc_PwrMon_samplerCallback( bsp_TimerGp_TimerId_t    timerId,
                                 bsp_TimerGp_SubTimerId_t subTimerId,
                                 uint32_t                 mask )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_ENTER();
    BSP_GPIO_OUT_SET_HIGH( TPA2 );
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_samplerCtx;

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

    // Auto stop when sample count is 0
    if( ctx->smplCnt == 0 )
    {
        bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
        svc_PwrMonEh_buildAndSendStopInd( SVC_PWRMONEH_STATUS_SUCCESS );
    }

    // MaxInt smplCnt samples until manually stopped
    ctx->smplCnt = (ctx->smplCnt == 0xFFFFFFFF) ? ctx->smplCnt : (ctx->smplCnt - 1);
    BSP_GPIO_OUT_SET_LOW( TPA2 );
    BSP_TRACE_PWRMON_SAMPLER_SET_EXIT();
    return;
}


/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_samplerInit( void )
{
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
    memset( &svc_PwrMon_samplerCtx, 0, sizeof(svc_PwrMon_samplerCtx) );
}

/*============================================================================*/
void svc_PwrMon_samplerStop( void )
{
    svc_PwrMon_samplerCtx.smplCnt = 0;
}

/*============================================================================*/
svc_PwrMonEh_SamplerStats_t*
svc_PwrMon_samplerStatsPtr( void )
{
    return( &svc_PwrMon_samplerCtx.stats );
}

/*============================================================================*/
void
svc_PwrMon_samplerStatsReset( void )
{
    memset( &svc_PwrMon_samplerCtx.stats, 0, sizeof(svc_PwrMon_samplerCtx.stats) );
    return;
}

/*============================================================================*/
void svc_PwrMon_samplerStart( svc_PwrMonEh_ChBitmap_t chBitmap,
                              uint32_t                smplCnt )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_samplerCtx;
    memset( &ctx->pktInfoArray[0], 0, sizeof(ctx->pktInfoArray) );
    svc_PwrMon_samplerStatsReset();

    // Reset Frame
    ctx->firstSmpl = true;
    ctx->chBitmap  = chBitmap;
    ctx->seq       = 0;

    // Set smplCnt to maxInt if passed in 0 to prevent autostop
    ctx->smplCnt = (smplCnt == 0) ? 0xFFFFFFFF : smplCnt;

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
        pktPtr->smplInd.chBitmap = 0;

        // These fields change per packet, initialized to 0
        pktPtr->smplInd.seq = ctx->seq;
    }

    // Get each power monitor IC set to the proper register with a dummy read
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        dev_PwrMon_Data_t tmp;

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
