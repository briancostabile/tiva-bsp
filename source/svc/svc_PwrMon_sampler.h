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
#include "bsp_Pragma.h"
#include "dev_PwrMon.h"
#include "svc_MsgFwk.h"
#include "svc_PwrMonEh.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_SAMPLER_TICK_US 400

/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_samplerInit(void);

/*============================================================================*/
void svc_PwrMon_samplerStop(void);

/*============================================================================*/
void svc_PwrMon_samplerStart(svc_PwrMonEh_ChBitmap_t chBitmap, uint32_t smplcnt);

/*============================================================================*/
svc_PwrMonEh_SamplerStats_t *svc_PwrMon_samplerStatsPtr(void);

/*============================================================================*/
void svc_PwrMon_samplerStatsReset(void);
