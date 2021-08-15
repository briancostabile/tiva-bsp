/**
 * Copyright 2018 Brian Costabile
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
 * @file dev_Pressure_bmp180.c
 * @brief Contains Pressure driver for the BMP180.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_Pressure.h"


#if defined(BSP_PLATFORM_ENABLE_DEV_PRESSURE_BMP180)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/

// Configuration defines
#define DEV_PRESSURE_I2C_ADDR   ((bsp_I2c_Addr_t)0x77)
#define DEV_PRESSURE_I2C_SPEED  BSP_I2C_SPEED_FAST

#define DEV_PRESSURE_INT_PERSIST DEV_PRESSURE_REG_INT_PERSIST_CYCLES_1
#define DEV_PRESSURE_RANGE       DEV_PRESSURE_REG_RANGE_64000
#define DEV_PRESSURE_RESOLUTION  DEV_PRESSURE_REG_RESOLUTION_16BIT

#define DEV_PRESSURE_THRESHOLD_LOW_DEFAULT    0x0000
#define DEV_PRESSURE_THRESHOLD_HIGH_DEFAULT   0x0001




#define DEV_PRESSURE_REG_CALIB0       ((bsp_Pressure_RegId_t)0xAA)
#define DEV_PRESSURE_REG_RESET        ((bsp_Pressure_RegId_t)0xE0)
#define DEV_PRESSURE_REG_CTRL         ((bsp_Pressure_RegId_t)0xF4)
#define DEV_PRESSURE_REG_MSB          ((bsp_Pressure_RegId_t)0xF6)
#define DEV_PRESSURE_REG_LSB          ((bsp_Pressure_RegId_t)0xF7)
#define DEV_PRESSURE_REG_XLSB         ((bsp_Pressure_RegId_t)0xF8)
typedef uint8_t bsp_Pressure_RegId_t;

typedef uint8_t dev_Pressure_RegValue_t;


typedef struct BSP_ATTR_PACKED dev_Pressure_CalibInfo_s
{
    int16_t  ac1;
    int16_t  ac2;
    int16_t  ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t  b1;
    int16_t  b2;
    int16_t  mb;
    int16_t  mc;
    int16_t  md;
} dev_Pressure_CalibInfo_t;

// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_Pressure_UsrDataCallback_t)( void );


/*=============================================================================
 *                                   Globals
 *===========================================================================*/
// Callback to be called at the end of the measurement transactions. Can be NULL.
dev_Pressure_MeasCallback_t dev_Pressure_measCallback;

// Write buffer is only ever 3 bytes max. 1 byte for the pointer Id and 2 bytes
// for the register value
// Read buffer is only ever 3 bytes max. One-byte for the regId then 2 bytes
// for the read out data
uint8_t dev_Pressure_i2cBuffer[4];

// Global to hold a single i2c transaction.
bsp_I2c_MasterTrans_t dev_Pressure_i2cTrans;

// Global to hold the one-time read calibration information
dev_Pressure_CalibInfo_t dev_Pressure_calibration;

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/

/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_Pressure_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    if( usrData != NULL )
    {
        ((dev_Pressure_UsrDataCallback_t)usrData)();
    }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
static void
dev_Pressure_i2cTransQueue( bsp_I2c_TransType_t type,
                            size_t              len,
                            uint8_t*            buffer,
                            void*               usrData )
{
    dev_Pressure_i2cTrans.type     = type;
    dev_Pressure_i2cTrans.speed    = DEV_PRESSURE_I2C_SPEED;
    dev_Pressure_i2cTrans.addr     = DEV_PRESSURE_I2C_ADDR;
    dev_Pressure_i2cTrans.len      = len;
    dev_Pressure_i2cTrans.buffer   = buffer;
    dev_Pressure_i2cTrans.callback = dev_Pressure_i2cTransCallback;
    dev_Pressure_i2cTrans.usrData  = usrData;

    bsp_I2c_masterTransQueue( BSP_PLATFORM_I2C_ISL29023, &dev_Pressure_i2cTrans );
    return;
}

/*===========================================================================*/
// Wrapper to write to the control/reset registers on the BMP180. This is a simple
// write of a regId byte plus a single data byte for the new register value.
static void
dev_Pressure_i2cRegWrite( bsp_Pressure_RegId_t    regId,
                          dev_Pressure_RegValue_t regValue,
                          void*                   usrData )
{
    dev_Pressure_i2cBuffer[0] = regId;
    dev_Pressure_i2cBuffer[1] = regValue;

    dev_Pressure_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 2, dev_Pressure_i2cBuffer, usrData );
    return;
}

/*===========================================================================*/
// Wrapper to read 16-bit data from the BMP180.
static void
dev_Pressure_i2cRegRead( bsp_Pressure_RegId_t regId,
                         void*                usrData )
{
    dev_Pressure_i2cBuffer[0] = regId;
    dev_Pressure_i2cBuffer[1] = 0;
    dev_Pressure_i2cBuffer[2] = 0;
    dev_Pressure_i2cBuffer[3] = 0;

    dev_Pressure_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE_READ, 3, dev_Pressure_i2cBuffer, usrData );
    return;
}

/*===========================================================================*/
static void
dev_Pressure_updateCalib0( void )
{
    dev_Pressure_calibration.ac1 = ((dev_Pressure_i2cBuffer[1] << 8) | dev_Pressure_i2cBuffer[2]);
    return;
}

/*===========================================================================*/
static void
dev_Pressure_calibrationRead( void )
{
    dev_Pressure_i2cRegRead( DEV_PRESSURE_REG_CALIB0, dev_Pressure_updateCalib0 );
    return;
}

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_Pressure_init( void )
{
    dev_Pressure_measCallback = NULL;

    /* make sure I2C is enabled */
    bsp_I2c_masterControl( BSP_PLATFORM_I2C_ISL29023, BSP_I2C_CONTROL_ENABLE );

    dev_Pressure_calibrationRead();

    return;
}


/*===========================================================================*/
void
dev_Pressure_measTrigger( dev_Pressure_MeasCallback_t callback )
{
    return;
}
#endif
