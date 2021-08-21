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
 * @file svc_SamplerEh.h
 * @brief Contains the message interface related to the Samplerity service
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
// Event handler message IDs
#define SVC_SAMPLEREH_SAMPLE_START_REQ SVC_MSGFWK_MSG_ID_BUILD_REQ( SVC_EHID_SAMPLER, 0 )
#define SVC_SAMPLEREH_SAMPLE_START_CNF SVC_MSGFWK_MSG_ID_BUILD_CNF( SVC_EHID_SAMPLER, 1 )

#define SVC_SAMPLEREH_SAMPLE_STOP_REQ SVC_MSGFWK_MSG_ID_BUILD_REQ( SVC_EHID_SAMPLER, 2 )
#define SVC_SAMPLEREH_SAMPLE_STOP_CNF SVC_MSGFWK_MSG_ID_BUILD_CNF( SVC_EHID_SAMPLER, 3 )

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
// Event handler message structures
typedef struct BSP_ATTR_PACKED svc_SamplerEh_SampleStartReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_SamplerEh_MeasSampleStartReq_t;

typedef struct BSP_ATTR_PACKED svc_SamplerEh_SampleStartCnf_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_SamplerEh_MeasSampleStartCnf_t;

typedef struct BSP_ATTR_PACKED svc_SamplerEh_SampleStopReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_SamplerEh_MeasSampleStopReq_t;

typedef struct BSP_ATTR_PACKED svc_SamplerEh_SampleStopCnf_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_SamplerEh_MeasSampleStopCnf_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_SamplerEh_info;