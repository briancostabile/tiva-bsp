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
 * @file bsp_Button.h
 * @brief Contains a thin wrapper API around GPIOs for the switch-style buttons
 */
#pragma once

#include "bsp_Gpio.h"

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
#define BSP_BUTTON_PRESS_POLARITY_HIGH 0
#define BSP_BUTTON_PRESS_POLARITY_LOW  1
typedef uint8_t bsp_Button_PressPolarity_t;

/*============================================================================*/
typedef struct {
    bsp_Gpio_PortId_t          portId;
    bsp_Gpio_BitMask_t         mask;
    bsp_Button_PressPolarity_t pressPolarity;
}bsp_Button_IoInfo_t;


/* Include after defining IoInfo structure because platform depends on that structure */
#include BUILD_INCLUDE_STRING( bsp_Button_, PLATFORM )

/*============================================================================*/
typedef uint8_t bsp_Button_Id_t;

/*============================================================================*/
#define BSP_BUTTON_CONTROL_DISABLE 0
#define BSP_BUTTON_CONTROL_ENABLE  1
typedef uint8_t bsp_Button_Control_t;

/*============================================================================*/
#define BSP_BUTTON_STATE_NOT_PRESSED 0
#define BSP_BUTTON_STATE_PRESSED     1
typedef uint8_t bsp_Button_State_t;

/*============================================================================*/
typedef void (* bsp_Button_PressHandler_t)( bsp_Button_Id_t id );


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Button_init( void );

/*============================================================================*/
void
bsp_Button_control( bsp_Button_Id_t      id,
                    bsp_Button_Control_t control );

/*============================================================================*/
void
bsp_Button_registerHandler( bsp_Button_Id_t           id,
                            bsp_Button_PressHandler_t handler );

/*============================================================================*/
bsp_Button_State_t
bsp_Button_state( bsp_Button_Id_t id );
