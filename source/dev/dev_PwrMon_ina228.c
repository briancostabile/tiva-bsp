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

#define DEV_PWRMON_I2C_ID0     0
#define DEV_PWRMON_I2C_ADDR0   ((bsp_I2c_Addr_t)0x40)
#define DEV_PWRMON_I2C_SPEED0  BSP_I2C_SPEED_FAST
#define DEV_PWRMON_I2C_ID1     1
#define DEV_PWRMON_I2C_ADDR1   ((bsp_I2c_Addr_t)0x40)
#define DEV_PWRMON_I2C_SPEED1  BSP_I2C_SPEED_FAST

#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/

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
#define DEV_PWRMON_TEMP_LIMIT       ((dev_PwrMon_I2cCmd_t)0x10)
#define DEV_PWRMON_PWR_LIMIT        ((dev_PwrMon_I2cCmd_t)0x11)
#define DEV_PWRMON_MFGR_ID          ((dev_PwrMon_I2cCmd_t)0x3E)
#define DEV_PWRMON_DEV_ID           ((dev_PwrMon_I2cCmd_t)0x3F)
typedef uint8_t dev_PwrMon_I2cCmd_t;

#define DEV_PWRMON_REG_TYPE_RO true
#define DEV_PWRMON_REG_TYPE_RW false
typedef uint8_t dev_PwrMon_RegType_t;


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
    bsp_I2c_MasterTrans_t     i2cTrans;
    uint8_t                   wLen;
    uint8_t                   wBuffer[3];
    uint8_t                   rLen;
    void*                     rPtr;
    dev_PwrMon_Callback_t callback;
    dev_PwrMon_Reg_t          prevRegId;
    dev_PwrMon_DeviceId_t     deviceId;
    dev_PwrMon_ManufacturerId_t mftrId;
} dev_PwrMon_DeviceCtx_t;

typedef struct dev_PwrMon_DeviceInfo_s
{
    bsp_I2c_Id_t            i2cId;
    bsp_I2c_Addr_t          i2cAddr;
    bsp_I2c_Speed_t         i2cSpeed;
    dev_PwrMon_DeviceCtx_t* ctx;
} dev_PwrMon_DeviceInfo_t;


// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_PwrMon_UsrDataCallback_t)( void );

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
    { DEV_PWRMON_TEMP_LIMIT,       2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_PWR_LIMIT,        2, DEV_PWRMON_REG_TYPE_RW },
    { DEV_PWRMON_MFGR_ID,          2, DEV_PWRMON_REG_TYPE_RO },
    { DEV_PWRMON_DEV_ID,           2, DEV_PWRMON_REG_TYPE_RO },
};

dev_PwrMon_DeviceCtx_t dev_PwrMon_deviceCtx[BSP_PLATFORM_PWRMON_NUM_DEVICES];

const dev_PwrMon_DeviceInfo_t dev_PwrMon_deviceInfo[] =
{
    { DEV_PWRMON_I2C_ID0, DEV_PWRMON_I2C_ADDR0, DEV_PWRMON_I2C_SPEED0, &dev_PwrMon_deviceCtx[0] },
    { DEV_PWRMON_I2C_ID1, DEV_PWRMON_I2C_ADDR1, DEV_PWRMON_I2C_SPEED1, &dev_PwrMon_deviceCtx[1] }
};


/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
static inline const dev_PwrMon_RegInfo_t*
dev_PwrMon_getRegInfo( dev_PwrMon_I2cCmd_t regId )
{
    if( regId <= DEV_PWRMON_PWR_LIMIT )
    {
        return( &dev_PwrMon_regInfo[regId] );
    }
    else
    {
        volatile uint32_t idx = (DEV_PWRMON_PWR_LIMIT + 1) + (regId - DEV_PWRMON_MFGR_ID);
        return(&dev_PwrMon_regInfo[idx]);
    }
}

/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_PwrMon_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    dev_PwrMon_DeviceCtx_t* ctx = (dev_PwrMon_DeviceCtx_t*)usrData;
    if( ctx->callback != NULL ) { ctx->callback(); }
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
    devPtr->ctx->wBuffer[1] = ((regValue >> 0) & 0xFF);
    devPtr->ctx->wBuffer[2] = ((regValue >> 8) & 0xFF);

    devPtr->ctx->i2cTrans.type    = BSP_I2C_TRANS_TYPE_WRITE;
    devPtr->ctx->i2cTrans.wLen    = 3; // All writable registers are 2 bytes plus 1 byte address
    devPtr->ctx->i2cTrans.wBuffer = devPtr->ctx->wBuffer;
    devPtr->ctx->i2cTrans.rLen    = 0;
    devPtr->ctx->i2cTrans.rBuffer = NULL;
    dev_PwrMon_i2cTransQueue( devPtr );
}

/*===========================================================================*/
// Wrapper to read data from the INA228.
static void
dev_PwrMon_i2cRegRead( const dev_PwrMon_DeviceInfo_t* devPtr,
                       dev_PwrMon_I2cCmd_t            regId )
{
    const dev_PwrMon_RegInfo_t* regInfoPtr = dev_PwrMon_getRegInfo( regId );

    devPtr->ctx->i2cTrans.rLen    = regInfoPtr->len;
    devPtr->ctx->i2cTrans.rBuffer = devPtr->ctx->rPtr;
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
}


/*===========================================================================*/
static void
dev_PwrMon_manufacturerId( dev_PwrMon_DevId_t           devId,
                           dev_PwrMon_ManufacturerId_t* dataPtr,
                           dev_PwrMon_Callback_t        callback,
                           void*                        cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback = callback;
    devPtr->ctx->rLen     = sizeof(dev_PwrMon_ManufacturerId_t);
    devPtr->ctx->rPtr     = dataPtr;
    dev_PwrMon_i2cRegRead( devPtr, DEV_PWRMON_DEV_ID );
    return;
}

/*===========================================================================*/
static void
dev_PwrMon_deviceId( dev_PwrMon_DevId_t     devId,
                     dev_PwrMon_DeviceId_t* dataPtr,
                     dev_PwrMon_Callback_t  callback,
                     void*                  cbData )
{
    const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
    devPtr->ctx->callback = callback;
    devPtr->ctx->rLen     = sizeof(dev_PwrMon_DeviceId_t);
    devPtr->ctx->rPtr     = dataPtr;
    dev_PwrMon_i2cRegRead( devPtr, DEV_PWRMON_MFGR_ID );
    return;
}

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_PwrMon_init( void )
{
    for( int i=0; i < BSP_PLATFORM_PWRMON_NUM_DEVICES; i++ )
    {
        dev_PwrMon_deviceCtx[i].prevRegId = 0xFF;
        dev_PwrMon_deviceCtx[i].callback  = NULL;
        bsp_I2c_masterControl( dev_PwrMon_deviceInfo[i].i2cId, BSP_I2C_CONTROL_ENABLE );

        dev_PwrMon_manufacturerId( i, &dev_PwrMon_deviceCtx[i].deviceId, NULL, NULL );
        dev_PwrMon_deviceId( i, &dev_PwrMon_deviceCtx[i].mfgrId, NULL, NULL );
    }
    // Configure all devices

    return;
}



// /*===========================================================================*/
// void
// dev_PwrMon_sample( dev_PwrMon_DevId_t        devId,
//                    dev_PwrMon_Sample_t*      dataPtr,
//                    dev_PwrMon_Callback_t callback )
// {
//     const dev_PwrMon_DeviceInfo_t* devPtr = &dev_PwrMon_deviceInfo[devId];
//     devPtr->ctx->callback = callback;
//     devPtr->ctx->rLen     = sizeof(dev_PwrMon_Sample_t);
//     devPtr->ctx->rPtr     = dataPtr;
//     dev_PwrMon_i2cRegRead( devPtr, DEV_PWRMON_REG_CURRENT );
//     return;
// }

#endif
