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
 * @file bsp_Trace.c
 * @brief Contains IO port initialization function to configure all Test point
 *        IO pins to outputs
 */

#include "bsp_Types.h"
#include "bsp_Trace.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Trace_init( void )
{
    uint8_t i;

    for( i=0; i<DIM(bsp_Trace_ioInfoTable); i++ )
    {
        /* For each port configure the test points as output 2ma drive
         * and set each output to low
         */
        bsp_Gpio_configOutput( bsp_Trace_ioInfoTable[i].portId,
                               bsp_Trace_ioInfoTable[i].mask,
                               FALSE, BSP_GPIO_DRIVE_2MA );

        bsp_Gpio_write( bsp_Trace_ioInfoTable[i].portId,
                        bsp_Trace_ioInfoTable[i].mask,
                        0 );
    }

    return;
}
