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
 * @file bsp_TimerGp.c
 * @brief Contains Functions for configuring and accessing the General Purpose
 *        timers
 */
#include "bsp_Types.h"
#include "bsp_TimerGp.h"
#include "bsp_Mcu.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#define BSP_TIMERGP_TIMER_NUM             7
#define BSP_TIMERGP_TICKS_PER_MICROSECOND (BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ / 1000000)

/*==============================================================================
 *                                Types
 *============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const bsp_TimerGp_PlatformInfo_t bsp_TimerGp_platformInfoTable[BSP_TIMERGP_ID_NUM_TIMERS];

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_TimerGp_isrCommon(bsp_TimerGp_TimerId_t timerId, bsp_TimerGp_SubTimerId_t subTimerId)
{
    bsp_TimerGp_TimeoutHandler_t handler;
    uint32_t                     portBaseAddr;
    uint32_t                     tmpMis;

    BSP_TRACE_INT_ENTER();
    BSP_TRACE_TIMER_INT_ENTER();
    portBaseAddr = bsp_TimerGp_platformInfoTable[timerId].baseAddr;
    handler      = *bsp_TimerGp_platformInfoTable[timerId].handlerTable[subTimerId].handler;

    /* Read masked interrupt register and clear */
    tmpMis = MAP_TimerIntStatus(portBaseAddr, false);
    MAP_TimerIntClear(portBaseAddr, tmpMis);

    /* Call a handler */
    handler(timerId, subTimerId, tmpMis);

    BSP_TRACE_TIMER_INT_EXIT();
    BSP_TRACE_INT_EXIT();
    return;
}

/*============================================================================*/
void bsp_TimerGp_init(void)
{
    bsp_TimerGp_TimerId_t    timerId;
    bsp_TimerGp_SubTimerId_t subTimerId;

    /* Every port is enabled and the interrupts cleared */
    for (timerId = 0; timerId < DIM(bsp_TimerGp_platformInfoTable); timerId++) {
        /* Enable all of the General Purpose Timer peripheral blocks */
        MAP_SysCtlPeripheralEnable(bsp_TimerGp_platformInfoTable[timerId].sysCtrlAddr);

        for (subTimerId = 0; subTimerId < BSP_TIMERGP_ID_NUM_SUB_TIMERS; subTimerId++) {
            const bsp_TimerGp_InterruptInfo_t *handlerPtr =
                &bsp_TimerGp_platformInfoTable[timerId].handlerTable[subTimerId];

            /* Disable and clear all interrupts */
            bsp_Interrupt_disable(handlerPtr->intId);
            bsp_Interrupt_clearPending(handlerPtr->intId);

            /* Clear the callback table */
            *handlerPtr->handler = NULL;

            /* Re-Enable all interrupts */
            bsp_Interrupt_enable(handlerPtr->intId);
        }

        /* Wait for the peripheral to be ready in the system controller before moving on */
        while (MAP_SysCtlPeripheralReady(bsp_TimerGp_platformInfoTable[timerId].sysCtrlAddr) ==
               FALSE)
            ;
    }

    return;
}

/*============================================================================*/
void bsp_TimerGp_startCountdown(
    bsp_TimerGp_TimerId_t        timerId,
    bsp_TimerGp_Type_t           type,
    bsp_TimerGp_DmaCtrl_t        dmaCtrlMask,
    bsp_TimerGp_Timeout_t        timeout,
    bsp_TimerGp_TimeoutHandler_t callback)
{
    const bsp_TimerGp_PlatformInfo_t * timerInfoPtr = &bsp_TimerGp_platformInfoTable[timerId];
    const bsp_TimerGp_InterruptInfo_t *timerIntInfoPtr =
        &bsp_TimerGp_platformInfoTable[timerId].handlerTable[BSP_TIMERGP_SUB_ID_A];

    BSP_TRACE_TIMER_START_ENTER();
    MAP_TimerDisable(timerInfoPtr->baseAddr, TIMER_A);
    MAP_TimerIntDisable(timerInfoPtr->baseAddr, TIMER_A);
    MAP_TimerIntClear(timerInfoPtr->baseAddr, TIMER_A);

    MAP_TimerClockSourceSet(timerInfoPtr->baseAddr, TIMER_CLOCK_SYSTEM);
    MAP_TimerConfigure(
        timerInfoPtr->baseAddr,
        (type == BSP_TIMERGP_TYPE_PERIODIC) ? TIMER_CFG_PERIODIC : TIMER_CFG_ONE_SHOT);
    MAP_TimerDMAEventSet(timerInfoPtr->baseAddr, dmaCtrlMask);

    *timerIntInfoPtr->handler = callback;
    MAP_TimerLoadSet(
        timerInfoPtr->baseAddr, TIMER_A, (timeout * BSP_TIMERGP_TICKS_PER_MICROSECOND));

    MAP_TimerIntEnable(timerInfoPtr->baseAddr, TIMER_A);
    MAP_TimerEnable(timerInfoPtr->baseAddr, TIMER_A);
    BSP_TRACE_TIMER_START_EXIT();
    return;
}

/*============================================================================*/
void bsp_TimerGp_stop(bsp_TimerGp_TimerId_t timerId)
{
    const bsp_TimerGp_PlatformInfo_t * timerInfoPtr = &bsp_TimerGp_platformInfoTable[timerId];
    const bsp_TimerGp_InterruptInfo_t *timerIntInfoPtr =
        &bsp_TimerGp_platformInfoTable[timerId].handlerTable[BSP_TIMERGP_SUB_ID_A];

    BSP_TRACE_TIMER_STOP_ENTER();
    MAP_TimerDisable(timerInfoPtr->baseAddr, TIMER_A);
    MAP_TimerIntDisable(timerInfoPtr->baseAddr, TIMER_A);
    MAP_TimerIntClear(timerInfoPtr->baseAddr, TIMER_A);

    *timerIntInfoPtr->handler = NULL;
    BSP_TRACE_TIMER_STOP_EXIT();
    return;
}
