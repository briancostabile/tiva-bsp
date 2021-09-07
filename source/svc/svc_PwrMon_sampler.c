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
#include "bsp_UsbBulk.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_SAMPLER_TIMER_ID BSP_TIMERGP_ID_0

// How many packets to buffer
#define SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT 2

// Packet sample portion in bytes
#define SVC_PWRMON_SAMPLER_PACKET_DATA_SIZE ( SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET * \
                                              SVC_PWRMON_SAMPLER_CHANNELS_MAX *      \
                                              SVC_PWRMON_SAMPLER_SAMPLE_SIZE_MAX )

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef struct svc_PwrMon_SamplerFrameInfo_s
{
    svc_PwrMon_ChannelBitmap_t vBitmap;
    svc_PwrMon_ChannelBitmap_t iBitmap;
    uint16_t                   smplSet;
} svc_PwrMon_SamplerPktInfo_t;

typedef struct svc_PwrMon_SamplerCtx_s
{
    svc_PwrMon_SamplerPacket_t     pktArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMon_SamplerPktInfo_t    pktInfoArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMon_SamplerStats_t      stats;
    svc_PwrMon_SamplerSampleSize_t sampleSize;
    svc_PwrMon_ChannelBitmap_t     chBitmap;
    uint8_t                        numCh;
    uint8_t                        pktIdx;
    uint8_t                        pktPrevIdx;
    bool_t                         firstSample;
    uint32_t                       pktLen;
    uint32_t                       pktRemain;
    dev_PwrMon_SamplerPktReady_t   pktReadyCb;
} svc_PwrMon_SamplerCtx_t;


/*============================================================================*/
// Define a max size packet with known pattern to test the Packet Data pipe
//#define SVC_PWRMON_SAMPLER_TEST_PKT
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
static const svc_PwrMon_SamplerPacket_t svc_PwrMon_tstPkt = {
    {
        .numCh = 11,
        .numSmplSet = 50,
        .sampleSize = 4,
        .seq = 1234,
        .smplStartIdx = 1200,
        .chBitmap = 0x000003FF
    },
    {
        {   0, 1 }, {   1, 2 }, {   2, 3 }, {   3, 4 }, {   4, 5 }, {   5, 6 }, {   6, 7 }, {   7, 8 }, {   8, 9 }, {   9, 10 }, { 500, 11 },
        {  10, 1 }, {  11, 2 }, {  12, 3 }, {  13, 4 }, {  14, 5 }, {  15, 6 }, {  16, 7 }, {  17, 8 }, {  18, 9 }, {  19, 10 }, { 501, 11 },
        {  20, 1 }, {  21, 2 }, {  22, 3 }, {  23, 4 }, {  24, 5 }, {  25, 6 }, {  26, 7 }, {  27, 8 }, {  28, 9 }, {  29, 10 }, { 502, 11 },
        {  30, 1 }, {  31, 2 }, {  32, 3 }, {  33, 4 }, {  34, 5 }, {  35, 6 }, {  36, 7 }, {  37, 8 }, {  38, 9 }, {  39, 10 }, { 503, 11 },
        {  40, 1 }, {  41, 2 }, {  42, 3 }, {  43, 4 }, {  44, 5 }, {  45, 6 }, {  46, 7 }, {  47, 8 }, {  48, 9 }, {  49, 10 }, { 504, 11 },
        {  50, 1 }, {  51, 2 }, {  52, 3 }, {  53, 4 }, {  54, 5 }, {  55, 6 }, {  56, 7 }, {  57, 8 }, {  58, 9 }, {  59, 10 }, { 505, 11 },
        {  60, 1 }, {  61, 2 }, {  62, 3 }, {  63, 4 }, {  64, 5 }, {  65, 6 }, {  66, 7 }, {  67, 8 }, {  68, 9 }, {  69, 10 }, { 506, 11 },
        {  70, 1 }, {  71, 2 }, {  72, 3 }, {  73, 4 }, {  74, 5 }, {  75, 6 }, {  76, 7 }, {  77, 8 }, {  78, 9 }, {  79, 10 }, { 507, 11 },
        {  80, 1 }, {  81, 2 }, {  82, 3 }, {  83, 4 }, {  84, 5 }, {  85, 6 }, {  86, 7 }, {  87, 8 }, {  88, 9 }, {  89, 10 }, { 508, 11 },
        {  90, 1 }, {  91, 2 }, {  92, 3 }, {  93, 4 }, {  94, 5 }, {  95, 6 }, {  96, 7 }, {  97, 8 }, {  98, 9 }, {  99, 10 }, { 509, 11 },
        { 100, 1 }, { 101, 2 }, { 102, 3 }, { 103, 4 }, { 104, 5 }, { 105, 6 }, { 106, 7 }, { 107, 8 }, { 108, 9 }, { 109, 10 }, { 510, 11 },
        { 110, 1 }, { 111, 2 }, { 112, 3 }, { 113, 4 }, { 114, 5 }, { 115, 6 }, { 116, 7 }, { 117, 8 }, { 118, 9 }, { 119, 10 }, { 511, 11 },
        { 120, 1 }, { 121, 2 }, { 122, 3 }, { 123, 4 }, { 124, 5 }, { 125, 6 }, { 126, 7 }, { 127, 8 }, { 128, 9 }, { 129, 10 }, { 512, 11 },
        { 130, 1 }, { 131, 2 }, { 132, 3 }, { 133, 4 }, { 134, 5 }, { 135, 6 }, { 136, 7 }, { 137, 8 }, { 138, 9 }, { 139, 10 }, { 513, 11 },
        { 140, 1 }, { 141, 2 }, { 142, 3 }, { 143, 4 }, { 144, 5 }, { 145, 6 }, { 146, 7 }, { 147, 8 }, { 148, 9 }, { 149, 10 }, { 514, 11 },
        { 150, 1 }, { 151, 2 }, { 152, 3 }, { 153, 4 }, { 154, 5 }, { 155, 6 }, { 156, 7 }, { 157, 8 }, { 158, 9 }, { 159, 10 }, { 515, 11 },
        { 160, 1 }, { 161, 2 }, { 162, 3 }, { 163, 4 }, { 164, 5 }, { 165, 6 }, { 166, 7 }, { 167, 8 }, { 168, 9 }, { 169, 10 }, { 516, 11 },
        { 170, 1 }, { 171, 2 }, { 172, 3 }, { 173, 4 }, { 174, 5 }, { 175, 6 }, { 176, 7 }, { 177, 8 }, { 178, 9 }, { 179, 10 }, { 517, 11 },
        { 180, 1 }, { 181, 2 }, { 182, 3 }, { 183, 4 }, { 184, 5 }, { 185, 6 }, { 186, 7 }, { 187, 8 }, { 188, 9 }, { 189, 10 }, { 518, 11 },
        { 190, 1 }, { 191, 2 }, { 192, 3 }, { 193, 4 }, { 194, 5 }, { 195, 6 }, { 196, 7 }, { 197, 8 }, { 198, 9 }, { 199, 10 }, { 519, 11 },
        { 200, 1 }, { 201, 2 }, { 202, 3 }, { 203, 4 }, { 204, 5 }, { 205, 6 }, { 206, 7 }, { 207, 8 }, { 208, 9 }, { 209, 10 }, { 520, 11 },
        { 210, 1 }, { 211, 2 }, { 212, 3 }, { 213, 4 }, { 214, 5 }, { 215, 6 }, { 216, 7 }, { 217, 8 }, { 218, 9 }, { 219, 10 }, { 521, 11 },
        { 220, 1 }, { 221, 2 }, { 222, 3 }, { 223, 4 }, { 224, 5 }, { 225, 6 }, { 226, 7 }, { 227, 8 }, { 228, 9 }, { 229, 10 }, { 522, 11 },
        { 230, 1 }, { 231, 2 }, { 232, 3 }, { 233, 4 }, { 234, 5 }, { 235, 6 }, { 236, 7 }, { 237, 8 }, { 238, 9 }, { 239, 10 }, { 523, 11 },
        { 240, 1 }, { 241, 2 }, { 242, 3 }, { 243, 4 }, { 242, 5 }, { 245, 6 }, { 246, 7 }, { 247, 8 }, { 248, 9 }, { 249, 10 }, { 524, 11 },
        { 250, 1 }, { 251, 2 }, { 252, 3 }, { 253, 4 }, { 254, 5 }, { 255, 6 }, { 256, 7 }, { 257, 8 }, { 258, 9 }, { 259, 10 }, { 525, 11 },
        { 260, 1 }, { 261, 2 }, { 262, 3 }, { 263, 4 }, { 264, 5 }, { 265, 6 }, { 266, 7 }, { 267, 8 }, { 268, 9 }, { 269, 10 }, { 526, 11 },
        { 270, 1 }, { 271, 2 }, { 272, 3 }, { 273, 4 }, { 274, 5 }, { 275, 6 }, { 276, 7 }, { 277, 8 }, { 278, 9 }, { 279, 10 }, { 527, 11 },
        { 280, 1 }, { 281, 2 }, { 282, 3 }, { 283, 4 }, { 284, 5 }, { 285, 6 }, { 286, 7 }, { 287, 8 }, { 288, 9 }, { 289, 10 }, { 528, 11 },
        { 290, 1 }, { 291, 2 }, { 292, 3 }, { 293, 4 }, { 294, 5 }, { 295, 6 }, { 296, 7 }, { 297, 8 }, { 298, 9 }, { 299, 10 }, { 529, 11 },
        { 300, 1 }, { 301, 2 }, { 302, 3 }, { 303, 4 }, { 304, 5 }, { 305, 6 }, { 306, 7 }, { 307, 8 }, { 308, 9 }, { 309, 10 }, { 530, 11 },
        { 310, 1 }, { 311, 2 }, { 312, 3 }, { 313, 4 }, { 314, 5 }, { 315, 6 }, { 316, 7 }, { 317, 8 }, { 318, 9 }, { 319, 10 }, { 531, 11 },
        { 320, 1 }, { 321, 2 }, { 322, 3 }, { 323, 4 }, { 324, 5 }, { 325, 6 }, { 326, 7 }, { 327, 8 }, { 328, 9 }, { 329, 10 }, { 532, 11 },
        { 330, 1 }, { 331, 2 }, { 332, 3 }, { 333, 4 }, { 334, 5 }, { 335, 6 }, { 336, 7 }, { 337, 8 }, { 338, 9 }, { 339, 10 }, { 533, 11 },
        { 340, 1 }, { 341, 2 }, { 342, 3 }, { 343, 4 }, { 344, 5 }, { 345, 6 }, { 346, 7 }, { 347, 8 }, { 348, 9 }, { 349, 10 }, { 534, 11 },
        { 350, 1 }, { 351, 2 }, { 352, 3 }, { 353, 4 }, { 354, 5 }, { 355, 6 }, { 356, 7 }, { 357, 8 }, { 358, 9 }, { 359, 10 }, { 535, 11 },
        { 360, 1 }, { 361, 2 }, { 362, 3 }, { 363, 4 }, { 364, 5 }, { 365, 6 }, { 366, 7 }, { 367, 8 }, { 368, 9 }, { 369, 10 }, { 536, 11 },
        { 370, 1 }, { 371, 2 }, { 372, 3 }, { 373, 4 }, { 374, 5 }, { 375, 6 }, { 376, 7 }, { 377, 8 }, { 378, 9 }, { 379, 10 }, { 537, 11 },
        { 380, 1 }, { 381, 2 }, { 382, 3 }, { 383, 4 }, { 384, 5 }, { 385, 6 }, { 386, 7 }, { 387, 8 }, { 388, 9 }, { 389, 10 }, { 538, 11 },
        { 390, 1 }, { 391, 2 }, { 392, 3 }, { 393, 4 }, { 394, 5 }, { 395, 6 }, { 396, 7 }, { 397, 8 }, { 398, 9 }, { 399, 10 }, { 539, 11 },
        { 400, 1 }, { 401, 2 }, { 402, 3 }, { 403, 4 }, { 404, 5 }, { 405, 6 }, { 406, 7 }, { 407, 8 }, { 408, 9 }, { 409, 10 }, { 540, 11 },
        { 410, 1 }, { 411, 2 }, { 412, 3 }, { 413, 4 }, { 414, 5 }, { 415, 6 }, { 416, 7 }, { 417, 8 }, { 418, 9 }, { 419, 10 }, { 541, 11 },
        { 420, 1 }, { 421, 2 }, { 422, 3 }, { 423, 4 }, { 424, 5 }, { 425, 6 }, { 426, 7 }, { 427, 8 }, { 428, 9 }, { 429, 10 }, { 542, 11 },
        { 430, 1 }, { 431, 2 }, { 432, 3 }, { 433, 4 }, { 434, 5 }, { 435, 6 }, { 436, 7 }, { 437, 8 }, { 438, 9 }, { 439, 10 }, { 543, 11 },
        { 440, 1 }, { 441, 2 }, { 442, 3 }, { 443, 4 }, { 444, 5 }, { 445, 6 }, { 446, 7 }, { 447, 8 }, { 448, 9 }, { 449, 10 }, { 544, 11 },
        { 450, 1 }, { 451, 2 }, { 452, 3 }, { 453, 4 }, { 454, 5 }, { 455, 6 }, { 456, 7 }, { 457, 8 }, { 458, 9 }, { 459, 10 }, { 555, 11 },
        { 460, 1 }, { 461, 2 }, { 462, 3 }, { 463, 4 }, { 464, 5 }, { 465, 6 }, { 466, 7 }, { 467, 8 }, { 468, 9 }, { 469, 10 }, { 546, 11 },
        { 470, 1 }, { 471, 2 }, { 472, 3 }, { 473, 4 }, { 474, 5 }, { 475, 6 }, { 476, 7 }, { 477, 8 }, { 478, 9 }, { 479, 10 }, { 547, 11 },
        { 480, 1 }, { 481, 2 }, { 482, 3 }, { 483, 4 }, { 484, 5 }, { 485, 6 }, { 486, 7 }, { 487, 8 }, { 488, 9 }, { 489, 10 }, { 548, 11 },
        { 490, 1 }, { 491, 2 }, { 492, 2 }, { 493, 4 }, { 494, 5 }, { 495, 6 }, { 496, 7 }, { 497, 8 }, { 498, 9 }, { 499, 10 }, { 549, 11 }
    }
};
#endif
/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_SamplerCtx_t BSP_ATTR_ALIGNMENT(4) svc_PwrMon_ctx;

#define SVC_PWRMON_SAMPLER_SET_COMPLETE(_ctx)                \
    ( ((_ctx)->pktInfoArray[(_ctx)->pktIdx].iBitmap == 0) && \
      ((_ctx)->pktInfoArray[(_ctx)->pktIdx].vBitmap == 0) )

/*==============================================================================
 *                             Local Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_PwrMon_samplerPktSentCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_PKT_SND_DONE();
    svc_PwrMon_SamplerCtx_t* ctx = (svc_PwrMon_SamplerCtx_t*)cbData;
    ctx->pktRemain = 0;
    return;
}

/*============================================================================*/
// Called at the beginning of the fixed time sampling callback, before making all i2c requests
bool_t
svc_PwrMon_checkForCompletion( svc_PwrMon_SamplerCtx_t* ctx )
{
    svc_PwrMon_SamplerPacket_t*  pktPtr;
    svc_PwrMon_SamplerPktInfo_t* pktInfoPtr;

    BSP_TRACE_PWRMON_SAMPLER_SET_DONE_ENTER();
    // Quick exit on first time after starting
    if( ctx->firstSample == true )
    {
        ctx->firstSample = false;
        BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
        return( false );
    }

    // Unconditionally increment the smplSet and smplNum
    pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];
    pktInfoPtr->smplSet++;
    ctx->stats.smplNum++;

    if( SVC_PWRMON_SAMPLER_SET_COMPLETE( ctx ) == false )
    {
        // Previous sample set did not complete in time.
        // Todo: Keep track of errors per channel
        ctx->stats.smplErrNum++;
    }

    if( pktInfoPtr->smplSet == SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET )
    {
        BSP_TRACE_PWRMON_SAMPLER_PKT_DONE();

        // Make sure the previous
        ctx->stats.pktErrNum += (ctx->pktRemain != 0) ? 1 : 0;

        // Reset frame info and point to next packet
        ctx->pktPrevIdx     = ctx->pktIdx;
        ctx->pktIdx         = ((ctx->pktIdx + 1) % SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT);
        pktPtr              = &ctx->pktArray[ ctx->pktIdx ];
        pktInfoPtr          = &ctx->pktInfoArray[ ctx->pktIdx ];
        pktInfoPtr->smplSet = 0;

        // Setup next packet header
        ctx->stats.pktSndNum++;
        pktPtr->hdr.seq          = ctx->stats.pktSndNum;
        pktPtr->hdr.smplStartIdx = ctx->stats.smplNum;

        BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
        return( true );
    }

    BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
    return( false );
}

/*============================================================================*/
// Called at the end of the fixed time sampling callback, after making all i2c requests
void
svc_PwrMon_sendPrevPkt( svc_PwrMon_SamplerCtx_t* ctx )
{
    BSP_TRACE_PWRMON_SAMPLER_PKT_SND_ENTER();

    ctx->pktRemain = ctx->pktLen;
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
    ctx->pktReadyCb( (void*)&svc_PwrMon_tstPkt,
                     sizeof(svc_PwrMon_tstPkt),
                     svc_PwrMon_samplerPktSentCallback,
                     ctx );
#else
    ctx->pktReadyCb( &ctx->pktArray[ ctx->pktPrevIdx ],
                     ctx->pktLen,
                     svc_PwrMon_samplerPktSentCallback,
                     ctx );
#endif

    BSP_TRACE_PWRMON_SAMPLER_PKT_SND_EXIT();
}

/*============================================================================*/
void svc_PwrMon_voltageCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].vBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_currentCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].iBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_EXIT();
    return;
}

/*============================================================================*/
// Main interrupt driving the sampling of the Power Monitor ICs
void svc_PwrMon_samplerCallback( bsp_TimerGp_TimerId_t    timerId,
                                 bsp_TimerGp_SubTimerId_t subTimerId,
                                 uint32_t                 mask )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;

    bool_t sndPkt = svc_PwrMon_checkForCompletion( ctx );

    // Request all configured channels for next sample Set
    svc_PwrMon_SamplerPacket_t*  pktPtr     = &ctx->pktArray[ ctx->pktIdx ];
    svc_PwrMon_SamplerPktInfo_t* pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];
    uint16_t chIdx = 0;
    uint16_t offsetIdx = (pktInfoPtr->smplSet * ctx->numCh);

    // Reset the sample set bitmaps
    pktInfoPtr->vBitmap = ctx->chBitmap;
    pktInfoPtr->iBitmap = ctx->chBitmap;
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            dev_PwrMon_channelCurrentRead( chId,
                                           (dev_PwrMon_Data_t*)&pktPtr->data[ (offsetIdx + chIdx) ].i,
                                           svc_PwrMon_currentCallback,
                                           (void*)(uint32_t)chId );

            dev_PwrMon_channelBusVoltageRead( chId,
                                              (dev_PwrMon_Data_t*)&pktPtr->data[ (offsetIdx + chIdx) ].v,
                                              svc_PwrMon_voltageCallback,
                                              (void*)(uint32_t)chId );
            chIdx++;
        }
    }

    // Check if packet needs to be sent
    if( sndPkt == true )
    {
        svc_PwrMon_sendPrevPkt( ctx );
    }
    BSP_TRACE_PWRMON_SAMPLER_SET_EXIT();
    return;
}


/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_samplerInit( dev_PwrMon_SamplerPktReady_t callback )
{
    BSP_ASSERT(callback != NULL);
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
    memset( &svc_PwrMon_ctx, 0, sizeof(svc_PwrMon_ctx) );
    svc_PwrMon_ctx.pktReadyCb = callback;
}

/*============================================================================*/
void svc_PwrMon_samplerStop( void )
{
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
}

/*============================================================================*/
svc_PwrMon_SamplerStats_t*
svc_PwrMon_samplerStatsPtr( void )
{
    return( &svc_PwrMon_ctx.stats );
}

/*============================================================================*/
void svc_PwrMon_samplerStart( svc_PwrMon_ChannelBitmap_t     chBitmap,
                              svc_PwrMon_SamplerSampleSize_t sampleSize )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;

    memset( &ctx->pktInfoArray[0], 0, sizeof(ctx->pktInfoArray) );
    memset( &ctx->stats, 0, sizeof(ctx->stats) );

    // Reset Frame
    ctx->firstSample = true;
    ctx->chBitmap    = chBitmap;
    ctx->sampleSize  = sampleSize;

    // Count the channels
    ctx->numCh = 0;
    while( chBitmap )
    {
        ctx->numCh += (chBitmap & 0x01);
        chBitmap >>= 1;
    }

    // Compute the packet length in bytes
    ctx->pktLen = ( sizeof(svc_PwrMon_SamplerPacketHdr_t) +
                    ( ctx->numCh *
                      SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET *
                      sampleSize ) );

    // Setup static portion of headers
    for( int i=0; i<SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT; i++ )
    {
        svc_PwrMon_SamplerPacket_t* pktPtr = &ctx->pktArray[ i ];
        pktPtr->hdr.numCh      = ctx->numCh;
        pktPtr->hdr.numSmplSet = SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET;
        pktPtr->hdr.sampleSize = ctx->sampleSize;
        pktPtr->hdr.chBitmap   = ctx->chBitmap;

        // These fields change per packet, initialized to 0
        pktPtr->hdr.seq          = ctx->stats.pktSndNum;
        pktPtr->hdr.smplStartIdx = ctx->stats.smplNum;
    }

    // Get each power monitor IC set to the proper register with a dummy read
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        dev_PwrMon_Data_t tmp;
        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            dev_PwrMon_channelCurrentRead( chId, &tmp, NULL, NULL );
            dev_PwrMon_channelBusVoltageRead( chId, &tmp, NULL, NULL );
        }
    }

    bsp_TimerGp_startCountdown( SVC_PWRMON_SAMPLER_TIMER_ID,
                                BSP_TIMERGP_TYPE_PERIODIC,
                                BSP_TIMERGP_DMA_CTRL_NONE,
                                SVC_PWRMON_SAMPLER_TICK_US,
                                svc_PwrMon_samplerCallback );
}
#endif