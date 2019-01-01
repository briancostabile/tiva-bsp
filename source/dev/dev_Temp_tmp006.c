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
 * @file dev_Temp_.c
 * @brief Contains Macros and defines for the HW reset reason.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_Temp.h"


/*=============================================================================
 *                                   Defines
 *===========================================================================*/

// Configuration defines
#define DEV_TEMP_I2C_ADDR   ((bsp_I2c_Addr_t)0x41)
#define DEV_TEMP_I2C_SPEED  BSP_I2C_SPEED_FAST

#define DEV_TEMP_REG_VOLTAGE     ((bsp_Temp_I2cCmd_t)0x00)
#define DEV_TEMP_REG_TEMPERATURE ((bsp_Temp_I2cCmd_t)0x01)
#define DEV_TEMP_REG_CONFIG      ((bsp_Temp_I2cCmd_t)0x02)
#define DEV_TEMP_REG_MFG_ID      ((bsp_Temp_I2cCmd_t)0xFE)
#define DEV_TEMP_REG_DEV_ID      ((bsp_Temp_I2cCmd_t)0xFF)
typedef uint8_t bsp_Temp_I2cCmd_t;

#define DEV_TEMP_REG_NOT_RESET 0
#define DEV_TEMP_REG_RESET     1

#define DEV_TEMP_REG_MODE_POWER_DOWN 0x00
#define DEV_TEMP_REG_MODE_POWER_ON   0x07

// Conversion rate num samples averaged
#define DEV_TEMP_REG_MODE_CONV_RATE_1  0x00
#define DEV_TEMP_REG_MODE_CONV_RATE_2  0x01
#define DEV_TEMP_REG_MODE_CONV_RATE_4  0x02
#define DEV_TEMP_REG_MODE_CONV_RATE_8  0x03
#define DEV_TEMP_REG_MODE_CONV_RATE_16 0x04

// Enable the data ready pin
#define DEV_TEMP_REG_DATA_READY_DISABLE 0x00
#define DEV_TEMP_REG_DATA_READY_ENABLE  0x01

#define DEV_TEMP_REG_CONFIG_BUILD( _rst, _mode, _cnv, _int ) \
(                              \
    (((_rst)  & 0x01) << 15) | \
    (((_mode) & 0x07) << 12) | \
    (((_cnv)  & 0x07) <<  9) | \
    (((_int)  & 0x01) <<  8)   \
)

typedef uint16_t dev_Temp_Reg_t;

// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_Temp_UsrDataCallback_t)( void );

// The Multiplication factor
#define DEV_TEMP_MULT_FACTOR 0.03125
#define DEV_TEMP_CONVERT_TEMP_FLOAT( _val ) ((float)(_val) * DEV_TEMP_MULT_FACTOR)

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
// Callback to be called at the end of the measurement transactions. Can be NULL.
dev_Temp_MeasCallback_t dev_Temp_measCallback;

// I2C buffer is only ever 3 bytes max. On writes: 1 byte for the pointer Id and 2 bytes
// for the register value. On reads: 2-bytes for temp and 1 byte for CRC
uint8_t dev_Temp_i2cBuffer[3];

// globals to hold the most recent Measurements
dev_Temp_MeasTemperature_t dev_Temp_temperature;

// Global to hold a single i2c transaction.
bsp_I2c_MasterTrans_t dev_Temp_i2cTrans;


/*=============================================================================
 *                              Local Functions
 *===========================================================================*/

/*===========================================================================*/
// Inline function to convert floating point to 10.6 fixed point.
inline uint16_t
dev_Temp_cnvFloatToFixed10p6( float input )
{
    return (uint16_t)(input * (1 << 6));
}


/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_Temp_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    if( usrData != NULL )
    {
        ((dev_Temp_UsrDataCallback_t)usrData)();
    }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
static void
dev_Temp_i2cTransQueue( bsp_I2c_TransType_t type,
                        size_t              len,
                        uint8_t*            buffer,
                        void*               usrData )
{
    dev_Temp_i2cTrans.type     = type;
    dev_Temp_i2cTrans.speed    = DEV_TEMP_I2C_SPEED;
    dev_Temp_i2cTrans.addr     = DEV_TEMP_I2C_ADDR;
    dev_Temp_i2cTrans.len      = len;
    dev_Temp_i2cTrans.buffer   = buffer;
    dev_Temp_i2cTrans.callback = dev_Temp_i2cTransCallback;
    dev_Temp_i2cTrans.usrData  = usrData;

    bsp_I2c_masterTransQueue( BSP_PLATFORM_I2C_TMP006, &dev_Temp_i2cTrans );

    return;
}

/*===========================================================================*/
// Wrapper to write to the config register on the SHT21. This is a simple write of a single
// command byte plus a single data byte for the new register value.
static void
dev_Temp_i2cRegWrite( bsp_Temp_I2cCmd_t pointerId,
                      dev_Temp_Reg_t    regValue,
                      void*             usrData )
{
    dev_Temp_i2cBuffer[0] = pointerId;
    dev_Temp_i2cBuffer[1] = ((regValue >> 8) & 0xFF);
    dev_Temp_i2cBuffer[2] = ((regValue >> 0) & 0xFF);

    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 3, dev_Temp_i2cBuffer, usrData );
    return;
}

/*===========================================================================*/
static void
dev_Temp_regSelect( bsp_Temp_I2cCmd_t regId )
{
    // After reset is triggered, setup the pointer to be the Temperature register
    dev_Temp_i2cBuffer[0] = regId;
    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 1, dev_Temp_i2cBuffer, NULL );
    return;
}

/*===========================================================================*/
static void
dev_Temp_powerOn( void )
{
    dev_Temp_Reg_t reg = DEV_TEMP_REG_CONFIG_BUILD( DEV_TEMP_REG_NOT_RESET,
                                                    DEV_TEMP_REG_MODE_POWER_ON,
                                                    DEV_TEMP_REG_MODE_CONV_RATE_2,
                                                    DEV_TEMP_REG_DATA_READY_ENABLE );

    dev_Temp_i2cRegWrite( DEV_TEMP_REG_CONFIG, reg, NULL );
    return;
}

/*===========================================================================*/
static void
dev_Temp_reset( void )
{
    dev_Temp_Reg_t reg = DEV_TEMP_REG_CONFIG_BUILD( DEV_TEMP_REG_RESET,
                                                    DEV_TEMP_REG_MODE_POWER_DOWN,
                                                    DEV_TEMP_REG_MODE_CONV_RATE_2,
                                                    DEV_TEMP_REG_DATA_READY_DISABLE );

    dev_Temp_i2cRegWrite( DEV_TEMP_REG_CONFIG, reg, NULL );
    return;
}

/*===========================================================================*/
static void
dev_Temp_readCompleteHandler( void )
{
    float temp_float = DEV_TEMP_CONVERT_TEMP_FLOAT( ((dev_Temp_i2cBuffer[0] << 6) | (dev_Temp_i2cBuffer[1] >> 2)) );
    dev_Temp_temperature = dev_Temp_cnvFloatToFixed10p6( temp_float );

    if( dev_Temp_measCallback != NULL )
    {
        dev_Temp_measCallback( dev_Temp_temperature );
    }
    return;
}

/*===========================================================================*/
static void
dev_Temp_dataReadyHandler( bsp_Gpio_PortId_t    portId,
                           bsp_Gpio_PinOffset_t pinOffset )
{
    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_READ,
                            sizeof(dev_Temp_Reg_t),
                            dev_Temp_i2cBuffer,
                            dev_Temp_readCompleteHandler );
    return;
}

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_Temp_init( void )
{
    /* Disable the GPIO interrupt while configuring */
    bsp_Gpio_intControl( BSP_GPIO_PORT_ID_INT_TEMP,
                         BSP_GPIO_BIT_MASK_INT_TEMP,
                         BSP_GPIO_INT_CONTROL_DISABLE );

    /* Disable any alternative function */
    bsp_Gpio_configAltFunction( BSP_GPIO_PORT_ID_INT_TEMP,
                                BSP_GPIO_BIT_MASK_INT_TEMP,
                                false, 0 );

    /* Configure as input */
    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_INT_TEMP,
                          BSP_GPIO_BIT_MASK_INT_TEMP,
                          FALSE,
                          BSP_GPIO_PULL_NONE );

    bsp_Gpio_intConfig( BSP_GPIO_PORT_ID_INT_TEMP,
                        BSP_GPIO_BIT_MASK_INT_TEMP,
                        FALSE,
                        FALSE,
                        BSP_GPIO_INT_TYPE_EDGE_FALLING,
                        dev_Temp_dataReadyHandler );

    /* Enable the GPIO interrupt after done configuring */
    bsp_Gpio_intControl( BSP_GPIO_PORT_ID_INT_TEMP,
                         BSP_GPIO_BIT_MASK_INT_TEMP,
                         BSP_GPIO_INT_CONTROL_ENABLE );

    /* make sure I2C is enabled */
    bsp_I2c_masterControl( BSP_PLATFORM_I2C_TMP006, BSP_I2C_CONTROL_ENABLE );

    dev_Temp_reset();

    // Need delay after reset before communication will work
    bsp_Clk_delayMs( 100 );

    // Enable the sensor
    dev_Temp_powerOn();
	
	// Wait for power on command to complete before sending the register selection
    bsp_Clk_delayMs( 1 );
	
	// This will forever set the read address to be the temperature reading. This
	// allows less i2c traffic during reads.
    dev_Temp_regSelect( DEV_TEMP_REG_TEMPERATURE );

    return;
}


/*===========================================================================*/
void
dev_Temp_measTrigger( dev_Temp_MeasCallback_t callback )
{
    // Callback is called through interrupt line
    BSP_MCU_CRITICAL_SECTION_ENTER();
    dev_Temp_measCallback = callback;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}


/*===========================================================================*/
void
dev_Temp_measRead( dev_Temp_MeasTemperature_t* tempPtr )
{
    *tempPtr = dev_Temp_temperature;
    return;
}

