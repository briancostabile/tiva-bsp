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
#define DEV_TEMP_I2C_ADDR   ((bsp_I2c_Addr_t)0x40)
#define DEV_TEMP_I2C_SPEED  BSP_I2C_SPEED_FAST
#define DEV_TEMP_RESOLUTION BSP_PLATFORM_SHT21_RESOLUTION
#define DEV_TEMP_RESET_TIME_MAX_MS 15

// Commands defined for the device.
// Datasheet SHT21: Table 6
#define DEV_TEMP_CMD_TRIG_T_HOLD  ((dev_Temp_Cmd_t)0xE3)
#define DEV_TEMP_CMD_TRIG_RH_HOLD ((dev_Temp_Cmd_t)0xE5)
#define DEV_TEMP_CMD_TRIG_T       ((dev_Temp_Cmd_t)0xF3)
#define DEV_TEMP_CMD_TRIG_RH      ((dev_Temp_Cmd_t)0xF5)
#define DEV_TEMP_CMD_REG_WRITE    ((dev_Temp_Cmd_t)0xE6)
#define DEV_TEMP_CMD_REG_READ     ((dev_Temp_Cmd_t)0xE7)
#define DEV_TEMP_CMD_REG_RESET    ((dev_Temp_Cmd_t)0xFE)
typedef uint8_t dev_Temp_Cmd_t;

// Resolution options for temperature and humidity.
// Datasheet SHT21: Table
#define DEV_TEMP_RESOLUTION_RH12_T14  ((dev_Temp_Resolution_t)0x00)
#define DEV_TEMP_RESOLUTION_RH8_T12   ((dev_Temp_Resolution_t)0x01)
#define DEV_TEMP_RESOLUTION_RH10_T13  ((dev_Temp_Resolution_t)0x02)
#define DEV_TEMP_RESOLUTION_RH11_T11  ((dev_Temp_Resolution_t)0x03)
typedef uint8_t dev_Temp_Resolution_t;

// Setting the Resolution is tricky. The 2 bits are not next to each other
// in the configuration register.
#define DEV_TEMP_REG_SET_RES(_reg, _res)  (((_reg) & ~0x81) | ((((_res) & 0x02) << 7) | ((_res) & 0x01)))

/******
 * Defines and macros to convert the raw received value for temperature into a 10.6 fixed point value in signed units of degrees Celsius
 */
#define DEV_TEMP_TEMP_OFFSET (float)46.85
#define DEV_TEMP_TEMP_FACTOR (float)175.72
#define DEV_TEMP_TEMP_FRACTION (float)65536
#define DEV_TEMP_CONVERT_TEMP_FLOAT( _val ) ((((float)(_val) * DEV_TEMP_TEMP_FACTOR) / DEV_TEMP_TEMP_FRACTION) - DEV_TEMP_TEMP_OFFSET)

/******
 * Defines and macros to convert the raw received value for Relative Humidity into a 10.6 fixed point percentage
 */
#define DEV_TEMP_HUMID_OFFSET (float)6
#define DEV_TEMP_HUMID_FACTOR (float)125
#define DEV_TEMP_HUMID_FRACTION (float)65536
#define DEV_TEMP_CONVERT_HUMID_FLOAT( _val ) ((((float)(_val) * DEV_TEMP_HUMID_FACTOR) / DEV_TEMP_HUMID_FRACTION) - DEV_TEMP_HUMID_OFFSET)


// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_Temp_UsrDataCallback_t)( void );

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
// Callback to be called at the end of the measurement transactions. Can be NULL.
dev_Temp_MeasCallback_t dev_Temp_measCallback;

// Write buffer is only ever 2 bytes max. 1 byte for command Id and another
// byte for the register value in the register-write command
uint8_t dev_Temp_i2cWriteBuffer[2];

// Read buffer is only ever 3 bytes max. A temp/humidity conversion is 24-bits
uint8_t dev_Temp_i2cReadBuffer[3];

// globals to hold the most recent Measurements
dev_Temp_MeasHumid_t dev_Temp_humid;
dev_Temp_MeasTemp_t  dev_Temp_temp;

// Global to hold a single i2c transaction.
bsp_I2c_MasterTrans_t dev_Temp_i2cTrans;


/*=============================================================================
 *                              Local Functions
 *===========================================================================*/

/*===========================================================================*/
// Inline function to convert floating point to 10.6 fixed point.
inline uint16_t dev_Temp_cnvFloatToFixed10p6(float input)
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
void
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

    bsp_I2c_masterTransQueue( BSP_PLATFORM_I2C_SHT21, &dev_Temp_i2cTrans );

    return;
}


/*===========================================================================*/
// Wrapper to send a command to the SHT21. This is a simple write of a single
// command byte
static void
dev_Temp_i2cSndCmd( dev_Temp_Cmd_t cmd,
                    void*          usrData )
{
    dev_Temp_i2cWriteBuffer[0] = cmd;
    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 1, dev_Temp_i2cWriteBuffer, usrData );
}

/*===========================================================================*/
// Wrapper to write to the config register on the SHT21. This is a simple write of a single
// command byte plus a single data byte for the new register value.
static void
dev_Temp_i2cRegWrite( uint8_t reg,
                      void*   usrData )
{
    dev_Temp_i2cWriteBuffer[0] = DEV_TEMP_CMD_REG_WRITE;
    dev_Temp_i2cWriteBuffer[1] = reg;

    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 2, dev_Temp_i2cWriteBuffer, usrData );
}

/*===========================================================================*/
// Wrapper to read data from the SHT21. The amount of data available depends on the
// command previously written. Measurements are 3 bytes, the config register is a
// single byte
static void
dev_Temp_i2cRead( size_t len,
                  void*  usrData )
{
    dev_Temp_i2cTransQueue( BSP_I2C_TRANS_TYPE_READ, len, dev_Temp_i2cReadBuffer, usrData );
}


/*****************************************************************************
 * Callbacks to trigger and read new humidity and temperature values
 */
/*===========================================================================*/
static void
dev_Temp_i2cTransCbackHumidRead( void )
{
    // Save off the converted value and
    float temp_float = DEV_TEMP_CONVERT_HUMID_FLOAT( ((dev_Temp_i2cReadBuffer[0] << 8) | (dev_Temp_i2cReadBuffer[1] & 0xFC)) );
    dev_Temp_humid = dev_Temp_cnvFloatToFixed10p6( temp_float );

    // Measurements are complete so call the callback
   if( dev_Temp_measCallback != NULL )
   {
       dev_Temp_measCallback( dev_Temp_humid, dev_Temp_temp );
   }
}

/*===========================================================================*/
static void
dev_Temp_i2cTransCbackHumidTrigger( void )
{
    // Read out the Humidity
    dev_Temp_i2cRead( 3, dev_Temp_i2cTransCbackHumidRead );
}

/*===========================================================================*/
static void
dev_Temp_i2cTransCbackTempRead( void )
{
    // Save off the converted value and
    float temp_float = DEV_TEMP_CONVERT_TEMP_FLOAT( ((dev_Temp_i2cReadBuffer[0] << 8) | (dev_Temp_i2cReadBuffer[1] & 0xFC)) );
    dev_Temp_temp = dev_Temp_cnvFloatToFixed10p6( temp_float );

    // Trigger a relative humidity read
    dev_Temp_i2cSndCmd( DEV_TEMP_CMD_TRIG_RH_HOLD, dev_Temp_i2cTransCbackHumidTrigger );
}

/*===========================================================================*/
static void
dev_Temp_i2cTransCbackTempTrigger( void )
{
    // Read out the temperature
    dev_Temp_i2cRead( 3, dev_Temp_i2cTransCbackTempRead );
}

/*===========================================================================*/
// Wrapper function to setup and trigger the sequence of i2c commands
// to trigger the capture of temperature and humidity
static void
dev_Temp_i2cTriggerMeas( dev_Temp_MeasCallback_t callback )
{
    // Trigger a Temperature Read
    dev_Temp_measCallback = callback;
    dev_Temp_i2cSndCmd( DEV_TEMP_CMD_TRIG_T_HOLD, dev_Temp_i2cTransCbackTempTrigger );
}


/*****************************************************************************
 * Callbacks/Functions/Data to Update the configuration register
 */

// Flag to spin on
static volatile bool dev_Temp_i2cTransCompleteRegWrite;

// Configuration settings saved in globals so callbacks can
// reference them
dev_Temp_Resolution_t dev_Temp_resolution;

/*===========================================================================*/
// Last callback in the sequence. Called when the Reg-Write
// command has been successfully sent.
static void
dev_Temp_i2cTransCbackRegWrite( void )
{
    dev_Temp_i2cTransCompleteRegWrite = TRUE;
}

/*===========================================================================*/
// Callback called when the Read of the configuration register
// has completed and the register contents are in the ReadBuffer
static void
dev_Temp_i2cTransCbackRegRead( void )
{
    uint8_t reg = DEV_TEMP_REG_SET_RES( dev_Temp_i2cReadBuffer[0], dev_Temp_resolution );
    dev_Temp_i2cRegWrite( reg, dev_Temp_i2cTransCbackRegWrite );
}

/*===========================================================================*/
// Callback called Reg-Read command has been sent. Presumably,
// the register contents are waiting to be read out.
static void
dev_Temp_i2cTransCbackRegReadCmd( void )
{
    dev_Temp_i2cRead( 1, dev_Temp_i2cTransCbackRegRead );
}

/*===========================================================================*/
// Wrapper function to setup and trigger the sequence of i2C commands
// to be sent to the device. This spins waiting for the operation
// to complete which is fine because this is only called at power up.
// If it's decided to call this during normal operation then the driver
// should be configured with OS calls to use a semaphore for OS signals
static void
dev_Temp_Config( dev_Temp_Resolution_t resolution )
{
    dev_Temp_i2cTransCompleteRegWrite = FALSE;
    dev_Temp_resolution = resolution;

    dev_Temp_i2cSndCmd( DEV_TEMP_CMD_REG_READ, dev_Temp_i2cTransCbackRegReadCmd );

    // spin until complete
    while( dev_Temp_i2cTransCompleteRegWrite == FALSE );
}


/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_Temp_init( dev_Temp_MeasCallback_t callback )
{
    dev_Temp_humid = 0;
    dev_Temp_temp  = 0;

    // make sure I2C is enabled
    bsp_I2c_masterControl( BSP_PLATFORM_I2C_SHT21, BSP_I2C_CONTROL_ENABLE );

    // Reset the device
    dev_Temp_i2cSndCmd( DEV_TEMP_CMD_REG_RESET, NULL );

    // Hard coded delay to wait for reset to complete
    bsp_Clk_delayMs( DEV_TEMP_RESET_TIME_MAX_MS );

    // Configure the Humidity/Temperature sensor
    // This will start a chain of transactions/Interrupts to:
    //     read control Reg
    //     write control Reg with updated settings
    dev_Temp_Config( DEV_TEMP_RESOLUTION );

    // Trigger the initial measurement
    // This will start a chain of transactions/Interrupts to:
    //     trigger Temperature measurement
    //     read-Temperature
    //     trigger Relative Humidity measurement
    //     read Relative Humidity
    dev_Temp_i2cTriggerMeas( callback );

    return;
}


/*===========================================================================*/
void
dev_Temp_measTrigger( dev_Temp_MeasCallback_t callback )
{
    dev_Temp_i2cTriggerMeas( callback );
    return;
}


/*===========================================================================*/
void
dev_Temp_measRead( dev_Temp_MeasTemp_t*   tempPtr,
                   dev_Temp_MeasHumid_t* humidPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    *tempPtr = dev_Temp_temp;
    *humidPtr = dev_Temp_humid;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}

