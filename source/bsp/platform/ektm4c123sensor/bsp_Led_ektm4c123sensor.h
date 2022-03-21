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
 * @file bsp_Led_ektm4c123sensor.h
 * @brief Contains platform specific LED mappings to IO pins
 */
#pragma once

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define BSP_PLATFORM_LED_ID_CH0    0
#define BSP_PLATFORM_LED_ID_CH1    1
#define BSP_PLATFORM_LED_ID_STATUS 2
#define BSP_PLATFORM_LED_ID_SENSOR 3
#define BSP_PLATFORM_LED_NUM       4

#define BSP_PLATFORM_LED_NAME_CH0      ""
#define BSP_PLATFORM_LED_COLOR_CH0     0
#define BSP_PLATFORM_LED_PORT_ID_CH0   0
#define BSP_PLATFORM_LED_BASE_ADDR_CH0 0
#define BSP_PLATFORM_LED_BIT_MASK_CH0  0
#define BSP_PLATFORM_LED_DRIVE_CH0     0
#define BSP_PLATFORM_LED_POLARITY_CH0  0

#define BSP_PLATFORM_LED_NAME_CH1      ""
#define BSP_PLATFORM_LED_COLOR_CH1     0
#define BSP_PLATFORM_LED_PORT_ID_CH1   0
#define BSP_PLATFORM_LED_BASE_ADDR_CH1 0
#define BSP_PLATFORM_LED_BIT_MASK_CH1  0
#define BSP_PLATFORM_LED_DRIVE_CH1     0
#define BSP_PLATFORM_LED_POLARITY_CH1  0

#define BSP_PLATFORM_LED_NAME_LED_0      "status-red"
#define BSP_PLATFORM_LED_COLOR_LED_0     BSP_LED_COLOR_MASK_R
#define BSP_PLATFORM_LED_PORT_ID_LED_0   BSP_GPIO_PORT_ID_LED_R
#define BSP_PLATFORM_LED_BASE_ADDR_LED_0 BSP_GPIO_BASE_ADDR_LED_R
#define BSP_PLATFORM_LED_BIT_MASK_LED_0  BSP_GPIO_BIT_MASK_LED_R
#define BSP_PLATFORM_LED_DRIVE_LED_0     BSP_GPIO_DRIVE_2MA
#define BSP_PLATFORM_LED_POLARITY_LED_0  1

#define BSP_PLATFORM_LED_NAME_LED_1      "status-blue"
#define BSP_PLATFORM_LED_COLOR_LED_1     BSP_LED_COLOR_MASK_B
#define BSP_PLATFORM_LED_PORT_ID_LED_1   BSP_GPIO_PORT_ID_LED_B
#define BSP_PLATFORM_LED_BASE_ADDR_LED_1 BSP_GPIO_BASE_ADDR_LED_B
#define BSP_PLATFORM_LED_BIT_MASK_LED_1  BSP_GPIO_BIT_MASK_LED_B
#define BSP_PLATFORM_LED_DRIVE_LED_1     BSP_GPIO_DRIVE_2MA
#define BSP_PLATFORM_LED_POLARITY_LED_1  1

#define BSP_PLATFORM_LED_NAME_LED_2      "status-green"
#define BSP_PLATFORM_LED_COLOR_LED_2     BSP_LED_COLOR_MASK_G
#define BSP_PLATFORM_LED_PORT_ID_LED_2   BSP_GPIO_PORT_ID_LED_G
#define BSP_PLATFORM_LED_BASE_ADDR_LED_2 BSP_GPIO_BASE_ADDR_LED_G
#define BSP_PLATFORM_LED_BIT_MASK_LED_2  BSP_GPIO_BIT_MASK_LED_G
#define BSP_PLATFORM_LED_DRIVE_LED_2     BSP_GPIO_DRIVE_2MA
#define BSP_PLATFORM_LED_POLARITY_LED_2  1

#define BSP_PLATFORM_LED_NAME_LED_3      "sensor"
#define BSP_PLATFORM_LED_COLOR_LED_3     BSP_LED_COLOR_MASK_R
#define BSP_PLATFORM_LED_PORT_ID_LED_3   BSP_GPIO_PORT_ID_SENSOR_LED
#define BSP_PLATFORM_LED_BASE_ADDR_LED_3 BSP_GPIO_BASE_ADDR_SENSOR_LED
#define BSP_PLATFORM_LED_BIT_MASK_LED_3  BSP_GPIO_BIT_MASK_SENSOR_LED
#define BSP_PLATFORM_LED_DRIVE_LED_3     BSP_GPIO_DRIVE_2MA
#define BSP_PLATFORM_LED_POLARITY_LED_3  1

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
