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
 * @file bsp_Led_dktm4c129x.h
 * @brief Contains platform specific LED mappings to IO pins
 */
#pragma once

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define BSP_PLATFORM_LED_ID_STATUS 0
#define BSP_PLATFORM_LED_NUM 1

#define BSP_PLATFORM_NAME_LED_0       "status-red"
#define BSP_PLATFORM_COLOR_LED_0      BSP_LED_COLOR_MASK_R
#define BSP_PLATFORM_PORT_ID_LED_0    BSP_GPIO_PORT_ID_LED_R
#define BSP_PLATFORM_BASE_ADDR_LED_0  BSP_GPIO_BASE_ADDR_LED_R
#define BSP_PLATFORM_BIT_MASK_LED_0   BSP_GPIO_BIT_MASK_LED_R
#define BSP_PLATFORM_DRIVE_LED_0      BSP_GPIO_DRIVE_2MA

#define BSP_PLATFORM_NAME_LED_1       "status-blue"
#define BSP_PLATFORM_COLOR_LED_1      BSP_LED_COLOR_MASK_B
#define BSP_PLATFORM_PORT_ID_LED_1    BSP_GPIO_PORT_ID_LED_B
#define BSP_PLATFORM_BASE_ADDR_LED_1  BSP_GPIO_BASE_ADDR_LED_B
#define BSP_PLATFORM_BIT_MASK_LED_1   BSP_GPIO_BIT_MASK_LED_B
#define BSP_PLATFORM_DRIVE_LED_1      BSP_GPIO_DRIVE_2MA

#define BSP_PLATFORM_NAME_LED_2       "status-green"
#define BSP_PLATFORM_COLOR_LED_2      BSP_LED_COLOR_MASK_G
#define BSP_PLATFORM_PORT_ID_LED_2    BSP_GPIO_PORT_ID_LED_G
#define BSP_PLATFORM_BASE_ADDR_LED_2  BSP_GPIO_BASE_ADDR_LED_G
#define BSP_PLATFORM_BIT_MASK_LED_2   BSP_GPIO_BIT_MASK_LED_G
#define BSP_PLATFORM_DRIVE_LED_2      BSP_GPIO_DRIVE_2MA

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
