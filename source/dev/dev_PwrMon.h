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
 * @file dev_PwrMon.h
 * @brief Contains Macros, defines, and prototypes for the Power Monitor ADCs.
 */
#pragma once

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
typedef uint16_t dev_PwrMon_DeviceId_t;
typedef uint16_t dev_PwrMon_ManufacturerId_t;
typedef uint16_t dev_PwrMon_Config_t;
typedef uint16_t dev_PwrMon_Cal_t;
typedef int16_t  dev_PwrMon_Data_t;
typedef uint16_t dev_PwrMon_AlertMask_t;
typedef uint16_t dev_PwrMon_ShuntVal_t;


enum
{
    BSP_PWRMOMN_AVG_MODE_SAMPLES_1    = 0x00,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_4    = 0x01,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_16   = 0x02,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_64   = 0x03,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_128  = 0x04,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_256  = 0x05,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_512  = 0x06,
    BSP_PWRMOMN_AVG_MODE_SAMPLES_1024 = 0x07
};
typedef uint8_t dev_PwrMon_AvgMode_t;

enum
{
    BSP_PWRMOMN_CONV_TIME_US_140   = 0x00,
    BSP_PWRMOMN_CONV_TIME_US_204   = 0x01,
    BSP_PWRMOMN_CONV_TIME_US_332   = 0x02,
    BSP_PWRMOMN_CONV_TIME_US_558   = 0x03,
    BSP_PWRMOMN_CONV_TIME_US_1100  = 0x04,
    BSP_PWRMOMN_CONV_TIME_US_2116  = 0x05,
    BSP_PWRMOMN_CONV_TIME_US_4156  = 0x06,
    BSP_PWRMOMN_CONV_TIME_US_8244  = 0x07
};
typedef uint8_t dev_PwrMon_ConvTime_t;

enum
{
    BSP_PWRMOMN_OP_MODE_PWR_DWN            = 0x00,
    BSP_PWRMOMN_OP_MODE_TRIG_SHUNT         = 0x01,
    BSP_PWRMOMN_OP_MODE_TRIG_BUS           = 0x02,
    BSP_PWRMOMN_OP_MODE_TRIG_SHUNT_AND_BUS = 0x03,
    BSP_PWRMOMN_OP_MODE_CONT_SHUNT         = 0x05,
    BSP_PWRMOMN_OP_MODE_CONT_BUS           = 0x06,
    BSP_PWRMOMN_OP_MODE_CONT_SHUNT_AND_BUS = 0x07,
};
typedef uint8_t dev_PwrMon_OpMode_t;


typedef uint8_t dev_PwrMon_ChannelId_t;
typedef uint8_t dev_PwrMon_DevId_t;
typedef void (*dev_PwrMon_Callback_t)( void* cbData );

/*==============================================================================
 *                               Prototypes
 *============================================================================*/
/*===========================================================================*/
void
dev_PwrMon_init( void );

/*===========================================================================*/
void
dev_PwrMon_channelConfig( dev_PwrMon_ChannelId_t channelId,
                          dev_PwrMon_ConvTime_t  shuntConvTime,
                          dev_PwrMon_ConvTime_t  busConvTime,
                          dev_PwrMon_AvgMode_t   avgMode,
                          dev_PwrMon_Callback_t  callback,
                          void*                  cbData );

/*===========================================================================*/
void
dev_PwrMon_channelConfigShunt( dev_PwrMon_ChannelId_t channelId,
                               dev_PwrMon_ShuntVal_t  shunt,
                               dev_PwrMon_Callback_t  callback,
                               void*                  cbData );

/*===========================================================================*/
void
dev_PwrMon_channelCurrentRead( dev_PwrMon_ChannelId_t channelId,
                               dev_PwrMon_Data_t*     dataPtr,
                               dev_PwrMon_Callback_t  callback,
                               void*                  cbData );

/*===========================================================================*/
void
dev_PwrMon_channelBusVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                  dev_PwrMon_Data_t*     dataPtr,
                                  dev_PwrMon_Callback_t  callback,
                                  void*                  cbData );

/*===========================================================================*/
void
dev_PwrMon_channelShuntVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                    dev_PwrMon_Data_t*     dataPtr,
                                    dev_PwrMon_Callback_t  callback,
                                    void*                  cbData );

/*===========================================================================*/
void
dev_PwrMon_channelPowerRead( dev_PwrMon_ChannelId_t channelId,
                             dev_PwrMon_Data_t*     dataPtr,
                             dev_PwrMon_Callback_t  callback,
                             void*                  cbData );
