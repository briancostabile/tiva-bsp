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
 * @brief Contains Driver for the INA226 Power Monitor.
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
#include "dev_PwrMon_ina226.h"


#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/
enum
{
    DEV_PWRMON_AVG_MODE_SAMPLES_1    = 0x00,
    DEV_PWRMON_AVG_MODE_SAMPLES_4    = 0x01,
    DEV_PWRMON_AVG_MODE_SAMPLES_16   = 0x02,
    DEV_PWRMON_AVG_MODE_SAMPLES_64   = 0x03,
    DEV_PWRMON_AVG_MODE_SAMPLES_128  = 0x04,
    DEV_PWRMON_AVG_MODE_SAMPLES_256  = 0x05,
    DEV_PWRMON_AVG_MODE_SAMPLES_512  = 0x06,
    DEV_PWRMON_AVG_MODE_SAMPLES_1024 = 0x07
};
typedef uint8_t dev_PwrMon_AvgMode_t;

enum
{
    DEV_PWRMON_CONV_TIME_US_140   = 0x00,
    DEV_PWRMON_CONV_TIME_US_204   = 0x01,
    DEV_PWRMON_CONV_TIME_US_332   = 0x02,
    DEV_PWRMON_CONV_TIME_US_558   = 0x03,
    DEV_PWRMON_CONV_TIME_US_1100  = 0x04,
    DEV_PWRMON_CONV_TIME_US_2116  = 0x05,
    DEV_PWRMON_CONV_TIME_US_4156  = 0x06,
    DEV_PWRMON_CONV_TIME_US_8244  = 0x07
};
typedef uint8_t dev_PwrMon_ConvTime_t;

enum
{
    DEV_PWRMON_OP_MODE_PWR_DWN            = 0x00,
    DEV_PWRMON_OP_MODE_TRIG_SHUNT         = 0x01,
    DEV_PWRMON_OP_MODE_TRIG_BUS           = 0x02,
    DEV_PWRMON_OP_MODE_TRIG_SHUNT_AND_BUS = 0x03,
    DEV_PWRMON_OP_MODE_CONT_SHUNT         = 0x05,
    DEV_PWRMON_OP_MODE_CONT_BUS           = 0x06,
    DEV_PWRMON_OP_MODE_CONT_SHUNT_AND_BUS = 0x07,
};
typedef uint8_t dev_PwrMon_OpMode_t;


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


#define DEV_PWRMON_REG_CONFIG_BUILD( _mode, _sConv, _bConv, _avg )  ((( (_mode) & 0x07) << 0) | \
                                                                     (((_sConv) & 0x07) << 3) | \
                                                                     (((_bConv) & 0x07) << 6) | \
                                                                     ((  (_avg) & 0x07) << 9))
#define DEV_PWRMON_REG_ALERT_MASK_CVRF(_reg) ((((uint16_t)(_reg) >> 3) & 0x01) == 0x01)
#define DEV_PWRMON_REG_CONFIG_RESET 0x8000


/*=============================================================================
 *                                   Globals
 *===========================================================================*/
static const dev_PwrMon_RegInfo_t dev_PwrMon_ina226_regInfo[] =
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


/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
static inline const dev_PwrMon_RegInfo_t*
dev_PwrMon_ina226_getRegInfo( dev_PwrMon_I2cCmd_t regId )
{
    if( regId <= DEV_PWRMON_REG_ALERT_LIMIT )
    {
        return( &dev_PwrMon_ina226_regInfo[regId] );
    }
    else
    {
        volatile uint32_t idx = (DEV_PWRMON_REG_ALERT_LIMIT + 1) + (regId - DEV_PWRMON_REG_MFTR_ID);
        return(&dev_PwrMon_ina226_regInfo[idx]);
    }
}

/*=============================================================================
 *                                Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_PwrMon_ina226_manufacturerId( const dev_PwrMon_DeviceInfo_t* devPtr,
                                  dev_PwrMon_ManufacturerId_t*   dataPtr,
                                  dev_PwrMon_Callback_t          callback,
                                  void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_MFTR_ID;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_deviceId( const dev_PwrMon_DeviceInfo_t* devPtr,
                            dev_PwrMon_DeviceId_t*         dataPtr,
                            dev_PwrMon_Callback_t          callback,
                            void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_DEV_ID;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_vShuntGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             uint8_t*                       dataPtr,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_VSHUNT;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_vBusGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                           uint8_t*                       dataPtr,
                           dev_PwrMon_Callback_t          callback,
                           void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_VBUS;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_powerGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                            uint8_t*                       dataPtr,
                            dev_PwrMon_Callback_t          callback,
                            void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_POWER;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_currentGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                              uint8_t*                       dataPtr,
                              dev_PwrMon_Callback_t          callback,
                              void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_CURRENT;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina226_alertMaskSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_MASK_EN, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina226_alertMaskGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                uint8_t*                       dataPtr,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_MASK_EN;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina226_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina226_configSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             dev_PwrMon_WriteData_t         data,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_CONFIG, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina226_calSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                          dev_PwrMon_WriteData_t         data,
                          dev_PwrMon_Callback_t          callback,
                          void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_CALIBRATION, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina226_init( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    // Read out the device and manufacturer Ids
    dev_PwrMon_ina226_manufacturerId( devPtr, &devPtr->ctx->mftrId, NULL, NULL );
    dev_PwrMon_ina226_deviceId( devPtr, &devPtr->ctx->deviceId, NULL, NULL );

    // reset device, disable alert and set each cal reg to 0
    dev_PwrMon_ina226_configSet( devPtr, DEV_PWRMON_REG_CONFIG_RESET, NULL, NULL );
    dev_PwrMon_ina226_alertMaskSet( devPtr, 0x0000, NULL, NULL );
    dev_PwrMon_ina226_calSet( devPtr, 0x0000, NULL, NULL );

    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina226_config( const dev_PwrMon_DeviceInfo_t* devPtr,
                          dev_PwrMon_DevModeMask_t       modeMask,
                          dev_PwrMon_Callback_t          callback,
                          void*                          cbData )
{
    uint16_t tmpReg;
    dev_PwrMon_OpMode_t mode = DEV_PWRMON_OP_MODE_PWR_DWN;

    if( (modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_SHUNT) &&
        (modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_BUS) )
    {
        mode = DEV_PWRMON_OP_MODE_CONT_SHUNT_AND_BUS;
    }
    else if( modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_BUS )
    {
        mode = DEV_PWRMON_OP_MODE_CONT_BUS;
    }
    else
    {
        mode = DEV_PWRMON_OP_MODE_CONT_SHUNT;
    }

    tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( mode,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_ina226_configSet( devPtr, tmpReg, callback, cbData );

    return;
}

#define CAL_AVG_VBUS 256
/*===========================================================================*/
int32_t
dev_PwrMon_ina226_vBusCal( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    uint16_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( DEV_PWRMON_OP_MODE_PWR_DWN,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_ina226_configSet( devPtr, tmpReg, NULL, 0 );

    int32_t vBusSum = 0;
    int16_t vBus;
    for( int i=0; i<CAL_AVG_VBUS; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( DEV_PWRMON_OP_MODE_TRIG_BUS,
                                              DEV_PWRMON_CONV_TIME_US_332,
                                              DEV_PWRMON_CONV_TIME_US_332,
                                              DEV_PWRMON_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_ina226_configSet( devPtr, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_ina226_alertMaskGet( devPtr, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        dev_PwrMon_ina226_vBusGet( devPtr, (uint8_t*)&vBus, NULL, 0 );
        vBusSum += vBus;
    }
    vBus = (vBusSum / CAL_AVG_VBUS);

    return( (int32_t)vBus );
}

#define CAL_AVG_VSHUNT 256
/*===========================================================================*/
int32_t
dev_PwrMon_ina226_vShuntCal( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    dev_PwrMon_Data_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( DEV_PWRMON_OP_MODE_PWR_DWN,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_CONV_TIME_US_332,
                                          DEV_PWRMON_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_ina226_configSet( devPtr, tmpReg, NULL, 0 );

    int32_t vShuntSum = 0;
    int16_t vShunt;
    for( int i=0; i<CAL_AVG_VSHUNT; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_CONFIG_BUILD( DEV_PWRMON_OP_MODE_TRIG_SHUNT,
                                              DEV_PWRMON_CONV_TIME_US_332,
                                              DEV_PWRMON_CONV_TIME_US_332,
                                              DEV_PWRMON_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_ina226_configSet( devPtr, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_ina226_alertMaskGet( devPtr, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        dev_PwrMon_ina226_vShuntGet( devPtr, (uint8_t*)&vShunt, NULL, 0 );
        vShuntSum += vShunt;
    }
    vShunt = (vShuntSum / CAL_AVG_VSHUNT);

    return( (int32_t)vShunt );
}

#define SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV(_val) (((_val) * 1250) / 1000)
/*===========================================================================*/
int32_t
dev_PwrMon_ina226_vBusConvert( int32_t val )
{
    return( SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV(val) );
}

#define SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV(_val) (((_val) * 25) / 10)
/*===========================================================================*/
int32_t
dev_PwrMon_ina226_vShuntConvert( int32_t val )
{
    return( SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV(val) );
}

/*===========================================================================*/
int32_t
dev_PwrMon_ina226_format( int32_t val )
{
    // Sign extend: If 16th bit is 1 then it's negative
    if( (val & 0x00008000) != 0 )
    {
        val |= 0xFFFF0000;
    }
    return(val);
}

/*===========================================================================*/
dev_PwrMon_DeviceApi_t dev_PwrMon_ina226_deviceApi =
{
    dev_PwrMon_ina226_init,
    dev_PwrMon_ina226_config,
    {
        dev_PwrMon_ina226_vBusGet,
        dev_PwrMon_ina226_vBusCal,
        dev_PwrMon_ina226_format,
        dev_PwrMon_ina226_vBusConvert
    },
    {
        dev_PwrMon_ina226_vShuntGet,
        dev_PwrMon_ina226_vShuntCal,
        dev_PwrMon_ina226_format,
        dev_PwrMon_ina226_vShuntConvert
    }
};

#endif
