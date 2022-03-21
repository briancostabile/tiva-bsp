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
 * @file bsp_TimerGp_tm4c123.h
 * @brief Contains the tm4c123 core General Purpose timer defines.
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_Timer.h"
#include "driverlib/pin_map.h"

/*============================================================================*/
#define BSP_TIMERGP_ID_0 0
#define BSP_TIMERGP_ID_1 1
#define BSP_TIMERGP_ID_2 2
#define BSP_TIMERGP_ID_3 3
#define BSP_TIMERGP_ID_4 4
#define BSP_TIMERGP_ID_5 5
typedef uint8_t bsp_TimerGp_TimerId_t;
#define BSP_TIMERGP_ID_NUM_TIMERS 6

/*============================================================================*/
#define BSP_TIMERGP_SUB_ID_A 0
#define BSP_TIMERGP_SUB_ID_B 1
typedef uint8_t bsp_TimerGp_SubTimerId_t;
#define BSP_TIMERGP_ID_NUM_SUB_TIMERS 2

/*============================================================================*/

/*============================================================================*/
typedef void (*bsp_TimerGp_TimeoutHandler_t)(
    bsp_TimerGp_TimerId_t    timerId,
    bsp_TimerGp_SubTimerId_t subTimerId,
    uint32_t                 mask);

/*============================================================================*/
typedef struct {
    bsp_Interrupt_Id_t            intId;
    bsp_TimerGp_TimeoutHandler_t *handler;
} bsp_TimerGp_InterruptInfo_t;

/*============================================================================*/
typedef struct {
    uint32_t                          baseAddr;
    uint32_t                          sysCtrlAddr;
    const bsp_TimerGp_InterruptInfo_t handlerTable[BSP_TIMERGP_ID_NUM_SUB_TIMERS];
} bsp_TimerGp_PlatformInfo_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer0A(void);
void bsp_TimerGp_interruptHandlerTimer0B(void);
void bsp_TimerGp_interruptHandlerTimer1A(void);
void bsp_TimerGp_interruptHandlerTimer1B(void);
void bsp_TimerGp_interruptHandlerTimer2A(void);
void bsp_TimerGp_interruptHandlerTimer2B(void);
void bsp_TimerGp_interruptHandlerTimer3A(void);
void bsp_TimerGp_interruptHandlerTimer3B(void);
void bsp_TimerGp_interruptHandlerTimer4A(void);
void bsp_TimerGp_interruptHandlerTimer4B(void);
void bsp_TimerGp_interruptHandlerTimer5A(void);
void bsp_TimerGp_interruptHandlerTimer5B(void);
