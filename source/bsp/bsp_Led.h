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
 * @file bsp_Led.h
 * @brief Contains types and prototypes to access the LEDs
 */
#pragma once

#include "bsp_Platform.h"
#include "bsp_Gpio.h"
#include BUILD_INCLUDE_STRING( bsp_Led_, PLATFORM )

/*==============================================================================
 *                             Defines
 *============================================================================*/
/*==============================================================================
 *                             Macros
 *============================================================================*/
/*==============================================================================
 *                               Types
 *============================================================================*/
/*============================================================================*/
#define BSP_LED_COLOR_MASK_R 0x00FF0000
#define BSP_LED_COLOR_MASK_G 0x0000FF00
#define BSP_LED_COLOR_MASK_B 0x000000FF
typedef uint32_t bsp_Led_Color_t;

/*============================================================================*/
typedef uint32_t bsp_Led_Id_t;

#define BSP_LED_POLARITY_ACTIVE_LOW  0
#define BSP_LED_POLARITY_ACTIVE_HIGH 1
typedef uint8_t bsp_Led_Polarity_t;

/*============================================================================*/
typedef struct {
    const char*        name;
    bsp_Led_Color_t    color;
    bsp_Gpio_PortId_t  portId;
    uint32_t           portAddr;
    bsp_Gpio_BitMask_t mask;
    bsp_Gpio_Drive_t   drive;
    bsp_Led_Polarity_t polarity;
} bsp_Led_GroupElementInfo_t;

/*============================================================================*/
typedef struct {
    const char*                       name;
    const bsp_Led_GroupElementInfo_t* groupTable;
    size_t                            cnt;
} bsp_Led_Info_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Led_init( void );

/*============================================================================*/
void
bsp_Led_setColor( bsp_Led_Id_t    id,
                  bsp_Led_Color_t color );
