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
 * @file bsp_Platform_ektm4c123sensor.h
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

//#define BSP_PLATFORM_ENABLE_DEV_HUMID_SHT21
//#define BSP_PLATFORM_ENABLE_DEV_LIGHT_ISL20023
//#define BSP_PLATFORM_ENABLE_DEV_PRESSURE_BMP180
//#define BSP_PLATFORM_ENABLE_DEV_TEMP_TMP006
#define BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226

// List the I2C devices for this platform as an array of structures
// { <index-into-bsp_I2c_staticInfo>, <index-into-bsp_I2c_pinInfoTableSclX>, <index-into-bsp_I2c_pinInfoTableSdaX> }
// I2C3:
//    - SensorHub MPU9150
//    - SensorHub BMP180
//    - SensorHub SHT21
//    - SensorHub ISL290023
//    - SensorHub TMP006
// I2C1:
//    - INA226 0
//    - INA226 1
#define BSP_PLATFORM_I2C_LIST  { \
    { 3, 0, 0 }, \
    { 1, 0, 0 }  \
}

// Each I2C device must identify the I2C hardware block it is connected to
//#define BSP_PLATFORM_I2C_SHT21    3
//#define BSP_PLATFORM_I2C_TMP006   3
//#define BSP_PLATFORM_I2C_ISL29023 3
#define BSP_PLATFORM_I2C_INA226   1

// Humidity/Temperature sensor accuracy selection
//      Humid  Temp
// 0x00 12 bit 14 bit
// 0x01  8 bit 12 bit
// 0x02 10 bit 13 bit
// 0x03 11 bit 11 bit
#define BSP_PLATFORM_SHT21_RESOLUTION 0x03

/*==============================================================================
 *                            STDIO Mapping
 *============================================================================*/
#if (PLATFORM_IO == uart)
#define BSP_PLATFORM_IO_MAP_STDOUT "uart0"
#define BSP_PLATFORM_IO_MAP_STDIN  "uart0"
#define BSP_PLATFORM_IO_MAP_STDERR "uart0"
#elif (PLATFORM_IO == usb)
#define BSP_PLATFORM_IO_MAP_STDOUT "usb0"
#define BSP_PLATFORM_IO_MAP_STDIN  "usb0"
#define BSP_PLATFORM_IO_MAP_STDERR "usb0"
#endif
