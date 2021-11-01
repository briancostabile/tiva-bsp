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
/*===========================================================================*/
/**
 * @file dev_PwrMon_ina226.c
 * @brief Contains Driver for the INA228 Power Monitor.
 */
#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Gpio.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_PwrMon.h"


#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/

// Configuration defines
#define DEV_PWRMON_REG_CONFIG       ((dev_PwrMon_I2cCmd_t)0x00)
#define DEV_PWRMON_REG_VSHUNT       ((dev_PwrMon_I2cCmd_t)0x01)
#define DEV_PWRMON_REG_VBUS         ((dev_PwrMon_I2cCmd_t)0x02)
#define DEV_PWRMON_REG_POWER        ((dev_PwrMon_I2cCmd_t)0x03)
#define DEV_PWRMON_REG_CURRENT      ((dev_PwrMon_I2cCmd_t)0x04)
#define DEV_PWRMON_REG_CALIBRATION  ((dev_PwrMon_I2cCmd_t)0x05)
#define DEV_PWRMON_REG_MASK_EN      ((dev_PwrMon_I2cCmd_t)0x06)
#define DEV_PWRMON_REG_ALERT_LIMIT  ((dev_PwrMon_I2cCmd_t)0x07)
#define DEV_PWRMON_REG_MFTR_ID      ((dev_PwrMon_I2cCmd_t)0xFE)
#define DEV_PWRMON_REG_DEV_ID       ((dev_PwrMon_I2cCmd_t)0xFF)
#define DEV_PWRMON_REG_INVALID      ((dev_PwrMon_I2cCmd_t)0xAA)
typedef uint8_t dev_PwrMon_I2cCmd_t;

#define DEV_PWRMON_REG_TYPE_RO true
#define DEV_PWRMON_REG_TYPE_RW false
typedef uint8_t dev_PwrMon_RegType_t;

#define DEV_PWRMON_REG_CONFIG_BUILD( _mode, _sConv, _bConv, _avg )  ((( (_mode) & 0x07) << 0) | \
                                                                     (((_sConv) & 0x07) << 3) | \
                                                                     (((_bConv) & 0x07) << 6) | \
                                                                     ((  (_avg) & 0x07) << 9))
#define DEV_PWRMON_REG_ALERT_MASK_CVRF(_reg) ((((uint16_t)(_reg) >> 3) & 0x01) == 0x01)
#define DEV_PWRMON_REG_CONFIG_RESET 0x8000

/*=============================================================================
 *                                   Types
 *===========================================================================*/
/*===========================================================================*/
typedef uint16_t dev_PwrMon_Reg_t;

typedef struct dev_PwrMon_RegInfo_s
{
    dev_PwrMon_I2cCmd_t cmd;
    uint8_t             len;
    bool                rdOnly;
} dev_PwrMon_RegInfo_t;

typedef struct dev_PwrMon_DeviceCtx_s
{
    bsp_I2c_MasterTrans_t i2cTrans;
    uint8_t               wLen;
    uint8_t               wBuffer[3];
    uint8_t               rLen;
    void*                 rPtr;
    dev_PwrMon_Callback_t callback;
    void*                 cbData;

    bool                        active;
    dev_PwrMon_I2cCmd_t         prevRegId;
    dev_PwrMon_Config_t         config;
    dev_PwrMon_Cal_t            cal;
    dev_PwrMon_AlertMask_t      alertMask;
    dev_PwrMon_DeviceId_t       deviceId;
    dev_PwrMon_ManufacturerId_t mftrId;
} dev_PwrMon_DeviceCtx_t;

typedef struct dev_PwrMon_DeviceInfo_s
{
    bsp_I2c_Id_t            i2cId;
    bsp_I2c_Addr_t          i2cAddr;
    bsp_I2c_Speed_t         i2cSpeed;
    dev_PwrMon_ChannelId_t  channelId;
    dev_PwrMon_DeviceCtx_t* ctx;
} dev_PwrMon_DeviceInfo_t;

#define DEV_PWR_MGR_RAIL_INFO_VBUS_IDX    0
#define DEV_PWR_MGR_RAIL_INFO_VSHUNT_IDX  1
#define DEV_PWR_MGR_RAIL_INFO_CURRENT_IDX 2
#define DEV_PWR_MGR_RAIL_INFO_POWER_IDX   3
typedef struct dev_PwrMon_ChannelDevices_s
{
    dev_PwrMon_DevId_t devices[4]; // bus, shunt, current, power
} dev_PwrMon_ChannelDevices_t;

typedef struct dev_PwrMon_ChannelInfo_s
{
    dev_PwrMon_DevId_t vBus;
    dev_PwrMon_DevId_t vShunt;
    dev_PwrMon_DevId_t current;
    dev_PwrMon_DevId_t power;
    bsp_Gpio_PortId_t  calPort;
    bsp_Gpio_BitMask_t calMask;
} dev_PwrMon_ChannelInfo_t;


// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_PwrMon_UsrDataCallback_t)( void );

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
static const dev_PwrMon_RegInfo_t dev_PwrMon_regInfo[] =
{
    { DEV_PWRMON_REG_CONFIG,      2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_VSHUNT,      2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_VBUS,        2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_POWER,       2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_CURRENT,     2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_CALIBRATION, 2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_MASK_EN,     2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_ALERT_LIMIT, 2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_MFTR_ID,     2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_DEV_ID,      2, DEV_PWRMON_REG_TYPE_RO },
};

dev_PwrMon_DeviceCtx_t dev_PwrMon_deviceCtx[BSP_PLATFORM_PWRMON_NUM_DEVICES];
const dev_PwrMon_DeviceInfo_t dev_PwrMon_deviceInfo[] = BSP_PLATFORM_PWRMON_DEVICE_TABLE(dev_PwrMon_deviceCtx);
const dev_PwrMon_ChannelInfo_t dev_PwrMon_channelInfo[] = BSP_PLATFORM_PWRMON_CHANNEL_MAP_TABLE;

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
static inline const dev_PwrMon_RegInfo_t*
dev_PwrMon_getRegInfo( dev_PwrMon_I2cCmd_t regId )
{
    if( regId <= DEV_PWRMON_REG_ALERT_LIMIT )
    {
        return( &dev_PwrMon_regInfo[regId] );
    }
    else
    {
        volatile uint32_t idx = (DEV_PWRMON_REG_ALERT_LIMIT + 1) + (regId - DEV_PWRMON_REG_MFTR_ID);
        return(&dev_PwrMon_regInfo[idx]);
    }
}

/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_PwrMon_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    dev_PwrMon_DeviceCtx_t* ctx = (dev_PwrMon_DeviceCtx_t*)usrData;
    ctx->active = false;
    if( ctx->callback != NULL ) { ctx->callback( ctx->cbData ); }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
static void
dev_PwrMon_i2cTransQueue( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    devPtr->ctx->i2cTrans.speed    = devPtr->i2cSpeed;
    devPtr->ctx->i2cTrans.addr     = devPtr->i2cAddr;
    devPtr->ctx->i2cTrans.callback = dev_PwrMon_i2cTransCallback;
    devPtr->ctx->i2cTrans.usrData  = devPtr->ctx;
    devPtr->ctx->active            = true;
    bsp_I2c_masterTransQueue( devPtr->i2cId, &devPtr->ctx->i2cTrans );
    return;
}

/*===========================================================================*/
// Wrapper to write to the config register on the INA228.
static void
dev_PwrMon_i2cRegWrite( const dev_PwrMon_DeviceInfo_t* devPtr,
                        dev_PwrMon_I2cCmd_t            regId,
                        uint16_t                       regValue )
{
    devPtr->ctx->wBuffer[0] = regId;
    devPtr->ctx->wBuffer[1] = ((regValue >> 8) & 0xFF);
    devPtr->ctx->wBuffer[2] = ((regValue >> 0) & 0xFF);

    devPtr->ctx->prevRegId = regId;

    devPtr->ctx->i2cTrans.type     = BSP_I2C_TRANS_TYPE_WRITE;
    devPtr->ctx->i2cTrans.wLen     = 3; // All writable registers are 2 bytes plus 1 byte address
    devPtr->ctx->i2cTrans.wBuffer  = devPtr->ctx->wBuffer;
    devPtr->ctx->i2cTrans.rLen     = 0;
    devPtr->ctx->i2cTrans.rBuffer  = NULL;
    devPtr->ctx->i2cTrans.rReverse = false;
    dev_PwrMon_i2cTransQueue( devPtr );

    // Spin for completion if no callback
    while( (devPtr->ctx->callback == NULL) && (devPtr->ctx->active == true) );
}

/*===========================================================================*/
// Wrapper to read data from the INA226.
static void
dev_PwrMon_i2cRegRead( const dev_PwrMon_DeviceInfo_t* devPtr,
                       dev_PwrMon_I2cCmd_t            regId )
{
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_getRegInfo( regId );

    devPtr->ctx->i2cTrans.rLen     = regInfoPtr->len;
    devPtr->ctx->i2cTrans.rBuffer  = devPtr->ctx->rPtr;
    devPtr->ctx->i2cTrans.rReverse = true;
    if( regId != devPtr->ctx->prevRegId )
    {
        devPtr->ctx->wBuffer[0]       = regId;
        devPtr->ctx->i2cTrans.type    = BSP_I2C_TRANS_TYPE_WRITE_READ;
        devPtr->ctx->i2cTrans.wLen    = 1;
        devPtr->ctx->i2cTrans.wBuffer = devPtr->ctx->wBuffer;
    }
    else
    {
        devPtr->ctx->i2cTrans.type    = BSP_I2C_TRANS_TYPE_READ;
        devPtr->ctx->i2cTrans.wLen    = 0;
        devPtr->ctx->i2cTrans.wBuffer = NULL;
    }
    devPtr->ctx->prevRegId = regId;
    dev_PwrMon_i2cTransQueue( devPtr );

    // Spin for completion if no callback
    while( (devPtr->ctx->callback == NULL) && (devPtr->ctx->active == true) );
}


/*===========================================================================*/
static void
dev_PwrMon_commonRead( dev_PwrMon_I2cCmd_t    regId,
                       dev_PwrMon_DevId_t     devId,
                       void*                  dataPtr,
                       dev_PwrMon_Callback_t  callback,
                       void*                  cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    devPtr->ctx->rLen     = sizeof(dev_PwrMon_Data_t);
    devPtr->ctx->rPtr     = dataPtr;
    dev_PwrMon_i2cRegRead( devPtr, regId );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_manufacturerId( dev_PwrMon_DevId_t           devId,
                           dev_PwrMon_ManufacturerId_t* dataPtr,
                           dev_PwrMon_Callback_t        callback,
                           void*                        cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_MFTR_ID,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_deviceId( dev_PwrMon_DevId_t     devId,
                     dev_PwrMon_DeviceId_t* dataPtr,
                     dev_PwrMon_Callback_t  callback,
                     void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_DEV_ID,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_vShuntGet( dev_PwrMon_DevId_t     devId,
                      uint8_t*               dataPtr,
                      dev_PwrMon_Callback_t  callback,
                      void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_VSHUNT,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_vBusGet( dev_PwrMon_DevId_t     devId,
                    uint8_t*               dataPtr,
                    dev_PwrMon_Callback_t  callback,
                    void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_VBUS,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_powerGet( dev_PwrMon_DevId_t     devId,
                     uint8_t*               dataPtr,
                     dev_PwrMon_Callback_t  callback,
                     void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_POWER,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_currentGet( dev_PwrMon_DevId_t     devId,
                       uint8_t*               dataPtr,
                       dev_PwrMon_Callback_t  callback,
                       void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_CURRENT,
                           devId, dataPtr, callback, cbData );
}

/*===========================================================================*/
static void
dev_PwrMon_alertMaskSet( dev_PwrMon_DevId_t     devId,
                         dev_PwrMon_Data_t      data,
                         dev_PwrMon_Callback_t  callback,
                         void*                  cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback  = callback;
    devPtr->ctx->cbData    = cbData;
    devPtr->ctx->alertMask = data;
    dev_PwrMon_i2cRegWrite( devPtr, DEV_PWRMON_REG_MASK_EN, data );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_alertMaskGet( dev_PwrMon_DevId_t     devId,
                         uint8_t*               dataPtr,
                         dev_PwrMon_Callback_t  callback,
                         void*                  cbData )
{
    dev_PwrMon_commonRead( DEV_PWRMON_REG_MASK_EN,
                           devId, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_configSet( dev_PwrMon_DevId_t     devId,
                      dev_PwrMon_Data_t      data,
                      dev_PwrMon_Callback_t  callback,
                      void*                  cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    devPtr->ctx->config   = data;
    dev_PwrMon_i2cRegWrite( devPtr, DEV_PWRMON_REG_CONFIG, data );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_calSet( dev_PwrMon_DevId_t     devId,
                   dev_PwrMon_Data_t      data,
                   dev_PwrMon_Callback_t  callback,
                   void*                  cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    devPtr->ctx->cal      = data;
    dev_PwrMon_i2cRegWrite( devPtr, DEV_PWRMON_REG_CALIBRATION, data );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_calInit( dev_PwrMon_ChannelId_t channelId )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    bsp_Gpio_configOutput( channelPtr->calPort, channelPtr->calMask, false, BSP_GPIO_DRIVE_2MA );
    bsp_Gpio_write( channelPtr->calPort, channelPtr->calMask, channelPtr->calMask );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_calEnable( dev_PwrMon_ChannelId_t channelId )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    bsp_Gpio_write( channelPtr->calPort, channelPtr->calMask, 0 );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_calDisable( dev_PwrMon_ChannelId_t channelId )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    bsp_Gpio_write( channelPtr->calPort, channelPtr->calMask, channelPtr->calMask );
    return;
}
#define CAL_AVG_VBUS 256
/*===========================================================================*/
int16_t
dev_PwrMon_calDeviceVbus( dev_PwrMon_DevId_t devId )
{
    BSP_GPIO_OUT_SET_HIGH( TPE2 );
    dev_PwrMon_Data_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( BSP_PWRMOMN_OP_MODE_PWR_DWN,
                                          BSP_PWRMOMN_CONV_TIME_US_140,
                                          BSP_PWRMOMN_CONV_TIME_US_140,
                                          BSP_PWRMOMN_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_configSet( devId, tmpReg, NULL, 0 );

    int32_t vBusSum = 0;
    int16_t vBus;
    for( int i=0; i<CAL_AVG_VBUS; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( BSP_PWRMOMN_OP_MODE_TRIG_BUS,
                                              BSP_PWRMOMN_CONV_TIME_US_332,
                                              BSP_PWRMOMN_CONV_TIME_US_332,
                                              BSP_PWRMOMN_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_configSet( devId, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_alertMaskGet( devId, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        BSP_GPIO_OUT_SET_HIGH( TPE3 );
        dev_PwrMon_vBusGet( devId, (uint8_t*)&vBus, NULL, 0 );
        BSP_GPIO_OUT_SET_LOW( TPE3 );
        vBusSum += vBus;
    }
    vBus = (vBusSum / CAL_AVG_VBUS);
    BSP_GPIO_OUT_SET_LOW( TPE2 );

    return( vBus );
}

#define CAL_AVG_VSHUNT 256
/*===========================================================================*/
int16_t
dev_PwrMon_calDeviceVshunt( dev_PwrMon_DevId_t devId )
{
    BSP_GPIO_OUT_SET_HIGH( TPE3 );
    dev_PwrMon_Data_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( BSP_PWRMOMN_OP_MODE_PWR_DWN,
                                          BSP_PWRMOMN_CONV_TIME_US_140,
                                          BSP_PWRMOMN_CONV_TIME_US_140,
                                          BSP_PWRMOMN_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_configSet( devId, tmpReg, NULL, 0 );

    int32_t vShuntSum = 0;
    int16_t vShunt;
    for( int i=0; i<CAL_AVG_VSHUNT; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( BSP_PWRMOMN_OP_MODE_TRIG_SHUNT,
                                              BSP_PWRMOMN_CONV_TIME_US_332,
                                              BSP_PWRMOMN_CONV_TIME_US_332,
                                              BSP_PWRMOMN_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_configSet( devId, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_alertMaskGet( devId, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        BSP_GPIO_OUT_SET_HIGH( TPE2 );
        dev_PwrMon_vShuntGet( devId, (uint8_t*)&vShunt, NULL, 0 );
        BSP_GPIO_OUT_SET_LOW( TPE2 );
        vShuntSum += vShunt;
    }
    vShunt = (vShuntSum / CAL_AVG_VSHUNT);
    BSP_GPIO_OUT_SET_LOW( TPE3 );

    return vShunt;
}

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_PwrMon_init( void )
{
    uint16_t i2cBusInitMask = 0x0000;
    for( uint8_t i=0; i < BSP_PLATFORM_PWRMON_NUM_DEVICES; i++ )
    {
        dev_PwrMon_deviceCtx[i].prevRegId = DEV_PWRMON_REG_INVALID;
        dev_PwrMon_deviceCtx[i].callback  = NULL;

        // Only init busses that haven't already been initialized
        if ( (i2cBusInitMask & (1 << dev_PwrMon_deviceInfo[i].i2cId)) == 0 )
        {
            i2cBusInitMask |= (1 << dev_PwrMon_deviceInfo[i].i2cId);
            bsp_I2c_masterControl( dev_PwrMon_deviceInfo[i].i2cId, BSP_I2C_CONTROL_ENABLE );
        }

        dev_PwrMon_manufacturerId( i, &dev_PwrMon_deviceCtx[i].mftrId, NULL, NULL );
        dev_PwrMon_deviceId( i, &dev_PwrMon_deviceCtx[i].deviceId, NULL, NULL );

        // Initialize:
        // reset each device and disable every alert and set each cal reg to 0
        dev_PwrMon_configSet( i, DEV_PWRMON_REG_CONFIG_RESET, NULL, NULL );
        dev_PwrMon_alertMaskSet( i, 0x0000, NULL, NULL );
        dev_PwrMon_calSet( i, 0x0000, NULL, NULL );
    }

    // For each channel, configure the default settings
    for (uint8_t i=0; i<DIM(dev_PwrMon_channelInfo); i++)
    {
        dev_PwrMon_calInit( i );
        dev_PwrMon_channelConfig( i,
                                  BSP_PWRMOMN_CONV_TIME_US_140,
                                  BSP_PWRMOMN_CONV_TIME_US_140,
                                  BSP_PWRMOMN_AVG_MODE_SAMPLES_1,
                                  NULL, NULL );
    }

    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelOffsetCal( dev_PwrMon_ChannelId_t channelId,
                             dev_PwrMon_Data_t*     vBusPtr,
                             dev_PwrMon_Data_t*     vShuntPtr )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];

    // Enable the Calibration IO to short VIN +/- and VBUS
    dev_PwrMon_calEnable( channelId );
    bsp_Clk_delayMs(50);

    *vShuntPtr = dev_PwrMon_calDeviceVshunt(channelPtr->vShunt);
    *vBusPtr = dev_PwrMon_calDeviceVbus(channelPtr->vBus);

    // Disable the Calibration IO to short Vin +/-
    dev_PwrMon_calDisable( channelId );

    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelConfig( dev_PwrMon_ChannelId_t channelId,
                          dev_PwrMon_ConvTime_t  shuntConvTime,
                          dev_PwrMon_ConvTime_t  busConvTime,
                          dev_PwrMon_AvgMode_t   avgMode,
                          dev_PwrMon_Callback_t  callback,
                          void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_ChannelDevices_t* devicesPtr = (const dev_PwrMon_ChannelDevices_t*)channelPtr;

    // For each device on the channel make sure it's conversion mode
    // is set properly.
    for (uint8_t i=0; i<DIM(devicesPtr->devices); i++)
    {
        dev_PwrMon_Data_t tmpReg;
        dev_PwrMon_OpMode_t mode = BSP_PWRMOMN_OP_MODE_PWR_DWN;
        if( (channelPtr->vBus   == devicesPtr->devices[i]) &&
            (channelPtr->vShunt == devicesPtr->devices[i]) )
        {
            mode = BSP_PWRMOMN_OP_MODE_CONT_SHUNT_AND_BUS;
        }
        else if( channelPtr->vBus == devicesPtr->devices[i] )
        {
            mode = BSP_PWRMOMN_OP_MODE_CONT_BUS;
        }
        else
        {
            mode = BSP_PWRMOMN_OP_MODE_CONT_SHUNT;
        }
        tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( mode, shuntConvTime, busConvTime, avgMode );

        // Callback only on the last one
        dev_PwrMon_Callback_t cb;
        cb = (i == (DIM(devicesPtr->devices) - 1)) ? NULL : callback;
        dev_PwrMon_configSet( devicesPtr->devices[i], tmpReg, cb, cbData );
    }

    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelConfigCal( dev_PwrMon_ChannelId_t channelId,
                             dev_PwrMon_Cal_t       cal,
                             dev_PwrMon_Callback_t  callback,
                             void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_ChannelDevices_t* devicesPtr = (const dev_PwrMon_ChannelDevices_t*)channelPtr;

    for( uint8_t i=0; i<DIM(devicesPtr->devices); i++ )
    {
        // Only setup shunt for devices measuring current or power
        if( (channelPtr->current == devicesPtr->devices[i]) ||
            (channelPtr->power   == devicesPtr->devices[i]) )
        {
            // Callback only on the last one
            dev_PwrMon_Callback_t cb;
            cb = (i == (DIM(devicesPtr->devices) - 1)) ? NULL : callback;
            dev_PwrMon_calSet( devicesPtr->devices[i], cal, cb, cbData );
        }
    }

    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelCurrentRead( dev_PwrMon_ChannelId_t channelId,
                               uint8_t*               dataPtr,
                               dev_PwrMon_Callback_t  callback,
                               void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    dev_PwrMon_currentGet( channelPtr->current, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelBusVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                  uint8_t*               dataPtr,
                                  dev_PwrMon_Callback_t  callback,
                                  void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    dev_PwrMon_vBusGet( channelPtr->vBus, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelShuntVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                    uint8_t*               dataPtr,
                                    dev_PwrMon_Callback_t  callback,
                                    void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    dev_PwrMon_vShuntGet( channelPtr->vShunt, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelPowerRead( dev_PwrMon_ChannelId_t channelId,
                             uint8_t*               dataPtr,
                             dev_PwrMon_Callback_t  callback,
                             void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* channelPtr = &dev_PwrMon_channelInfo[channelId];
    dev_PwrMon_powerGet( channelPtr->power, dataPtr, callback, cbData );
    return;
}

#endif
