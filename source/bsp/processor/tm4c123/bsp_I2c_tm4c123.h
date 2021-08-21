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
 * @file bsp_I2c_tm4c123.h
 * @brief Contains the tm4c123 core I2C defines.
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Interrupt.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                                 Types
 *============================================================================*/

/*============================================================================*/
#define BSP_I2C_ID0 ((bsp_I2c_Id_t)0)
#define BSP_I2C_ID1 ((bsp_I2c_Id_t)1)
#define BSP_I2C_ID2 ((bsp_I2c_Id_t)2)
#define BSP_I2C_ID3 ((bsp_I2c_Id_t)3)
typedef uint8_t bsp_I2c_Id_t;

/*============================================================================*/
#define BSP_I2C_PLATFORM_NUM 4


/*============================================================================*/
#define BSP_I2C_PIN_SEL_0    ((bsp_I2c_PinSel_t)0)
#define BSP_I2C_PIN_SEL_1    ((bsp_I2c_PinSel_t)1)
#define BSP_I2C_PIN_SEL_2    ((bsp_I2c_PinSel_t)2)
#define BSP_I2C_PIN_SEL_3    ((bsp_I2c_PinSel_t)3)
#define BSP_I2C_PIN_SEL_4    ((bsp_I2c_PinSel_t)4)
#define BSP_I2C_PIN_SEL_5    ((bsp_I2c_PinSel_t)5)
#define BSP_I2C_PIN_SEL_6    ((bsp_I2c_PinSel_t)6)
#define BSP_I2C_PIN_SEL_7    ((bsp_I2c_PinSel_t)7)
#define BSP_I2C_PIN_SEL_NONE ((bsp_I2c_PinSel_t)8)
typedef uint8_t bsp_I2c_PinSel_t;

/*============================================================================*/
/**
 * @brief Structure to hold Pin muxing info for I2C pins
 */
typedef struct {
    bsp_Gpio_PortId_t    portId;
    bsp_Gpio_BitMask_t   mask;
    bsp_Gpio_AltFuncId_t altFuncId;
}bsp_I2c_PinInfo_t;

/*============================================================================*/
/**
 * @brief Structure to hold constant info about the I2C hardware
 */
typedef struct {
    uint32_t                 baseAddr;
    uint32_t                 sysCtrlAddr;
    uint32_t                 intId;
    uint8_t                  sclPinInfoTableLen;
    const bsp_I2c_PinInfo_t* sclPinInfoTable;
    uint8_t                  sdaPinInfoTableLen;
    const bsp_I2c_PinInfo_t* sdaPinInfoTable;
}bsp_I2c_StaticInfo_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_I2c_interruptHandler0( void );
void bsp_I2c_interruptHandler1( void );
void bsp_I2c_interruptHandler2( void );
void bsp_I2c_interruptHandler3( void );

/*==============================================================================
 *                                Globals
 *============================================================================*/

/*============================================================================*/
extern const bsp_I2c_StaticInfo_t bsp_I2c_staticInfo[ BSP_I2C_PLATFORM_NUM ];

