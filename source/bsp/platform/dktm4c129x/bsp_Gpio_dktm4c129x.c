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
 * @file bsp_Gpio_ektm4c123gxl.c
 * @brief Contains Configuration table for the supported IO ports on this platform
 */
#include "bsp_Gpio.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"

#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                               Public functions
 *============================================================================*/
/*============================================================================*/
// Platform specific initializations
void
bsp_Gpio_initPlatform( void )
{
    /* Configure No Connect pins as no-pull inputs */
    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPT4,
                          BSP_GPIO_BIT_MASK_NCPT4,
                          FALSE, BSP_GPIO_PULL_NONE );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPT5,
                          BSP_GPIO_BIT_MASK_NCPT5,
                          FALSE, BSP_GPIO_PULL_NONE );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPT6,
                          BSP_GPIO_BIT_MASK_NCPT6,
                          FALSE, BSP_GPIO_PULL_NONE );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPT7,
                          BSP_GPIO_BIT_MASK_NCPT7,
                          FALSE, BSP_GPIO_PULL_NONE );
    return;
}