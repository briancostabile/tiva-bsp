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
 * @file bsp_Gpio_sensorhub.c
 * @brief Contains Configuration table for the supported IO ports on this platform
 */
#include "bsp_Gpio.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"

#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Global Data
 *============================================================================*/
/*============================================================================*/
bsp_Gpio_InputHandler_t bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_NUM_PORTS][BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT];

/*============================================================================*/
const bsp_Gpio_PlatformPortInfo_t bsp_Gpio_platformPortInfoTable[ BSP_GPIO_PORT_ID_NUM_PORTS ] =
{
    { GPIO_PORTA_BASE, SYSCTL_PERIPH_GPIOA, BSP_INTERRUPT_ID_GPIOA, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_A][0]) },
    { GPIO_PORTB_BASE, SYSCTL_PERIPH_GPIOB, BSP_INTERRUPT_ID_GPIOB, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_B][0]) },
    { GPIO_PORTC_BASE, SYSCTL_PERIPH_GPIOC, BSP_INTERRUPT_ID_GPIOC, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_C][0]) },
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, BSP_INTERRUPT_ID_GPIOD, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_D][0]) },
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, BSP_INTERRUPT_ID_GPIOE, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_E][0]) },
    { GPIO_PORTF_BASE, SYSCTL_PERIPH_GPIOF, BSP_INTERRUPT_ID_GPIOF, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_F][0]) }
};

/*==============================================================================
 *                               Public functions
 *============================================================================*/
/*============================================================================*/
// Platform specific initializations
void
bsp_Gpio_initPlatform( void )
{
    /* Configure No Connect pins as no-pull inputs */
    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPB6,
                          BSP_GPIO_BIT_MASK_NCPB6,
                          FALSE, BSP_GPIO_PULL_NONE );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_NCPB7,
                          BSP_GPIO_BIT_MASK_NCPB7,
                          FALSE, BSP_GPIO_PULL_NONE );
    return;
}