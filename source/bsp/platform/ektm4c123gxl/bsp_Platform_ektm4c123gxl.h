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
 * @file bsp_Platform_ektm4c123gxl.h
 */
#pragma once

#include "inc/tm4c123gh6pm.h"

#define BSP_PLATFORM_PROCESSOR_TM4C123

/*==============================================================================
 *                            Clock related defines
 *============================================================================*/
/*============================================================================*/
#define BSP_PLATFORM_OSC_PRESENT TRUE
#define BSP_PLATFORM_OSC_FREQ_HZ 16000000

/*============================================================================*/
#define BSP_PLATFORM_XOSC_PRESENT TRUE
#define BSP_PLATFORM_XOSC_FREQ_HZ 32768

/*============================================================================*/
#define BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ 80000000

/*==============================================================================
 *                            IO defines
 *============================================================================*/
#define BSP_PLATFORM_IO_UART_NUM         1
#define BSP_PLATFORM_IO_UART0_ID         0
#define BSP_PLATFORM_IO_UART0_RX_PIN_SEL 0
#define BSP_PLATFORM_IO_UART0_TX_PIN_SEL 0
#define BSP_PLATFORM_IO_UART0_BAUD       (9600 * 1) //must be 9600 for Virtual Com port built into launchpad
#define BSP_PLATFORM_IO_UART0_RX_BUF_LEN 32
#define BSP_PLATFORM_IO_UART0_TX_BUF_LEN 128


/* USB Setup */
#define BSP_PLATFORM_USB_ENABLED
#define BSP_PLATFORM_USB_CDC
//#define BSP_PLATFORM_USB_BULK
#define BSP_PLATFORM_IO_USB0_RX_BUF_LEN  512
#define BSP_PLATFORM_IO_USB0_TX_BUF_LEN  (2*1024)


#define BSP_PLATFORM_IO_LED_NUM          3
#define BSP_PLATFORM_IO_PORT_ID_LED_0    BSP_GPIO_PORT_ID_LED_R
#define BSP_PLATFORM_IO_BASE_ADDR_LED_0  BSP_GPIO_BASE_ADDR_LED_R
#define BSP_PLATFORM_IO_BIT_OFFSET_LED_0 BSP_GPIO_BIT_OFFSET_LED_R
#define BSP_PLATFORM_IO_BIT_MASK_LED_0   BSP_GPIO_BIT_MASK_LED_R

#define BSP_PLATFORM_IO_PORT_ID_LED_1    BSP_GPIO_PORT_ID_LED_B
#define BSP_PLATFORM_IO_BASE_ADDR_LED_1  BSP_GPIO_BASE_ADDR_LED_B
#define BSP_PLATFORM_IO_BIT_OFFSET_LED_1 BSP_GPIO_BIT_OFFSET_LED_B
#define BSP_PLATFORM_IO_BIT_MASK_LED_1   BSP_GPIO_BIT_MASK_LED_B

#define BSP_PLATFORM_IO_PORT_ID_LED_2    BSP_GPIO_PORT_ID_LED_G
#define BSP_PLATFORM_IO_BASE_ADDR_LED_2  BSP_GPIO_BASE_ADDR_LED_G
#define BSP_PLATFORM_IO_BIT_OFFSET_LED_2 BSP_GPIO_BIT_OFFSET_LED_G
#define BSP_PLATFORM_IO_BIT_MASK_LED_2   BSP_GPIO_BIT_MASK_LED_G


/*==============================================================================
 *                            STDIO Mapping
 *============================================================================*/
// Choose "uart0" or "usb0"
#define BSP_PLATFORM_IO_MAP_STDOUT "uart0"
#define BSP_PLATFORM_IO_MAP_STDIN  "uart0"
#define BSP_PLATFORM_IO_MAP_STDERR "uart0"
