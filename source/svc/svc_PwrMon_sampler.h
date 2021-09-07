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
 * @file svc_PwrMon_sampler.h
 * @brief Contains API for the Power Monitor sampler object
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "dev_PwrMon.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_SAMPLER_TICK_US 400

/*============================================================================*/
// Max channels per sample-time
#define SVC_PWRMON_SAMPLER_CHANNELS_MAX 11

// Max Sample size in bytes (16-bit V, 16-bit I)
#define SVC_PWRMON_SAMPLER_SAMPLE_SIZE_MAX 4

// Samples Per Packet: 50 sample sets at 2500Hz = 20ms worth of samples
#define SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET 50


/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
// Callback type passed into the Packet Ready callback
typedef void (*dev_PwrMon_SamplerPktSent_t)( void* cbData );

// Callback type passed in during initialization. To be called when
// a packet is full.
typedef void (*dev_PwrMon_SamplerPktReady_t)( void*                       dataPtr,
                                              size_t                      len,
                                              dev_PwrMon_SamplerPktSent_t callback,
                                              void*                       cbData );

// Define the sizes for the sample types supported
#define SVC_PWRMON_SAMPLER_SAMPLE_SIZE_24BIT 3
#define SVC_PWRMON_SAMPLER_SAMPLE_SIZE_32BIT 4
typedef uint8_t svc_PwrMon_SamplerSampleSize_t;


/*============================================================================*/
// Packed structures for the packet sent through USB.
typedef struct BSP_ATTR_PACKED {
    uint16_t numCh      :  6;
    uint16_t numSmplSet :  6;
    uint16_t sampleSize :  4;
    uint16_t seq;
    uint32_t smplStartIdx;
    uint32_t chBitmap;
} svc_PwrMon_SamplerPacketHdr_t;

typedef struct BSP_ATTR_PACKED {
    int16_t v;
    int16_t i;
} svc_PwrMon_SamplerPacketData_t;

typedef struct BSP_ATTR_PACKED {
    svc_PwrMon_SamplerPacketHdr_t  hdr;
    svc_PwrMon_SamplerPacketData_t data[ SVC_PWRMON_SAMPLER_SAMPLE_SETS_PER_PACKET * SVC_PWRMON_SAMPLER_CHANNELS_MAX ];
} svc_PwrMon_SamplerPacket_t;

/*============================================================================*/
// Stats made available outside
typedef struct svc_PwrMon_SamplerStats_s
{
    uint64_t smplNum;
    uint32_t smplErrNum;
    uint32_t pktSndNum;
    uint32_t pktErrNum;
} svc_PwrMon_SamplerStats_t;

/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_PwrMon_samplerInit( dev_PwrMon_SamplerPktReady_t callback );

/*============================================================================*/
void
svc_PwrMon_samplerStop( void );

/*============================================================================*/
void
svc_PwrMon_samplerStart( svc_PwrMon_ChannelBitmap_t     chBitmap,
                         svc_PwrMon_SamplerSampleSize_t sampleSize );

/*============================================================================*/
svc_PwrMon_SamplerStats_t*
svc_PwrMon_samplerStatsPtr( void );
