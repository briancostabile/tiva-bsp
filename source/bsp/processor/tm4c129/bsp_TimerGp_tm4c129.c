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
 * @file bsp_TimerGp_tm4c129.c
 * @brief Contains Functions for configuring and accessing the General Purpose Timer
 */
#include "bsp_TimerGp.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"
#include <string.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Global Data
 *============================================================================*/
/*============================================================================*/
bsp_TimerGp_TimeoutHandler_t bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_NUM_TIMERS]
                                                            [BSP_TIMERGP_ID_NUM_SUB_TIMERS];

/*============================================================================*/
const bsp_TimerGp_PlatformInfo_t bsp_TimerGp_platformInfoTable[BSP_TIMERGP_ID_NUM_TIMERS] = {
    {TIMER0_BASE,
     SYSCTL_PERIPH_TIMER0,
     {{INT_TIMER0A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_0][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER0B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_0][BSP_TIMERGP_SUB_ID_B])}}},
    {TIMER1_BASE,
     SYSCTL_PERIPH_TIMER1,
     {{INT_TIMER1A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_1][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER1B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_1][BSP_TIMERGP_SUB_ID_B])}}},
    {TIMER2_BASE,
     SYSCTL_PERIPH_TIMER2,
     {{INT_TIMER1A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_2][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER1B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_2][BSP_TIMERGP_SUB_ID_B])}}},
    {TIMER3_BASE,
     SYSCTL_PERIPH_TIMER3,
     {{INT_TIMER1A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_3][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER1B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_3][BSP_TIMERGP_SUB_ID_B])}}},
    {TIMER4_BASE,
     SYSCTL_PERIPH_TIMER4,
     {{INT_TIMER1A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_4][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER1B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_4][BSP_TIMERGP_SUB_ID_B])}}},
    {TIMER5_BASE,
     SYSCTL_PERIPH_TIMER5,
     {{INT_TIMER1A, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_5][BSP_TIMERGP_SUB_ID_A])},
      {INT_TIMER1B, &(bsp_TimerGp_timeoutHandlerTable[BSP_TIMERGP_ID_5][BSP_TIMERGP_SUB_ID_B])}}}};

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
extern void bsp_TimerGp_isrCommon(
    bsp_TimerGp_TimerId_t    timerId,
    bsp_TimerGp_SubTimerId_t subTimerId);

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer0A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_0, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer0B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_0, BSP_TIMERGP_SUB_ID_B);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer1A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_1, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer1B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_1, BSP_TIMERGP_SUB_ID_B);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer2A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_2, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer2B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_2, BSP_TIMERGP_SUB_ID_B);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer3A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_3, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer3B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_3, BSP_TIMERGP_SUB_ID_B);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer4A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_4, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer4B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_4, BSP_TIMERGP_SUB_ID_B);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer5A(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_5, BSP_TIMERGP_SUB_ID_A);
    return;
}

/*============================================================================*/
void bsp_TimerGp_interruptHandlerTimer5B(void)
{
    bsp_TimerGp_isrCommon(BSP_TIMERGP_ID_5, BSP_TIMERGP_SUB_ID_B);
    return;
}
