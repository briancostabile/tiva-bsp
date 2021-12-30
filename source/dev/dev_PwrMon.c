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
 * @file dev_PwrMon.c
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
#include "dev_PwrMon_ina.h"
#include "dev_PwrMon_ina226.h"
#include "dev_PwrMon_ina228.h"

#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226) || defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)


// How long after activating the calibration short before taking a measurement
#define DEV_PWRMON_CAL_SHORT_DELAY_MS 50

/*=============================================================================
 *                                   Types
 *===========================================================================*/
#define DEV_PWR_MGR_RAIL_INFO_VSHUNT_IDX  0
#define DEV_PWR_MGR_RAIL_INFO_VBUS_IDX    1
typedef struct dev_PwrMon_ChannelDevices_s
{
    dev_PwrMon_DevId_t devices[2]; // shunt, bus
} dev_PwrMon_ChannelDevices_t;

typedef struct dev_PwrMon_ChannelInfo_s
{
    dev_PwrMon_DevId_t vShunt;
    dev_PwrMon_DevId_t vBus;
    bsp_Gpio_PortId_t  calPort;
    bsp_Gpio_BitMask_t calMask;
} dev_PwrMon_ChannelInfo_t;

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
dev_PwrMon_DeviceCtx_t dev_PwrMon_deviceCtx[BSP_PLATFORM_PWRMON_NUM_DEVICES];
const dev_PwrMon_DeviceInfo_t dev_PwrMon_deviceInfo[] = BSP_PLATFORM_PWRMON_DEVICE_TABLE(dev_PwrMon_deviceCtx);
const dev_PwrMon_ChannelInfo_t dev_PwrMon_channelInfo[] = BSP_PLATFORM_PWRMON_CHANNEL_MAP_TABLE;


/*===========================================================================*/
// Order must match the enumeration for dev_PwrMon_DeviceType_t
dev_PwrMon_DeviceApi_t* dev_PwrMon_deviceApiTable[] =
{
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
    &dev_PwrMon_ina226_deviceApi,
#else
    NULL,
#endif
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)
    &dev_PwrMon_ina228_deviceApi
#else
    NULL
#endif
};

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
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
// Wrapper to write to the config register on the INA226/228.
static void
dev_PwrMon_i2cRegWrite( const dev_PwrMon_DeviceInfo_t* devPtr,
                        uint8_t                        regId,
                        dev_PwrMon_WriteData_t         regValue )
{
    devPtr->ctx->wBuffer[0] = regId;
    devPtr->ctx->wBuffer[1] = ((regValue >> 8) & 0xFF);
    devPtr->ctx->wBuffer[2] = ((regValue >> 0) & 0xFF);

    devPtr->ctx->prevRegId = regId;

    devPtr->ctx->i2cTrans.type     = BSP_I2C_TRANS_TYPE_WRITE;
    devPtr->ctx->i2cTrans.wLen     = (2 + 1); // All INA writable regs are 2 bytes, plus 1 byte address
    devPtr->ctx->i2cTrans.wBuffer  = devPtr->ctx->wBuffer;
    devPtr->ctx->i2cTrans.rLen     = 0;
    devPtr->ctx->i2cTrans.rBuffer  = NULL;
    devPtr->ctx->i2cTrans.rReverse = false;
    dev_PwrMon_i2cTransQueue( devPtr );

    // Spin for completion if no callback
    while( (devPtr->ctx->callback == NULL) && (devPtr->ctx->active == true) );
}

/*===========================================================================*/
// Wrapper to read data from the INA226/228.
static void
dev_PwrMon_i2cRegRead( const dev_PwrMon_DeviceInfo_t* devPtr,
                       uint8_t                        regId,
                       uint8_t                        regLen )
{
    devPtr->ctx->i2cTrans.rLen     = regLen;
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
/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_PwrMon_commonRead( const dev_PwrMon_DeviceInfo_t* devPtr,
                       uint8_t                        regId,
                       uint8_t                        regLen,
                       void*                          dataPtr,
                       dev_PwrMon_Callback_t          callback,
                       void*                          cbData )
{
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    devPtr->ctx->rLen     = regLen;
    devPtr->ctx->rPtr     = dataPtr;
    dev_PwrMon_i2cRegRead( devPtr, regId, regLen );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_commonWrite( const dev_PwrMon_DeviceInfo_t* devPtr,
                        uint8_t                        regId,
                        dev_PwrMon_WriteData_t         data,
                        dev_PwrMon_Callback_t          callback,
                        void*                          cbData )
{
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    dev_PwrMon_i2cRegWrite( devPtr, regId, data );
    return;
}

/*===========================================================================*/
// Go device by device initializing the contexts and devices. Then initiailze
// the calibration and configure devices to be off
void
dev_PwrMon_init( void )
{
    const dev_PwrMon_ChannelInfo_t* chInfoPtr;
    const dev_PwrMon_DeviceInfo_t*  devInfoPtr;
    const dev_PwrMon_DeviceApi_t*   devApiPtr;

    // Initialize each i2c bus and each device on the bus
    uint16_t i2cBusInitMask = 0x0000;
    for( uint8_t i=0; i < BSP_PLATFORM_PWRMON_NUM_DEVICES; i++ )
    {
        dev_PwrMon_deviceCtx[i].prevRegId = DEV_PWRMON_REG_INVALID;
        dev_PwrMon_deviceCtx[i].callback  = NULL;

        devInfoPtr = &dev_PwrMon_deviceInfo[i];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
        // Only init busses that haven't already been initialized
        if ( (i2cBusInitMask & (1 << devInfoPtr->i2cId)) == 0 )
        {
            i2cBusInitMask |= (1 << devInfoPtr->i2cId);
            bsp_I2c_masterControl( devInfoPtr->i2cId, BSP_I2C_CONTROL_ENABLE_HS );
        }

        devApiPtr->init(&dev_PwrMon_deviceInfo[i]);
    }

    // For each channel, configure the default settings
    for (uint8_t i=0; i<DIM(dev_PwrMon_channelInfo); i++)
    {
        chInfoPtr  = &dev_PwrMon_channelInfo[i];
        devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];

        devApiPtr->config( &dev_PwrMon_deviceInfo[chInfoPtr->vShunt],
                           DEV_PWR_MON_DEVICE_MODE_MASK_SHUNT, NULL, NULL );

        devApiPtr->config( &dev_PwrMon_deviceInfo[chInfoPtr->vBus],
                           DEV_PWR_MON_DEVICE_MODE_MASK_BUS, NULL, NULL );

        dev_PwrMon_calInit( i );
    }

    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelOffsetCal( dev_PwrMon_ChannelId_t channelId,
                             dev_PwrMon_Data_t*     vBusPtr,
                             dev_PwrMon_Data_t*     vShuntPtr )
{
    const dev_PwrMon_ChannelInfo_t* chInfoPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t* devInfoPtr;
    const dev_PwrMon_DeviceApi_t* devApiPtr;

    dev_PwrMon_calEnable(channelId);
    bsp_Clk_delayMs(DEV_PWRMON_CAL_SHORT_DELAY_MS);

    devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
    devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    *vShuntPtr = devApiPtr->vShunt.cal( &dev_PwrMon_deviceInfo[chInfoPtr->vShunt] );

    devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vBus];
    devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    *vBusPtr   = devApiPtr->vBus.cal( &dev_PwrMon_deviceInfo[chInfoPtr->vBus] );

    dev_PwrMon_calDisable(channelId);

    return;
}

#define DEV_PWR_MON_DEVICE_API_FROM_CH(_ch, _vId) \
dev_PwrMon_deviceApiTable[ dev_PwrMon_deviceInfo[ dev_PwrMon_channelInfo[_ch]._vId ].devType ]


/*===========================================================================*/
void
dev_PwrMon_channelBusVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                  uint8_t*               dataPtr,
                                  dev_PwrMon_Callback_t  callback,
                                  void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* chInfoPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t* devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vBus];
    const dev_PwrMon_DeviceApi_t* devApiPtr   = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    devApiPtr->vBus.read( &dev_PwrMon_deviceInfo[chInfoPtr->vBus],
                          dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_channelShuntVoltageRead( dev_PwrMon_ChannelId_t channelId,
                                    uint8_t*               dataPtr,
                                    dev_PwrMon_Callback_t  callback,
                                    void*                  cbData )
{
    const dev_PwrMon_ChannelInfo_t* chInfoPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t* devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
    const dev_PwrMon_DeviceApi_t* devApiPtr   = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    devApiPtr->vShunt.read( &dev_PwrMon_deviceInfo[chInfoPtr->vShunt],
                            dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
dev_PwrMon_Data_t
dev_PwrMon_vBusConvert( dev_PwrMon_ChannelId_t channelId,
                        dev_PwrMon_Data_t      val )
{
    const dev_PwrMon_DeviceApi_t* devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vBus);
    return( devApiPtr->vBus.convert(val) );
}

/*===========================================================================*/
dev_PwrMon_Data_t
dev_PwrMon_vShuntConvert( dev_PwrMon_ChannelId_t channelId,
                          dev_PwrMon_Data_t      val )
{
    const dev_PwrMon_DeviceApi_t* devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vShunt);
    return( devApiPtr->vShunt.convert(val) );
}

/*===========================================================================*/
dev_PwrMon_Data_t
dev_PwrMon_vBusFormat( dev_PwrMon_ChannelId_t channelId,
                       dev_PwrMon_Data_t      val )
{
    const dev_PwrMon_DeviceApi_t* devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vBus);
    return( devApiPtr->vBus.format(val) );
}

/*===========================================================================*/
dev_PwrMon_Data_t
dev_PwrMon_vShuntFormat( dev_PwrMon_ChannelId_t channelId,
                         dev_PwrMon_Data_t      val )
{
    const dev_PwrMon_DeviceApi_t* devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vShunt);
    return( devApiPtr->vShunt.format(val) );
}

#endif
