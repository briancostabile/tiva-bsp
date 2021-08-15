/**
 * Copyright 2017 Brian Costabile
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
 * @file bsp_Trace_dktm4c129x.c
 * @brief Contains table of available test point IOs
 */

#include "bsp_Types.h"
#include "bsp_Trace.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                               Globals
 *============================================================================*/
/*============================================================================*/
/* One entry for each IO port. */
const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS]=
{
    { BSP_GPIO_PORT_ID(PA0), (BSP_GPIO_MASK(TPA2) | BSP_GPIO_MASK(TPA3) | BSP_GPIO_MASK(TPA4) | BSP_GPIO_MASK(TPA5) | BSP_GPIO_MASK(TPA6) | BSP_GPIO_MASK(TPA7)) },
    { BSP_GPIO_PORT_ID(PB0), (BSP_GPIO_MASK(TPB2) | BSP_GPIO_MASK(TPB3) | BSP_GPIO_MASK(TPB4) | BSP_GPIO_MASK(TPB5) | BSP_GPIO_MASK(TPB6) | BSP_GPIO_MASK(TPB7)) },
    { BSP_GPIO_PORT_ID(PC0), (BSP_GPIO_MASK(TPC4) | BSP_GPIO_MASK(TPC5) | BSP_GPIO_MASK(TPC6) | BSP_GPIO_MASK(TPC7)) },
    { BSP_GPIO_PORT_ID(PD0), (BSP_GPIO_MASK(TPD0) | BSP_GPIO_MASK(TPD1) | BSP_GPIO_MASK(TPD2) | BSP_GPIO_MASK(TPD3) | BSP_GPIO_MASK(TPD4) | BSP_GPIO_MASK(TPD5)) },
    { BSP_GPIO_PORT_ID(PE0), (BSP_GPIO_MASK(TPE0) | BSP_GPIO_MASK(TPE1) | BSP_GPIO_MASK(TPE2) | BSP_GPIO_MASK(TPE3) | BSP_GPIO_MASK(TPE4) | BSP_GPIO_MASK(TPE6) | BSP_GPIO_MASK(TPE7)) },
    { BSP_GPIO_PORT_ID(PF0), (BSP_GPIO_MASK(TPF0) | BSP_GPIO_MASK(TPF3) | BSP_GPIO_MASK(TPF4) | BSP_GPIO_MASK(TPF5) | BSP_GPIO_MASK(TPF6) | BSP_GPIO_MASK(TPF7)) },
    { BSP_GPIO_PORT_ID(PG0), (BSP_GPIO_MASK(TPG0) | BSP_GPIO_MASK(TPG1) | BSP_GPIO_MASK(TPG2) | BSP_GPIO_MASK(TPG3) | BSP_GPIO_MASK(TPG4) | BSP_GPIO_MASK(TPG5) | BSP_GPIO_MASK(TPG6) | BSP_GPIO_MASK(TPG7)) },
    { BSP_GPIO_PORT_ID(PH0), (BSP_GPIO_MASK(TPH0) | BSP_GPIO_MASK(TPH1) | BSP_GPIO_MASK(TPH2) | BSP_GPIO_MASK(TPH3) | BSP_GPIO_MASK(TPH4) | BSP_GPIO_MASK(TPH5) | BSP_GPIO_MASK(TPH6) | BSP_GPIO_MASK(TPH7)) },
    { BSP_GPIO_PORT_ID(PJ0), (BSP_GPIO_MASK(TPJ0) | BSP_GPIO_MASK(TPJ1) | BSP_GPIO_MASK(TPJ2) | BSP_GPIO_MASK(TPJ3) | BSP_GPIO_MASK(TPJ4) | BSP_GPIO_MASK(TPJ5) | BSP_GPIO_MASK(TPJ6) | BSP_GPIO_MASK(TPJ7)) },
    { BSP_GPIO_PORT_ID(PK0), (BSP_GPIO_MASK(TPK0) | BSP_GPIO_MASK(TPK1) | BSP_GPIO_MASK(TPK2) | BSP_GPIO_MASK(TPK3) | BSP_GPIO_MASK(TPK5) | BSP_GPIO_MASK(TPK7)) },
    { BSP_GPIO_PORT_ID(PL0), (BSP_GPIO_MASK(TPL0) | BSP_GPIO_MASK(TPL1) | BSP_GPIO_MASK(TPL2) | BSP_GPIO_MASK(TPL3) | BSP_GPIO_MASK(TPL4) | BSP_GPIO_MASK(TPL5)) },
    { BSP_GPIO_PORT_ID(PM0), (BSP_GPIO_MASK(TPM0) | BSP_GPIO_MASK(TPM1) | BSP_GPIO_MASK(TPM2) | BSP_GPIO_MASK(TPM3) | BSP_GPIO_MASK(TPM4) | BSP_GPIO_MASK(TPM5) | BSP_GPIO_MASK(TPM6) | BSP_GPIO_MASK(TPM7)) },
    { BSP_GPIO_PORT_ID(PN0), (BSP_GPIO_MASK(TPN0) | BSP_GPIO_MASK(TPN1) | BSP_GPIO_MASK(TPN2) | BSP_GPIO_MASK(TPN4) | BSP_GPIO_MASK(TPN6) | BSP_GPIO_MASK(TPN7)) },
    { BSP_GPIO_PORT_ID(PP0), (BSP_GPIO_MASK(TPP0) | BSP_GPIO_MASK(TPP2) | BSP_GPIO_MASK(TPP3) | BSP_GPIO_MASK(TPP4) | BSP_GPIO_MASK(TPP5) | BSP_GPIO_MASK(TPP6) | BSP_GPIO_MASK(TPP7)) },
    { BSP_GPIO_PORT_ID(PQ0), (BSP_GPIO_MASK(TPQ0) | BSP_GPIO_MASK(TPQ1) | BSP_GPIO_MASK(TPQ2) | BSP_GPIO_MASK(TPQ3) | BSP_GPIO_MASK(TPQ5) | BSP_GPIO_MASK(TPQ6)) },
    { BSP_GPIO_PORT_ID(PR0), (BSP_GPIO_MASK(TPR0) | BSP_GPIO_MASK(TPR1) | BSP_GPIO_MASK(TPR2) | BSP_GPIO_MASK(TPR3) | BSP_GPIO_MASK(TPR4) | BSP_GPIO_MASK(TPR5) | BSP_GPIO_MASK(TPR6) | BSP_GPIO_MASK(TPR7)) },
    { BSP_GPIO_PORT_ID(PS0), (BSP_GPIO_MASK(TPS0) | BSP_GPIO_MASK(TPS1) | BSP_GPIO_MASK(TPS2) | BSP_GPIO_MASK(TPS3) | BSP_GPIO_MASK(TPS4) | BSP_GPIO_MASK(TPS5) | BSP_GPIO_MASK(TPS6) | BSP_GPIO_MASK(TPS7)) },
    { BSP_GPIO_PORT_ID(PT0), (BSP_GPIO_MASK(TPT0) | BSP_GPIO_MASK(TPT1) | BSP_GPIO_MASK(TPT2) | BSP_GPIO_MASK(TPT3)) }
};
