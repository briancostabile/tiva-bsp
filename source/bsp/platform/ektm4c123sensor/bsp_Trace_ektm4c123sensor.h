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
 * @file bsp_Trace_ektm4c123sensor.h
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
//#define BSP_TRACE_TIMER
//#define BSP_TRACE_PWRMON_SAMPLER
#define BSP_TRACE_PWRMONEH

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
#define BSP_TRACE_I2C_ISR_MASTER_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA2 );}
#define BSP_TRACE_I2C_ISR_MASTER_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA2 );}
#define BSP_TRACE_I2C_TRANS_QUEUE_ENTER()     {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_I2C_TRANS_QUEUE_EXIT()      {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_I2C_TRANS_START_ENTER()     {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_I2C_TRANS_START_EXIT()      {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_I2C_STATUS_OTHER()          {BSP_GPIO_OUT_SET_HIGH( TPA5 ); BSP_GPIO_OUT_SET_LOW( TPA5 );}
#else
#define BSP_TRACE_I2C_ISR_MASTER_ENTER()
#define BSP_TRACE_I2C_ISR_MASTER_EXIT()
#define BSP_TRACE_I2C_TRANS_QUEUE_ENTER()
#define BSP_TRACE_I2C_TRANS_QUEUE_EXIT()
#define BSP_TRACE_I2C_TRANS_START_ENTER()
#define BSP_TRACE_I2C_TRANS_START_EXIT()
#define BSP_TRACE_I2C_STATUS_OTHER()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_TIMER )
#define BSP_TRACE_TIMER_START_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_TIMER_START_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#define BSP_TRACE_TIMER_STOP_ENTER()  {BSP_GPIO_OUT_SET_HIGH( TPA6 );}
#define BSP_TRACE_TIMER_STOP_EXIT()   {BSP_GPIO_OUT_SET_LOW( TPA6 );}
#define BSP_TRACE_TIMER_INT_ENTER()   {BSP_GPIO_OUT_SET_HIGH( TPA7 );}
#define BSP_TRACE_TIMER_INT_EXIT()    {BSP_GPIO_OUT_SET_LOW( TPA7 );}
#else
#define BSP_TRACE_TIMER_START()
#define BSP_TRACE_TIMER_START_ENTER()
#define BSP_TRACE_TIMER_START_EXIT()
#define BSP_TRACE_TIMER_STOP_ENTER()
#define BSP_TRACE_TIMER_STOP_EXIT()
#define BSP_TRACE_TIMER_INT_ENTER()
#define BSP_TRACE_TIMER_INT_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_PWRMON_SAMPLER )
#define BSP_TRACE_PWRMON_SAMPLER_SET_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA2 );}
#define BSP_TRACE_PWRMON_SAMPLER_SET_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA2 );}
#define BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_ENTER()  {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT()   {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER()  {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_PWRMON_SAMPLER_CURRENT_EXIT()   {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_PWRMON_SAMPLER_SET_DONE_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#define BSP_TRACE_PWRMON_SAMPLER_PKT_DONE()       {BSP_GPIO_OUT_SET_HIGH( TPB4 );BSP_GPIO_OUT_SET_LOW( TPB4 );}
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_ENTER()  {BSP_GPIO_OUT_SET_HIGH( TPB5 );}
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_EXIT()   {BSP_GPIO_OUT_SET_LOW( TPB5 );}
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_DONE()   {BSP_GPIO_OUT_SET_HIGH( TPB5 );BSP_GPIO_OUT_SET_LOW( TPB5 ); \
                                                   BSP_GPIO_OUT_SET_HIGH( TPB5 );BSP_GPIO_OUT_SET_LOW( TPB5 );}
#else
#define BSP_TRACE_PWRMON_SAMPLER_SET_ENTER()
#define BSP_TRACE_PWRMON_SAMPLER_SET_EXIT()
#define BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_ENTER()
#define BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT()
#define BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER()
#define BSP_TRACE_PWRMON_SAMPLER_CURRENT_EXIT()
#define BSP_TRACE_PWRMON_SAMPLER_SET_DONE_ENTER()
#define BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT()
#define BSP_TRACE_PWRMON_SAMPLER_PKT_DONE()
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_ENTER()
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_EXIT()
#define BSP_TRACE_PWRMON_SAMPLER_PKT_SND_DONE()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_PWRMONEH )
#define BSP_TRACE_PWRMONEH_DATA_IND_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA2 );}
#define BSP_TRACE_PWRMONEH_DATA_IND_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA2 );}
#else
#define BSP_TRACE_PWRMONEH_DATA_IND_ENTER()
#define BSP_TRACE_PWRMONEH_DATA_IND_EXIT()
#endif

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
extern const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS];

