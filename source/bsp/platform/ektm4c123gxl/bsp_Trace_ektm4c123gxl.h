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
 * @file bsp_Trace_ektm4c123gxl.h
 * @brief Contains platform specific trace mappings to IO pins
 */
#pragma once

#include "bsp_Gpio.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define BSP_TRACE_ENABLE
//#define BSP_TRACE_INT
//#define BSP_TRACE_UARTS
//#define BSP_TRACE_USBIO
//#define BSP_TRACE_I2C

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_INT )
#define BSP_TRACE_INT_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA2 );}
#define BSP_TRACE_INT_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA2 );}
#define BSP_TRACE_INT_LOCK_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_INT_LOCK_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#else
#define BSP_TRACE_INT_ENTER()
#define BSP_TRACE_INT_EXIT()
#define BSP_TRACE_INT_LOCK_ENTER()
#define BSP_TRACE_INT_LOCK_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_UART )
#define BSP_TRACE_UART_INT_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_UART_INT_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_UART_SND_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_UART_SND_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_UART_RCV_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_UART_RCV_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#else
#define BSP_TRACE_UART_INT_ENTER()
#define BSP_TRACE_UART_INT_EXIT()
#define BSP_TRACE_UART_SND_ENTER()
#define BSP_TRACE_UART_SND_EXIT()
#define BSP_TRACE_UART_RCV_ENTER()
#define BSP_TRACE_UART_RCV_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_USBIO )
#define BSP_TRACE_USBIO_INT_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_USBIO_INT_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_USBIO_SND_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_USBIO_SND_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_USBIO_RCV_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_USBIO_RCV_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#define BSP_TRACE_USBIO_TX_COMPLETE_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA6 );}
#define BSP_TRACE_USBIO_TX_COMPLETE_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA6 );}
#define BSP_TRACE_USBIO_RX_AVAIL_ENTER()    {BSP_GPIO_OUT_SET_HIGH( TPA7 );}
#define BSP_TRACE_USBIO_RX_AVAIL_EXIT()     {BSP_GPIO_OUT_SET_LOW( TPA7 );}
#else
#define BSP_TRACE_USBIO_INT_ENTER()
#define BSP_TRACE_USBIO_INT_EXIT()
#define BSP_TRACE_USBIO_SND_ENTER()
#define BSP_TRACE_USBIO_SND_EXIT()
#define BSP_TRACE_USBIO_RCV_ENTER()
#define BSP_TRACE_USBIO_RCV_EXIT()
#define BSP_TRACE_USBIO_TX_COMPLETE_ENTER()
#define BSP_TRACE_USBIO_TX_COMPLETE_EXIT()
#define BSP_TRACE_USBIO_RX_AVAIL_ENTER()
#define BSP_TRACE_USBIO_RX_AVAIL_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_I2C )
#define BSP_TRACE_I2C_ISR_MASTER_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_I2C_ISR_MASTER_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#define BSP_TRACE_I2C_STATUS_IDLE()           {BSP_GPIO_OUT_SET_HIGH( TPA6 ); BSP_GPIO_OUT_SET_LOW( TPA6 );}
#define BSP_TRACE_I2C_STATUS_BUS_BUSY()       {BSP_GPIO_OUT_SET_HIGH( TPA7 ); BSP_GPIO_OUT_SET_LOW( TPA7 );}
#define BSP_TRACE_I2C_STATUS_OTHER()          {BSP_GPIO_OUT_SET_HIGH( TPA4 ); BSP_GPIO_OUT_SET_LOW( TPA4 );}
#else
#define BSP_TRACE_I2C_ISR_MASTER_ENTER()
#define BSP_TRACE_I2C_ISR_MASTER_EXIT()
#define BSP_TRACE_I2C_STATUS_IDLE()
#define BSP_TRACE_I2C_STATUS_BUS_BUSY()
#define BSP_TRACE_I2C_STATUS_OTHER()
#endif
/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
extern const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS];
