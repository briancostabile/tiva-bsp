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
 * @file dev_PwrMon_ina228.c
 * @brief Contains Driver for the INA228 Power Monitor.
 */
#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_PwrMon.h"
#include "dev_PwrMon_ina.h"
#include "dev_PwrMon_ina228.h"

#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)
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
    DEV_PWRMON_CONV_TIME_US_50   = 0x00,
    DEV_PWRMON_CONV_TIME_US_84   = 0x01,
    DEV_PWRMON_CONV_TIME_US_150  = 0x02,
    DEV_PWRMON_CONV_TIME_US_280  = 0x03,
    DEV_PWRMON_CONV_TIME_US_540  = 0x04,
    DEV_PWRMON_CONV_TIME_US_1052 = 0x05,
    DEV_PWRMON_CONV_TIME_US_2074 = 0x06,
    DEV_PWRMON_CONV_TIME_US_4120 = 0x07
};
typedef uint8_t dev_PwrMon_ConvTime_t;

enum
{
    DEV_PWRMON_OP_MODE_PWR_DWN             = 0x00,

    DEV_PWRMON_OP_MODE_TRIG_BUS            = 0x01,
    DEV_PWRMON_OP_MODE_TRIG_SHUNT          = 0x02,
    DEV_PWRMON_OP_MODE_TRIG_SHUNT_BUS      = 0x03,
    DEV_PWRMON_OP_MODE_TRIG_TEMP           = 0x04,
    DEV_PWRMON_OP_MODE_TRIG_TEMP_BUS       = 0x05,
    DEV_PWRMON_OP_MODE_TRIG_TEMP_SHUNT     = 0x06,
    DEV_PWRMON_OP_MODE_TRIG_TEMP_SHUNT_BUS = 0x07,

    DEV_PWRMON_OP_MODE_CONT_BUS            = 0x09,
    DEV_PWRMON_OP_MODE_CONT_SHUNT          = 0x0A,
    DEV_PWRMON_OP_MODE_CONT_SHUNT_BUS      = 0x0B,
    DEV_PWRMON_OP_MODE_CONT_TEMP           = 0x0C,
    DEV_PWRMON_OP_MODE_CONT_TEMP_BUS       = 0x0D,
    DEV_PWRMON_OP_MODE_CONT_TEMP_SHUNT     = 0x0E,
    DEV_PWRMON_OP_MODE_CONT_TEMP_SHUNT_BUS = 0x0F
};
typedef uint8_t dev_PwrMon_OpMode_t;


#define DEV_PWRMON_REG_CONFIG       ((dev_PwrMon_I2cCmd_t)0x00)
#define DEV_PWRMON_REG_ADC_CONFIG   ((dev_PwrMon_I2cCmd_t)0x01)
#define DEV_PWRMON_REG_SHUNT_CAL    ((dev_PwrMon_I2cCmd_t)0x02)
#define DEV_PWRMON_REG_SHUNT_TEMPCO ((dev_PwrMon_I2cCmd_t)0x03)
#define DEV_PWRMON_REG_VSHUNT       ((dev_PwrMon_I2cCmd_t)0x04)
#define DEV_PWRMON_REG_VBUS         ((dev_PwrMon_I2cCmd_t)0x05)
#define DEV_PWRMON_REG_DIETEMP      ((dev_PwrMon_I2cCmd_t)0x06)
#define DEV_PWRMON_REG_CURRENT      ((dev_PwrMon_I2cCmd_t)0x07)
#define DEV_PWRMON_REG_POWER        ((dev_PwrMon_I2cCmd_t)0x08)
#define DEV_PWRMON_REG_ENERGY       ((dev_PwrMon_I2cCmd_t)0x09)
#define DEV_PWRMON_REG_CHARGE       ((dev_PwrMon_I2cCmd_t)0x0A)
#define DEV_PWRMON_REG_DIAG_ALRT    ((dev_PwrMon_I2cCmd_t)0x0B)
#define DEV_PWRMON_REG_SOVL         ((dev_PwrMon_I2cCmd_t)0x0C)
#define DEV_PWRMON_REG_SUVL         ((dev_PwrMon_I2cCmd_t)0x0D)
#define DEV_PWRMON_REG_BOVL         ((dev_PwrMon_I2cCmd_t)0x0E)
#define DEV_PWRMON_REG_BUVL         ((dev_PwrMon_I2cCmd_t)0x0F)
#define DEV_PWRMON_REG_TEMP_LIMIT   ((dev_PwrMon_I2cCmd_t)0x10)
#define DEV_PWRMON_REG_PWR_LIMIT    ((dev_PwrMon_I2cCmd_t)0x11)
#define DEV_PWRMON_REG_MFTR_ID      ((dev_PwrMon_I2cCmd_t)0x3E)
#define DEV_PWRMON_REG_DEV_ID       ((dev_PwrMon_I2cCmd_t)0x3F)


#define DEV_PWRMON_REG_CONFIG_BUILD( _rst, _rstAcc, _convDly, _tComp, _adcRng ) ((((_adcRng)  & 0x01) <<  4) | \
                                                                                 (((_tComp)   & 0x01) <<  5) | \
                                                                                 (((_convDly) & 0xFF) <<  6) | \
                                                                                 (((_rstAcc)  & 0x01) << 14) | \
                                                                                 (((_rst)     & 0x01) << 15))

#define DEV_PWRMON_REG_ADC_CONFIG_BUILD( _mode, _sConv, _bConv, _tConv, _avg )  ((((_avg)   & 0x07) <<  0) | \
                                                                                 (((_tConv) & 0x07) <<  3) | \
                                                                                 (((_sConv) & 0x07) <<  6) | \
                                                                                 (((_bConv) & 0x07) <<  9) | \
                                                                                 (((_mode)  & 0x0F) << 12))

#define DEV_PWRMON_REG_ALERT_MASK_CVRF(_reg) ((((uint16_t)(_reg) >> 1) & 0x01) == 0x01)
#define DEV_PWRMON_REG_CONFIG_RESET DEV_PWRMON_REG_CONFIG_BUILD(1,1,0,0,0)

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
static const dev_PwrMon_RegInfo_t dev_PwrMon_regInfo[] =
{
    { DEV_PWRMON_REG_CONFIG,       2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_ADC_CONFIG,   2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_SHUNT_CAL,    2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_SHUNT_TEMPCO, 2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_VSHUNT,       3, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_VBUS,         3, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_DIETEMP,      2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_CURRENT,      3, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_POWER,        3, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_ENERGY,       5, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_CHARGE,       5, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_DIAG_ALRT,    2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_SOVL,         2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_SUVL,         2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_BOVL,         2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_BUVL,         2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_TEMP_LIMIT,   2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_PWR_LIMIT,    2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_REG_MFTR_ID,      2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_REG_DEV_ID,       2, DEV_PWRMON_REG_TYPE_RO },
};

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
static inline const dev_PwrMon_RegInfo_t*
dev_PwrMon_ina228_getRegInfo( dev_PwrMon_I2cCmd_t regId )
{
    if( regId <= DEV_PWRMON_REG_PWR_LIMIT )
    {
        return( &dev_PwrMon_regInfo[regId] );
    }
    else
    {
        volatile uint32_t idx = (DEV_PWRMON_REG_PWR_LIMIT + 1) + (regId - DEV_PWRMON_REG_MFTR_ID);
        return(&dev_PwrMon_regInfo[idx]);
    }
}

/*===========================================================================*/
static void
dev_PwrMon_ina228_manufacturerId( const dev_PwrMon_DeviceInfo_t* devPtr,
                                  dev_PwrMon_ManufacturerId_t*   dataPtr,
                                  dev_PwrMon_Callback_t          callback,
                                  void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_MFTR_ID;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_deviceId( const dev_PwrMon_DeviceInfo_t* devPtr,
                            dev_PwrMon_DeviceId_t*         dataPtr,
                            dev_PwrMon_Callback_t          callback,
                            void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_DEV_ID;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_vShuntGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             uint8_t*                       dataPtr,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_VSHUNT;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_vBusGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                           uint8_t*                       dataPtr,
                           dev_PwrMon_Callback_t          callback,
                           void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_VBUS;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_dieTempGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                              uint8_t*                       dataPtr,
                              dev_PwrMon_Callback_t          callback,
                              void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_DIETEMP;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_currentGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                              uint8_t*                       dataPtr,
                              dev_PwrMon_Callback_t          callback,
                              void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_CURRENT;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_powerGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                            uint8_t*                       dataPtr,
                            dev_PwrMon_Callback_t          callback,
                            void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_POWER;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_energyGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                            uint8_t*                       dataPtr,
                            dev_PwrMon_Callback_t          callback,
                            void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_ENERGY;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_chargeGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             uint8_t*                       dataPtr,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_CHARGE;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
}

/*===========================================================================*/
void
dev_PwrMon_ina228_alertMaskGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                uint8_t*                       dataPtr,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_DIAG_ALRT;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_configGet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             uint8_t*                       dataPtr,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_I2cCmd_t         regId = DEV_PWRMON_REG_CONFIG;
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_ina228_getRegInfo( regId );
    dev_PwrMon_commonRead( devPtr, regId, regInfoPtr->len, dataPtr, callback, cbData );
    return;
}



/*===========================================================================*/
void
dev_PwrMon_ina228_configSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                             dev_PwrMon_WriteData_t         data,
                             dev_PwrMon_Callback_t          callback,
                             void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_CONFIG, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_adcConfigSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_ADC_CONFIG, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_calSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                          dev_PwrMon_WriteData_t         data,
                          dev_PwrMon_Callback_t          callback,
                          void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_SHUNT_CAL, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_shuntTempcoSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                  dev_PwrMon_WriteData_t         data,
                                  dev_PwrMon_Callback_t          callback,
                                  void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_SHUNT_TEMPCO, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_alertMaskSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_DIAG_ALRT, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_shuntOverVSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                 dev_PwrMon_WriteData_t         data,
                                 dev_PwrMon_Callback_t          callback,
                                 void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_SOVL, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_shuntUnderVSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                  dev_PwrMon_WriteData_t         data,
                                  dev_PwrMon_Callback_t          callback,
                                  void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_SUVL, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_busOverVSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                               dev_PwrMon_WriteData_t         data,
                               dev_PwrMon_Callback_t          callback,
                               void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_BOVL, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_busUnderVSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_BUVL, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_tempLimitSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_TEMP_LIMIT, data, callback, cbData );
    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_pwrLimitSet( const dev_PwrMon_DeviceInfo_t* devPtr,
                                dev_PwrMon_WriteData_t         data,
                                dev_PwrMon_Callback_t          callback,
                                void*                          cbData )
{
    dev_PwrMon_commonWrite( devPtr, DEV_PWRMON_REG_PWR_LIMIT, data, callback, cbData );
    return;
}


/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_PwrMon_ina228_init( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    // Read out the device and manufacturer Ids
    dev_PwrMon_ina228_manufacturerId( devPtr, &devPtr->ctx->mftrId, NULL, NULL );
    dev_PwrMon_ina228_deviceId( devPtr, &devPtr->ctx->deviceId, NULL, NULL );

    // reset device, disable alert and set each cal reg to 0
    dev_PwrMon_ina228_configSet( devPtr, DEV_PWRMON_REG_CONFIG_RESET, NULL, NULL );
    dev_PwrMon_ina228_alertMaskSet( devPtr, 0x0000, NULL, NULL );
    dev_PwrMon_ina228_calSet( devPtr, 0x0000, NULL, NULL );
    dev_PwrMon_ina228_configSet( devPtr, DEV_PWRMON_REG_CONFIG_BUILD(0,0,0,0,1), NULL, NULL );

    return;
}

/*===========================================================================*/
void
dev_PwrMon_ina228_config( const dev_PwrMon_DeviceInfo_t* devPtr,
                          dev_PwrMon_DevModeMask_t       modeMask,
                          dev_PwrMon_Callback_t          callback,
                          void*                          cbData )
{
    uint16_t tmpReg;
    dev_PwrMon_OpMode_t mode = DEV_PWRMON_OP_MODE_PWR_DWN;

    if( (modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_SHUNT) &&
        (modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_BUS) )
    {
        mode = DEV_PWRMON_OP_MODE_CONT_SHUNT_BUS;
    }
    else if( modeMask & DEV_PWR_MON_DEVICE_MODE_MASK_BUS )
    {
        mode = DEV_PWRMON_OP_MODE_CONT_BUS;
    }
    else
    {
        mode = DEV_PWRMON_OP_MODE_CONT_SHUNT;
    }
    // DEV_PWRMON_CONV_TIME_US_50   = 0x00,
    // DEV_PWRMON_CONV_TIME_US_84   = 0x01,
    // DEV_PWRMON_CONV_TIME_US_150  = 0x02,
    // DEV_PWRMON_CONV_TIME_US_280  = 0x03,
    // DEV_PWRMON_CONV_TIME_US_540  = 0x04,
    // DEV_PWRMON_CONV_TIME_US_1052 = 0x05,
    // DEV_PWRMON_CONV_TIME_US_2074 = 0x06,
    // DEV_PWRMON_CONV_TIME_US_4120 = 0x07

    tmpReg = DEV_PWRMON_REG_ADC_CONFIG_BUILD( mode,
                                              DEV_PWRMON_CONV_TIME_US_84,
                                              DEV_PWRMON_CONV_TIME_US_84,
                                              DEV_PWRMON_CONV_TIME_US_84,
                                              DEV_PWRMON_AVG_MODE_SAMPLES_4 );

    dev_PwrMon_ina228_adcConfigSet( devPtr, tmpReg, callback, cbData );

    return;
}

#define CAL_AVG_VBUS 256
/*===========================================================================*/
int32_t
dev_PwrMon_ina228_vBusCal( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    uint16_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_ADC_CONFIG_BUILD( DEV_PWRMON_OP_MODE_PWR_DWN,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_ina228_adcConfigSet( devPtr, tmpReg, NULL, 0 );

    int32_t vBusSum = 0;
    int32_t vBus;
    for( int i=0; i<CAL_AVG_VBUS; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_ADC_CONFIG_BUILD( DEV_PWRMON_OP_MODE_TRIG_BUS,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_ina228_adcConfigSet( devPtr, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_ina228_alertMaskGet( devPtr, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        dev_PwrMon_ina228_vBusGet( devPtr, (uint8_t*)&vBus, NULL, 0 );
        vBusSum += (vBus >> 10);
    }
    vBus = (vBusSum / CAL_AVG_VBUS);

    return( (int32_t)vBus );
}

#define CAL_AVG_VSHUNT 256
/*===========================================================================*/
int32_t
dev_PwrMon_ina228_vShuntCal( const dev_PwrMon_DeviceInfo_t* devPtr )
{
    dev_PwrMon_Data_t tmpReg;

    // Power off device
    tmpReg = DEV_PWRMON_REG_ADC_CONFIG_BUILD( DEV_PWRMON_OP_MODE_PWR_DWN,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_CONV_TIME_US_280,
                                              DEV_PWRMON_AVG_MODE_SAMPLES_1 );

    dev_PwrMon_ina228_adcConfigSet( devPtr, tmpReg, NULL, 0 );

    int32_t vShuntSum = 0;
    int32_t vShunt;
    for( int i=0; i<CAL_AVG_VSHUNT; i++ )
    {
        // Trigger one-shot longer conversion of Shunt and Bus
        tmpReg = DEV_PWRMON_REG_ADC_CONFIG_BUILD( DEV_PWRMON_OP_MODE_TRIG_SHUNT,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_CONV_TIME_US_280,
                                                  DEV_PWRMON_AVG_MODE_SAMPLES_1 );

        dev_PwrMon_ina228_adcConfigSet( devPtr, tmpReg, NULL, 0 );

        // Wait for device to complete conversions
        tmpReg = 0;
        while( DEV_PWRMON_REG_ALERT_MASK_CVRF( tmpReg ) == 0 )
        {
            dev_PwrMon_ina228_alertMaskGet( devPtr, (uint8_t*)&tmpReg, NULL, 0 );
        }

        // Read out Vshunt and Vbus voltages. They should be 0
        dev_PwrMon_ina228_vShuntGet( devPtr, (uint8_t*)&vShunt, NULL, 0 );
        vShuntSum += (vShunt >> 10);
    }
    vShunt = (vShuntSum / CAL_AVG_VSHUNT);

    return( (int32_t)vShunt );
}

#define SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV(_val) (((_val) * 1953125) / 10000000)
/*===========================================================================*/
int32_t
dev_PwrMon_ina228_vBusConvert( int32_t val )
{
    return( SVC_PWRMON_CHANNEL_BUS_ADC_TO_MV(val) );
}

#define SVC_PWRMON_CHANNEL_SHUNT_ADC_TO_UV(_val) (((_val) * 3125) / 10000)
#define SVC_PWRMON_CHANNEL_SHUNT_HIGH_RES_ADC_TO_UV(_val) (((_val) * 78125) / 1000000)
/*===========================================================================*/
int32_t
dev_PwrMon_ina228_vShuntConvert( int32_t val )
{
    // Todo:Check configured ADC range
    int64_t tmp = SVC_PWRMON_CHANNEL_SHUNT_HIGH_RES_ADC_TO_UV((int64_t)(val));
    return( (int32_t)tmp );
}

/*===========================================================================*/
int32_t
dev_PwrMon_ina228_format( int32_t val )
{
    // Sign extend: If 20th bit is 1 then it's negative
    if( (val & 0x00800000) != 0 )
    {
        val |= 0xFF000000;
    }
    return( (val >> 4) );
}

/*===========================================================================*/
dev_PwrMon_DeviceApi_t dev_PwrMon_ina228_deviceApi =
{
    dev_PwrMon_ina228_init,
    dev_PwrMon_ina228_config,
    {
        dev_PwrMon_ina228_vBusGet,
        dev_PwrMon_ina228_vBusCal,
        dev_PwrMon_ina228_format,
        dev_PwrMon_ina228_vBusConvert
    },
    {
        dev_PwrMon_ina228_vShuntGet,
        dev_PwrMon_ina228_vShuntCal,
        dev_PwrMon_ina228_format,
        dev_PwrMon_ina228_vShuntConvert
    }
};
#endif
