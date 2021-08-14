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
 * @file dev_Humid_sht21.c
 * @brief Contains Macros and defines for the HW reset reason.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_Humid.h"

#if defined(BSP_PLATFORM_ENABLE_DEV_HUMID_SHT21)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/

// Configuration defines
#define DEV_HUMID_I2C_ADDR   ((bsp_I2c_Addr_t)0x40)
#define DEV_HUMID_I2C_SPEED  BSP_I2C_SPEED_FAST
#define DEV_HUMID_RESOLUTION BSP_PLATFORM_SHT21_RESOLUTION
#define DEV_HUMID_RESET_TIME_MAX_MS 15

// Commands defined for the device.
// Datasheet SHT21: Table 6
#define DEV_HUMID_CMD_TRIG_T_HOLD  ((dev_Humid_Cmd_t)0xE3)
#define DEV_HUMID_CMD_TRIG_RH_HOLD ((dev_Humid_Cmd_t)0xE5)
#define DEV_HUMID_CMD_TRIG_T       ((dev_Humid_Cmd_t)0xF3)
#define DEV_HUMID_CMD_TRIG_RH      ((dev_Humid_Cmd_t)0xF5)
#define DEV_HUMID_CMD_REG_WRITE    ((dev_Humid_Cmd_t)0xE6)
#define DEV_HUMID_CMD_REG_READ     ((dev_Humid_Cmd_t)0xE7)
#define DEV_HUMID_CMD_REG_RESET    ((dev_Humid_Cmd_t)0xFE)
typedef uint8_t dev_Humid_Cmd_t;

// Resolution options for temperature and humidity.
// Datasheet SHT21: Table
#define DEV_HUMID_RESOLUTION_RH12_T14  ((dev_Humid_Resolution_t)0x00)
#define DEV_HUMID_RESOLUTION_RH8_T12   ((dev_Humid_Resolution_t)0x01)
#define DEV_HUMID_RESOLUTION_RH10_T13  ((dev_Humid_Resolution_t)0x02)
#define DEV_HUMID_RESOLUTION_RH11_T11  ((dev_Humid_Resolution_t)0x03)
typedef uint8_t dev_Humid_Resolution_t;

// Setting the Resolution is tricky. The 2 bits are not next to each other
// in the configuration register.
#define DEV_HUMID_REG_SET_RES(_reg, _res)  (((_reg) & ~0x81) | ((((_res) & 0x02) << 7) | ((_res) & 0x01)))

/******
 * Defines and macros to convert the raw received value for temperature into a 10.6 fixed point value in signed units of degrees Celsius
 */
#define DEV_HUMID_TEMP_OFFSET (float)46.85
#define DEV_HUMID_TEMP_FACTOR (float)175.72
#define DEV_HUMID_TEMP_FRACTION (float)65536
#define DEV_HUMID_CONVERT_TEMP_FLOAT( _val ) ((((float)(_val) * DEV_HUMID_TEMP_FACTOR) / DEV_HUMID_TEMP_FRACTION) - DEV_HUMID_TEMP_OFFSET)

/******
 * Defines and macros to convert the raw received value for Relative Humidity into a 10.6 fixed point percentage
 */
#define DEV_HUMID_HUMID_OFFSET (float)6
#define DEV_HUMID_HUMID_FACTOR (float)125
#define DEV_HUMID_HUMID_FRACTION (float)65536
#define DEV_HUMID_CONVERT_HUMID_FLOAT( _val ) ((((float)(_val) * DEV_HUMID_HUMID_FACTOR) / DEV_HUMID_HUMID_FRACTION) - DEV_HUMID_HUMID_OFFSET)


// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_Humid_UsrDataCallback_t)( void );

/*=============================================================================
 *                                   Globals
 *===========================================================================*/

// Buffer for reading and writing. The read operation needs 1-byte for the write
// portion of the command. and then 3 additional bytes for the read portion 2-bytes for
// the temp/humidity value plus one byte for CRC.
uint8_t dev_Humid_i2cBuffer[4];

// globals to hold the most recent Measurements
dev_Humid_MeasHumidity_t    dev_Humid_humidity;
dev_Humid_MeasTemperature_t dev_Humid_temperature;

// Global to hold a single i2c transaction.
bsp_I2c_MasterTrans_t dev_Humid_i2cTrans;


/*=============================================================================
 *                              Local Functions
 *===========================================================================*/

/*===========================================================================*/
// Inline function to convert floating point to 10.6 fixed point.
inline uint16_t
dev_Humid_cnvFloatToFixed10p6( float input )
{
    return (uint16_t)(input * (1 << 6));
}

/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_Humid_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    if( usrData != NULL )
    {
        ((dev_Humid_UsrDataCallback_t)usrData)();
    }

    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
void
dev_Humid_i2cTransQueue( bsp_I2c_TransType_t type,
                         size_t              len,
                         uint8_t*            buffer,
                         void*               usrData )
{
    dev_Humid_i2cTrans.type     = type;
    dev_Humid_i2cTrans.speed    = DEV_HUMID_I2C_SPEED;
    dev_Humid_i2cTrans.addr     = DEV_HUMID_I2C_ADDR;
    dev_Humid_i2cTrans.len      = len;
    dev_Humid_i2cTrans.buffer   = buffer;
    dev_Humid_i2cTrans.callback = dev_Humid_i2cTransCallback;
    dev_Humid_i2cTrans.usrData  = usrData;

    bsp_I2c_masterTransQueue( BSP_PLATFORM_I2C_SHT21, &dev_Humid_i2cTrans );

    return;
}


/*===========================================================================*/
// Wrapper to send a command to the SHT21. This is a simple write of a single
// command byte
static void
dev_Humid_i2cSndCmd( dev_Humid_Cmd_t cmd,
                     void*           usrData )
{
    dev_Humid_i2cBuffer[0] = cmd;
    dev_Humid_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 1, dev_Humid_i2cBuffer, usrData );
}

/*===========================================================================*/
// Wrapper to write to the config register on the SHT21. This is a simple write of a single
// command byte plus a single data byte for the new register value.
static void
dev_Humid_i2cRegWrite( uint8_t reg,
                       void*   usrData )
{
    dev_Humid_i2cBuffer[0] = DEV_HUMID_CMD_REG_WRITE;
    dev_Humid_i2cBuffer[1] = reg;

    dev_Humid_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE, 2, dev_Humid_i2cBuffer, usrData );
}

/*===========================================================================*/
// Wrapper to read 3 bytes of data from the SHT21. Used for measurement reads
static void
dev_Humid_i2cRegRead( dev_Humid_Cmd_t cmdId,
                      void*           usrData )
{
    dev_Humid_i2cBuffer[0] = cmdId;
    dev_Humid_i2cBuffer[1] = 0;
    dev_Humid_i2cBuffer[2] = 0;
    dev_Humid_i2cBuffer[3] = 0;

    dev_Humid_i2cTransQueue( BSP_I2C_TRANS_TYPE_WRITE_READ, 4, dev_Humid_i2cBuffer, usrData );
}


/*****************************************************************************
 * Callbacks to trigger/read the humidity
 */
// Humidity callback stored per Humidity trigger/read transaction
volatile dev_Humid_MeasCallbackHumidity_t dev_Humid_measCallbackHumidity;

/*===========================================================================*/
static void
dev_Humid_i2cTransCbackHumidRead( void )
{
    // Save off the converted value and
    float temp_float = DEV_HUMID_CONVERT_HUMID_FLOAT( ((dev_Humid_i2cBuffer[1] << 8) | (dev_Humid_i2cBuffer[2] & 0xFC)) );
    dev_Humid_humidity = dev_Humid_cnvFloatToFixed10p6( temp_float );

    // Measurements are complete so call the callback
    if( dev_Humid_measCallbackHumidity != NULL )
    {
        dev_Humid_measCallbackHumidity( dev_Humid_humidity );
    }
}

/*===========================================================================*/
// Wrapper function to setup and trigger the sequence of i2c commands
// to trigger the capture of temperature and humidity
static void
dev_Humid_i2cTriggerMeasHumidity( dev_Humid_MeasCallbackHumidity_t callback )
{
    dev_Humid_measCallbackHumidity = callback;
    dev_Humid_i2cRegRead( DEV_HUMID_CMD_TRIG_RH_HOLD, dev_Humid_i2cTransCbackHumidRead );
}


/*****************************************************************************
 * Callbacks/Functions/Data to Trigger/read the temperature
 */
volatile dev_Humid_MeasCallbackTemperature_t dev_Humid_measCallbackTemperature;

/*===========================================================================*/
static void
dev_Humid_i2cTransCbackTempRead( void )
{
    // Save off the converted value and
    float temp_float = DEV_HUMID_CONVERT_TEMP_FLOAT( ((dev_Humid_i2cBuffer[1] << 8) | (dev_Humid_i2cBuffer[2] & 0xFC)) );
    dev_Humid_temperature = dev_Humid_cnvFloatToFixed10p6( temp_float );

    // Measurements are complete so call the callback
   if( dev_Humid_measCallbackTemperature != NULL )
   {
       dev_Humid_measCallbackTemperature( dev_Humid_temperature );
   }
}

/*===========================================================================*/
// Wrapper function to setup and trigger the sequence of i2c commands
// to trigger the capture of temperature and humidity
static void
dev_Humid_i2cTriggerMeasTemperature( dev_Humid_MeasCallbackTemperature_t callback )
{
    dev_Humid_measCallbackTemperature = callback;
    dev_Humid_i2cRegRead( DEV_HUMID_CMD_TRIG_T_HOLD, dev_Humid_i2cTransCbackTempRead );
}


/*****************************************************************************
 * Callbacks/Functions/Data to tie the temperature and humidity triggers/reads
 * into a combo read of both values
 */
// Callback to be called at the end of the measurement transactions. Can be NULL.
volatile dev_Humid_MeasCallback_t dev_Humid_measCallback;

/*===========================================================================*/
// Wrapper function to tie the temperature read after a humidity read
static void
dev_Humid_ComboCbackTemperature( dev_Humid_MeasTemperature_t temperature )
{
    // Measurements are complete so call the combo callback
   if( dev_Humid_measCallback != NULL )
   {
       dev_Humid_measCallback( dev_Humid_humidity, dev_Humid_temperature );
   }
}

/*===========================================================================*/
// Wrapper function to tie the temperature read after a humidity read
static void
dev_Humid_ComboCbackHumidity( dev_Humid_MeasHumidity_t humidity )
{
    dev_Humid_i2cTriggerMeasTemperature( dev_Humid_ComboCbackTemperature );
}


/*****************************************************************************
 * Callbacks/Functions/Data to Update the configuration register
 */

// Flag to spin on
static volatile bool dev_Humid_i2cTransCompleteRegWrite;

// Configuration settings saved in globals so callbacks can
// reference them
dev_Humid_Resolution_t dev_Humid_resolution;

/*===========================================================================*/
// Last callback in the sequence. Called when the Reg-Write
// command has been successfully sent.
static void
dev_Humid_i2cTransCbackRegWrite( void )
{
    dev_Humid_i2cTransCompleteRegWrite = TRUE;
}

/*===========================================================================*/
// Callback called when the Read of the configuration register
// has completed and the register contents are in the ReadBuffer
static void
dev_Humid_i2cTransCbackRegRead( void )
{
    uint8_t reg = DEV_HUMID_REG_SET_RES( dev_Humid_i2cBuffer[0], dev_Humid_resolution );
    dev_Humid_i2cRegWrite( reg, dev_Humid_i2cTransCbackRegWrite );
}

/*===========================================================================*/
// Wrapper function to setup and trigger the sequence of i2C commands
// to be sent to the device. This spins waiting for the operation
// to complete which is fine because this is only called at power up.
// If it's decided to call this during normal operation then the driver
// should be configured with OS calls to use a semaphore for OS signals
static void
dev_Humid_Config( dev_Humid_Resolution_t resolution )
{
    dev_Humid_i2cTransCompleteRegWrite = FALSE;
    dev_Humid_resolution = resolution;

    dev_Humid_i2cRegRead( DEV_HUMID_CMD_REG_READ, dev_Humid_i2cTransCbackRegRead );

    // spin until complete
    while( dev_Humid_i2cTransCompleteRegWrite == FALSE );
}


/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_Humid_init( void )
{
    dev_Humid_humidity    = 0;
    dev_Humid_temperature = 0;

    // make sure I2C is enabled
    bsp_I2c_masterControl( BSP_PLATFORM_I2C_SHT21, BSP_I2C_CONTROL_ENABLE );

    // Reset the Sensor
    dev_Humid_i2cSndCmd( DEV_HUMID_CMD_REG_RESET, NULL );

    // Hard coded delay to wait for reset to complete
    bsp_Clk_delayMs( DEV_HUMID_RESET_TIME_MAX_MS );

    // Configure the Humidity/Temperature sensor
    // This will start a chain of transactions/Interrupts to:
    //     read control Reg
    //     write control Reg with updated settings
    dev_Humid_Config( DEV_HUMID_RESOLUTION );

    return;
}


/*===========================================================================*/
void
dev_Humid_measTrigger( dev_Humid_MeasCallback_t callback )
{
    dev_Humid_measCallback = callback;

    // This will start a chain of transactions/Interrupts to:
    //     trigger Humidity measurement
    //     read-Humidity
    //     trigger Temperature measurement
    //     read-Temperature
    dev_Humid_i2cTriggerMeasHumidity( dev_Humid_ComboCbackHumidity );
    return;
}


/*===========================================================================*/
void
dev_Humid_measTriggerHumidity( dev_Humid_MeasCallbackHumidity_t callback )
{
    // This will start a chain of transactions/Interrupts to:
    //     trigger Humidity measurement
    //     read-Humidity
    dev_Humid_i2cTriggerMeasHumidity( callback );
    return;
}


/*===========================================================================*/
void
dev_Humid_measReadHumidity( dev_Humid_MeasHumidity_t* measPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    *measPtr = dev_Humid_humidity;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}


/*===========================================================================*/
void
dev_Humid_measTriggerTemperature( dev_Humid_MeasCallbackTemperature_t callback )
{
    // This will start a chain of transactions/Interrupts to:
    //     trigger Temperature measurement
    //     read-Temperature
    dev_Humid_i2cTriggerMeasTemperature( callback );
    return;
}


/*===========================================================================*/
void
dev_Humid_measReadTemperature( dev_Humid_MeasTemperature_t* measPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    *measPtr = dev_Humid_temperature;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}
#endif