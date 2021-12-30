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
 * @file dev_PwrMon_ina.h
 * @brief Contains Macros, defines, and prototypes for the Power Monitor ADCs.
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_I2c.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
typedef uint16_t dev_PwrMon_DeviceId_t;
typedef uint16_t dev_PwrMon_ManufacturerId_t;
typedef uint16_t dev_PwrMon_WriteData_t;

/*===========================================================================*/
#define DEV_PWR_MON_DEVICE_TYPE_INA226 0
#define DEV_PWR_MON_DEVICE_TYPE_INA228 1
typedef uint8_t dev_PwrMon_DeviceType_t;

typedef struct dev_PwrMon_DeviceCtx_s
{
    bsp_I2c_MasterTrans_t i2cTrans;
    uint8_t               wLen;
    uint8_t               wBuffer[6];
    uint8_t               rLen;
    void*                 rPtr;
    dev_PwrMon_Callback_t callback;
    void*                 cbData;

    volatile bool               active;
    uint8_t                     prevRegId;
    dev_PwrMon_DeviceId_t       deviceId;
    dev_PwrMon_ManufacturerId_t mftrId;
} dev_PwrMon_DeviceCtx_t;

typedef struct dev_PwrMon_DeviceInfo_s
{
    bsp_I2c_Id_t            i2cId;
    bsp_I2c_Addr_t          i2cAddr;
    bsp_I2c_Speed_t         i2cSpeed;
    dev_PwrMon_ChannelId_t  channelId;
    dev_PwrMon_DeviceType_t devType;
    dev_PwrMon_DeviceCtx_t* ctx;
} dev_PwrMon_DeviceInfo_t;

typedef uint8_t dev_PwrMon_DevId_t;

#define DEV_PWR_MON_DEVICE_MODE_MASK_SHUNT 0x01
#define DEV_PWR_MON_DEVICE_MODE_MASK_BUS   0x02
typedef uint8_t dev_PwrMon_DevModeMask_t;

// Choose an invalid address for any of the devices
#define DEV_PWRMON_REG_INVALID 0xF0

/*===========================================================================*/
/* Function prototype types for the API (object) structure */
typedef void (*dev_PwrMon_InitFunc_t)( const dev_PwrMon_DeviceInfo_t* devPtr );
typedef void (*dev_PwrMon_ConfigFunc_t)( const dev_PwrMon_DeviceInfo_t* devPtr,
                                         dev_PwrMon_DevModeMask_t modeMask,
                                         dev_PwrMon_Callback_t    callback,
                                         void*                    cbData );
typedef void (*dev_PwrMon_ReadFunc_t)( const dev_PwrMon_DeviceInfo_t* devPtr,
                                       uint8_t*                 dataPtr,
                                       dev_PwrMon_Callback_t    callback,
                                       void*                    cbData );
typedef int32_t (*dev_PwrMon_CalFunc_t)( const dev_PwrMon_DeviceInfo_t* devPtr );
typedef int32_t (*dev_PwrMon_ConvertFunc_t)( int32_t val );
typedef int32_t (*dev_PwrMon_FormatFunc_t)( int32_t val );

typedef struct
{
    dev_PwrMon_ReadFunc_t    read;
    dev_PwrMon_CalFunc_t     cal;
    dev_PwrMon_FormatFunc_t  format;
    dev_PwrMon_ConvertFunc_t convert;
} dev_PwrMon_DeviceApiVoltage_t;


typedef struct
{
    dev_PwrMon_InitFunc_t         init;
    dev_PwrMon_ConfigFunc_t       config;
    dev_PwrMon_DeviceApiVoltage_t vBus;
    dev_PwrMon_DeviceApiVoltage_t vShunt;
} dev_PwrMon_DeviceApi_t;


/*===========================================================================*/
typedef uint8_t dev_PwrMon_I2cCmd_t;
typedef struct dev_PwrMon_RegInfo_s
{
    dev_PwrMon_I2cCmd_t cmd;
    uint8_t             len;
    bool                rdOnly;
} dev_PwrMon_RegInfo_t;

#define DEV_PWRMON_REG_TYPE_RO true
#define DEV_PWRMON_REG_TYPE_RW false
typedef uint8_t dev_PwrMon_RegType_t;


/*===========================================================================*/
void
dev_PwrMon_commonRead( const dev_PwrMon_DeviceInfo_t* devPtr,
                       uint8_t                        regId,
                       uint8_t                        regLen,
                       void*                          dataPtr,
                       dev_PwrMon_Callback_t          callback,
                       void*                          cbData );

/*===========================================================================*/
void
dev_PwrMon_commonWrite( const dev_PwrMon_DeviceInfo_t* devPtr,
                        uint8_t                        regId,
                        dev_PwrMon_WriteData_t         data,
                        dev_PwrMon_Callback_t          callback,
                        void*                          cbData );
