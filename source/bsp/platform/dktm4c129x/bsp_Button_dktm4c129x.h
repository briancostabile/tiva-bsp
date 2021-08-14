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
 * @file bsp_Button_dktm4c129x.h
 * @brief Contains platform specific trace mappings to IO pins
 */
#pragma once

#include "bsp_Gpio.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/

#define BSP_PLATFORM_IO_BUTTON_NUM   3
#define BSP_GPIO_PORT_ID_BUTTON_0    BSP_GPIO_PORT_ID_USR_SW2
#define BSP_GPIO_BASE_ADDR_BUTTON_0  BSP_GPIO_BASE_ADDR_USR_SW2
#define BSP_GPIO_BIT_OFFSET_BUTTON_0 BSP_GPIO_BIT_OFFSET_USR_SW2
#define BSP_GPIO_BIT_MASK_BUTTON_0   BSP_GPIO_BIT_MASK_USR_SW2

#define BSP_GPIO_PORT_ID_BUTTON_1    BSP_GPIO_PORT_ID_USR_SW3
#define BSP_GPIO_BASE_ADDR_BUTTON_1  BSP_GPIO_BASE_ADDR_USR_SW3
#define BSP_GPIO_BIT_OFFSET_BUTTON_1 BSP_GPIO_BIT_OFFSET_USR_SW3
#define BSP_GPIO_BIT_MASK_BUTTON_1   BSP_GPIO_BIT_MASK_USR_SW3

#define BSP_GPIO_PORT_ID_BUTTON_2    BSP_GPIO_PORT_ID_USR_SW4
#define BSP_GPIO_BASE_ADDR_BUTTON_2  BSP_GPIO_BASE_ADDR_USR_SW4
#define BSP_GPIO_BIT_OFFSET_BUTTON_2 BSP_GPIO_BIT_OFFSET_USR_SW4
#define BSP_GPIO_BIT_MASK_BUTTON_2   BSP_GPIO_BIT_MASK_USR_SW4



/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
extern const bsp_Button_IoInfo_t bsp_Button_ioInfoTable[BSP_PLATFORM_IO_BUTTON_NUM];
