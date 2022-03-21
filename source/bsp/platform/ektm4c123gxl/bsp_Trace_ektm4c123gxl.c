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
 * @file bsp_Trace_ektm4c123gxl.c
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
/* clang-format off */
const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS]=
{
    { BSP_GPIO_PORT_ID(PA0), (BSP_GPIO_MASK(TPA2) | BSP_GPIO_MASK(TPA3) | BSP_GPIO_MASK(TPA4) | BSP_GPIO_MASK(TPA5) | BSP_GPIO_MASK(TPA6) | BSP_GPIO_MASK(TPA7)) },
    { BSP_GPIO_PORT_ID(PB0), (BSP_GPIO_MASK(TPB2) | BSP_GPIO_MASK(TPB3) | BSP_GPIO_MASK(TPB4) | BSP_GPIO_MASK(TPB5)) },
    { BSP_GPIO_PORT_ID(PC0), (BSP_GPIO_MASK(TPC4) | BSP_GPIO_MASK(TPC5) | BSP_GPIO_MASK(TPC6) | BSP_GPIO_MASK(TPC7)) },
    { BSP_GPIO_PORT_ID(PD0), (BSP_GPIO_MASK(TPD2) | BSP_GPIO_MASK(TPD3) | BSP_GPIO_MASK(TPD6) | BSP_GPIO_MASK(TPD7)) },
    { BSP_GPIO_PORT_ID(PE0), (BSP_GPIO_MASK(TPE0) | BSP_GPIO_MASK(TPE1) | BSP_GPIO_MASK(TPE2) | BSP_GPIO_MASK(TPE3) | BSP_GPIO_MASK(TPE4) | BSP_GPIO_MASK(TPE5)) },
    { BSP_GPIO_PORT_ID(PF0), 0x00000000 }
};

size_t bsp_Trace_ioInfoTableLen(void)
{
    return(DIM(bsp_Trace_ioInfoTable));
}
