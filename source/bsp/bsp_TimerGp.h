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
 * @file bsp_TimerGp.h
 * @brief Contains types and prototypes to access the General Purpose Timers
 */
#pragma once

#include "bsp_Platform.h"

#include BUILD_INCLUDE_STRING( bsp_TimerGp_, PLATFORM_CORE )

/*==============================================================================
 *                             Defines
 *============================================================================*/

/*==============================================================================
 *                             Macros
 *============================================================================*/

/*==============================================================================
 *                               Types
 *============================================================================*/
/*============================================================================*/
typedef uint32_t bsp_TimerGp_Timeout_t;

/*============================================================================*/
#define BSP_TIMERGP_TYPE_PERIODIC 0
#define BSP_TIMERGP_TYPE_ONE_SHOT 1
typedef uint8_t bsp_TimerGp_Type_t;

/*============================================================================*/
#define BSP_TIMERGP_CLK_SRC_SYSTEM 0
#define BSP_TIMERGP_CLK_SRC_PIOSC  1
typedef uint8_t bsp_TimerGp_ClkSrc_t;

/*============================================================================*/
#define BSP_TIMERGP_DMA_CTRL_NONE        0x00000000
#define BSP_TIMERGP_DMA_CTRL_TIMEOUT     0x00000001
#define BSP_TIMERGP_DMA_CTRL_CAP_MATCH   0x00000002
#define BSP_TIMERGP_DMA_CTRL_CAP_EVENT   0x00000004
#define BSP_TIMERGP_DMA_CTRL_CAP_RTC     0x00000008
#define BSP_TIMERGP_DMA_CTRL_MODE_MATCH  0x00000010
typedef uint8_t bsp_TimerGp_DmaCtrl_t;


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_TimerGp_init( void );

/*============================================================================*/
void
bsp_TimerGp_startCountdown( bsp_TimerGp_TimerId_t        timerId,
                            bsp_TimerGp_Type_t           type,
                            bsp_TimerGp_DmaCtrl_t        dmaCtrl,
                            bsp_TimerGp_Timeout_t        timeout, //in microseconds
                            bsp_TimerGp_TimeoutHandler_t callback );

/*============================================================================*/
void
bsp_TimerGp_stop( bsp_TimerGp_TimerId_t timerId );
