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
 * @file bsp_Uart_tm4c129.h
 * @brief Contains the tm4c129 core UART defines.
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Interrupt.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
#define BSP_UART_ID0 ((bsp_Uart_Id_t)0)
#define BSP_UART_ID1 ((bsp_Uart_Id_t)1)
#define BSP_UART_ID2 ((bsp_Uart_Id_t)2)
#define BSP_UART_ID3 ((bsp_Uart_Id_t)3)
#define BSP_UART_ID4 ((bsp_Uart_Id_t)4)
#define BSP_UART_ID5 ((bsp_Uart_Id_t)5)
#define BSP_UART_ID6 ((bsp_Uart_Id_t)6)
#define BSP_UART_ID7 ((bsp_Uart_Id_t)7)
typedef uint8_t bsp_Uart_Id_t;

/*============================================================================*/
#define BSP_UART_PLATFORM_NUM_UARTS 8

/*============================================================================*/
#define BSP_UART_PIN_SEL_0    ((bsp_Uart_PinSel_t)0)
#define BSP_UART_PIN_SEL_1    ((bsp_Uart_PinSel_t)1)
#define BSP_UART_PIN_SEL_2    ((bsp_Uart_PinSel_t)2)
#define BSP_UART_PIN_SEL_3    ((bsp_Uart_PinSel_t)3)
#define BSP_UART_PIN_SEL_4    ((bsp_Uart_PinSel_t)4)
#define BSP_UART_PIN_SEL_5    ((bsp_Uart_PinSel_t)5)
#define BSP_UART_PIN_SEL_6    ((bsp_Uart_PinSel_t)6)
#define BSP_UART_PIN_SEL_7    ((bsp_Uart_PinSel_t)7)
#define BSP_UART_PIN_SEL_NONE ((bsp_Uart_PinSel_t)8)
typedef uint8_t bsp_Uart_PinSel_t;

/*============================================================================*/
/**
 * @brief Structure to hold constant info about the UART hardware
 */
typedef struct {
    bsp_Gpio_PortId_t    portId;
    bsp_Gpio_BitMask_t   mask;
    bsp_Gpio_AltFuncId_t altFuncId;
}bsp_Uart_PinInfo_t;

/*============================================================================*/
/**
 * @brief Structure to hold constant info about the UART hardware
 */
typedef struct {
    uint32_t                  baseAddr;
    uint32_t                  sysCtrlAddr;
    uint32_t                  intId;
    uint8_t                   rxPinInfoTableLen;
    const bsp_Uart_PinInfo_t* rxPinInfoTable;
    uint8_t                   txPinInfoTableLen;
    const bsp_Uart_PinInfo_t* txPinInfoTable;
    uint8_t                   rtsPinInfoTableLen;
    const bsp_Uart_PinInfo_t* rtsPinInfoTable;
    uint8_t                   ctsPinInfoTableLen;
    const bsp_Uart_PinInfo_t* ctsPinInfoTable;
    uint8_t                   riPinInfoTableLen;
    const bsp_Uart_PinInfo_t* riPinInfoTable;
    uint8_t                   dcdPinInfoTableLen;
    const bsp_Uart_PinInfo_t* dcdPinInfoTable;
    uint8_t                   dsrPinInfoTableLen;
    const bsp_Uart_PinInfo_t* dsrPinInfoTable;
    uint8_t                   dtrPinInfoTableLen;
    const bsp_Uart_PinInfo_t* dtrPinInfoTable;
}bsp_Uart_StaticInfo_t;


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Uart_interruptHandler0( void );
void bsp_Uart_interruptHandler1( void );
void bsp_Uart_interruptHandler2( void );
void bsp_Uart_interruptHandler3( void );
void bsp_Uart_interruptHandler4( void );
void bsp_Uart_interruptHandler5( void );
void bsp_Uart_interruptHandler6( void );
void bsp_Uart_interruptHandler7( void );

/*==============================================================================
 *                                Globals
 *============================================================================*/

/*============================================================================*/
extern const bsp_Uart_StaticInfo_t bsp_Uart_staticInfoTable[ BSP_UART_PLATFORM_NUM_UARTS ];
