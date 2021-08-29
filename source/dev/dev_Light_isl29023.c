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
 * @file dev_Light_isl20023.c
 * @brief Contains Ambient light driver for the isl29023.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_Light.h"


#if defined(BSP_PLATFORM_ENABLE_DEV_LIGHT_ISL20023)
/*=============================================================================
 *                                   Defines
 *===========================================================================*/

// Configuration defines
#define DEV_LIGHT_I2C_ADDR   ((bsp_I2c_Addr_t)0x44)
#define DEV_LIGHT_I2C_SPEED  BSP_I2C_SPEED_FAST

#define DEV_LIGHT_INT_PERSIST DEV_LIGHT_REG_INT_PERSIST_CYCLES_1
#define DEV_LIGHT_RANGE       DEV_LIGHT_REG_RANGE_64000
#define DEV_LIGHT_RESOLUTION  DEV_LIGHT_REG_RESOLUTION_16BIT

#define DEV_LIGHT_THRESHOLD_LOW_DEFAULT    0x0000
#define DEV_LIGHT_THRESHOLD_HIGH_DEFAULT   0x0001


#define DEV_LIGHT_REG_CMD         ((bsp_Light_I2cCmd_t)0x00)
#define DEV_LIGHT_REG_CMD1        ((bsp_Light_I2cCmd_t)0x00)
#define DEV_LIGHT_REG_CMD2        ((bsp_Light_I2cCmd_t)0x01)
#define DEV_LIGHT_REG_DATA        ((bsp_Light_I2cCmd_t)0x02)
#define DEV_LIGHT_REG_DATA_LSB    ((bsp_Light_I2cCmd_t)0x02)
#define DEV_LIGHT_REG_DATA_MSB    ((bsp_Light_I2cCmd_t)0x03)
#define DEV_LIGHT_REG_INT_LT      ((bsp_Light_I2cCmd_t)0x04)
#define DEV_LIGHT_REG_INT_LT_LSB  ((bsp_Light_I2cCmd_t)0x04)
#define DEV_LIGHT_REG_INT_LT_MSB  ((bsp_Light_I2cCmd_t)0x05)
#define DEV_LIGHT_REG_INT_HT      ((bsp_Light_I2cCmd_t)0x06)
#define DEV_LIGHT_REG_INT_HT_LSB  ((bsp_Light_I2cCmd_t)0x06)
#define DEV_LIGHT_REG_INT_HT_MSB  ((bsp_Light_I2cCmd_t)0x07)
#define DEV_LIGHT_REG_TEST        ((bsp_Light_I2cCmd_t)0x08)
typedef uint8_t bsp_Light_I2cCmd_t;

// Interrupt Persist Bits:
// From isl29023 Documentation:
// " The interrupt persist bits provides control over when interrupts
//   occur. There are four different selections for this feature. A value
//   of n (where n is 1, 4, 8, and 16) results in an interrupt only if the
//   value remains outside the threshold window for n consecutive
//   integration cycles. For example, if n is equal to 16 and the ADC
//   resolution is set to 16-bits then the integration time is 100ms. An
//   interrupt is generated whenever the last conversion results in a
//   value outside of the programmed threshold window. The
//   interrupt is active-low and remains asserted until cleared by
//   writing the COMMAND register with the CLEAR bit set. Table 4
//   lists the possible interrupt persist bits."
//
#define DEV_LIGHT_REG_INT_PERSIST_CYCLES_1  0
#define DEV_LIGHT_REG_INT_PERSIST_CYCLES_4  1
#define DEV_LIGHT_REG_INT_PERSIST_CYCLES_8  2
#define DEV_LIGHT_REG_INT_PERSIST_CYCLES_16 3

// Interrupt Status Bit:
// From isl29023 Documentation:
// " The interrupt flag bit is a status bit for light intensity detection.
//   The bit is set to logic HIGH when the light intensity crosses the
//   interrupt thresholds window (register address 0x04 - 0x07), and
//   set to logic LOW when its within the interrupt thresholds window.
//   Once the interrupt is triggered, the INT pin goes low and the
//   interrupt status bit goes HIGH until the status bit is polled
//   through the I2C read command. Both the INT pin and the
//   interrupt status bit are automatically cleared at the end of the
//   8-bit Device Register byte (0x00) transfer. Table 5 shows interrupt
//   flag states."
//
#define DEV_LIGHT_REG_INT_STATUS_ACTIVE   0
#define DEV_LIGHT_REG_INT_STATUS_INACTIVE 1

// Operation Mode Bits:
// From isl29023 Documentation:
// " ISL29023 has different operating modes. These modes are
//   selected by setting B5 - B7 bits on register address 0x00. The
//   device powers up on a disable mode. Table 6 lists the possible
//   operating modes."
//
#define DEV_LIGHT_REG_OP_MODE_PWR_DOWN           0
#define DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_ALS 1
#define DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_IR  2
#define DEV_LIGHT_REG_OP_MODE_RSVD1              3
#define DEV_LIGHT_REG_OP_MODE_RSVD2              4
#define DEV_LIGHT_REG_OP_MODE_CONTINUOUS_ALS     5
#define DEV_LIGHT_REG_OP_MODE_CONTINUOUS_IR      6
#define DEV_LIGHT_REG_OP_MODE_RSVD3              7

typedef uint8_t dev_Light_OpMode_t;

// Full Scale Range Bits:
// From isl29023 Documentation:
// " The Full Scale Range (FSR) has four different selectable ranges.
//   Each range has a maximum allowable lux value. Higher range
//   values offer wider ALS lux value. Table 8 lists the possible values
//   of FSR for the 499kΩ REXT resistor."
//
#define DEV_LIGHT_REG_RANGE_1000  0
#define DEV_LIGHT_REG_RANGE_4000  1
#define DEV_LIGHT_REG_RANGE_16000 2
#define DEV_LIGHT_REG_RANGE_64000 3

#define DEV_LIGHT_REG_RANGE1 1000
#define DEV_LIGHT_REG_RANGE2 2000
#define DEV_LIGHT_REG_RANGE3 16000
#define DEV_LIGHT_REG_RANGE4 64000

#define DEV_LIGHT_REG_RANGE_IR 65535

typedef uint16_t dev_Light_Range_t;

// Resolution Bits:
// From isl29023 Documentation:
// " B3 and B2 determine the ADC’s resolution and the number of
//   clock cycles per conversion. Changing the number of clock cycles
//   does more than just change the resolution of the device; it also
//   changes the integration time, which is the period the device’s
//   analog-to-digital (A/D) converter samples the photodiode current
//   signal for a measurement. The ONLY 16-bit ADC resolution is
//   capable of rejecting 50Hz and 60Hz flicker caused by artificial light
//   sources. Table 9 lists the possible ADC resolution."
#define DEV_LIGHT_REG_RESOLUTION_16BIT 0
#define DEV_LIGHT_REG_RESOLUTION_12BIT 1
#define DEV_LIGHT_REG_RESOLUTION_8BIT  2
#define DEV_LIGHT_REG_RESOLUTION_4BIT  3

#define DEV_LIGHT_REG_CMD_BUILD( _op, _prst, _res, _range ) ( ((_res) << 10) | ((_range) << 8) | ((_op) << 5) | (_prst))

typedef uint16_t dev_Light_Reg_t;

// The User Data parameter for I2C transactions is a callback that this
// driver uses to chain I2C transactions together
typedef void (*dev_Light_UsrDataCallback_t)( void );


/*=============================================================================
 *                                   Globals
 *===========================================================================*/
// Callback to be called at the end of the measurement transactions. Can be NULL.
dev_Light_MeasCallback_t dev_Light_measCallback;

// Write buffer is only ever 3 bytes max. 1 byte for the pointer Id and 2 bytes
// for the register value
// Read buffer is only ever 3 bytes max. One-byte for the regId then 2 bytes
// for the read out data
uint8_t dev_Light_i2cBuffer[3];

// Global to hold a single i2c transaction.
bsp_I2c_MasterTrans_t dev_Light_i2cTrans;

dev_Light_OpMode_t dev_Light_opMode;

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/

/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void
dev_Light_i2cTransCallback( bsp_I2c_Status_t status, void* usrData )
{
    if( usrData != NULL )
    {
        ((dev_Light_UsrDataCallback_t)usrData)();
    }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
static void
dev_Light_i2cTransQueue( void* usrData )
{
    dev_Light_i2cTrans.speed    = DEV_LIGHT_I2C_SPEED;
    dev_Light_i2cTrans.addr     = DEV_LIGHT_I2C_ADDR;
    dev_Light_i2cTrans.callback = dev_Light_i2cTransCallback;
    dev_Light_i2cTrans.usrData  = usrData;
    bsp_I2c_masterTransQueue( BSP_PLATFORM_I2C_ISL29023, &dev_Light_i2cTrans );
    return;
}

/*===========================================================================*/
// Wrapper to write to the config register on the ISL29023. This is a simple write of a single
// command byte plus a single data byte for the new register value.
static void
dev_Light_i2cRegWrite( bsp_Light_I2cCmd_t regId,
                       dev_Light_Reg_t    regValue,
                       void*              usrData )
{
    dev_Light_i2cBuffer[0]     = regId;
    dev_Light_i2cBuffer[1]     = ((regValue >> 0) & 0xFF);
    dev_Light_i2cBuffer[2]     = ((regValue >> 8) & 0xFF);
    dev_Light_i2cTrans.type    = BSP_I2C_TRANS_TYPE_WRITE;
    dev_Light_i2cTrans.wLen    = 3;
    dev_Light_i2cTrans.wBuffer = dev_Light_i2cBuffer;
    dev_Light_i2cTrans.rLen    = 0;
    dev_Light_i2cTrans.rBuffer = NULL;
    dev_Light_i2cTransQueue( usrData );
}

/*===========================================================================*/
// Wrapper to read data from the ISL29023. The amount of data available depends on the
// command previously written. Measurements are 3 bytes, the config register is a
// single byte
static void
dev_Light_i2cRegRead( bsp_Light_I2cCmd_t regId,
                      void*              usrData )
{
    dev_Light_i2cBuffer[0]     = regId;
    dev_Light_i2cBuffer[1]     = 0;
    dev_Light_i2cBuffer[2]     = 0;
    dev_Light_i2cTrans.type    = BSP_I2C_TRANS_TYPE_WRITE_READ;
    dev_Light_i2cTrans.wLen    = 1;
    dev_Light_i2cTrans.wBuffer = &dev_Light_i2cBuffer[0];
    dev_Light_i2cTrans.rLen    = 2;
    dev_Light_i2cTrans.rBuffer = &dev_Light_i2cBuffer[1];
    dev_Light_i2cTransQueue( usrData );
}


//*************

/*===========================================================================*/
static void
dev_Light_setThresholdLow( void )
{
    dev_Light_i2cRegWrite( DEV_LIGHT_REG_INT_LT, DEV_LIGHT_THRESHOLD_LOW_DEFAULT, NULL );
}

/*===========================================================================*/
static void
dev_Light_setThresholdHigh( void )
{
    dev_Light_i2cRegWrite( DEV_LIGHT_REG_INT_HT, DEV_LIGHT_THRESHOLD_HIGH_DEFAULT, dev_Light_setThresholdLow );
}

/*===========================================================================*/
static void
dev_Light_clearInterrupt( void )
{
    dev_Light_i2cRegRead( DEV_LIGHT_REG_CMD, dev_Light_setThresholdHigh );
}

/*===========================================================================*/
static void
dev_Light_reset( void )
{
    dev_Light_Reg_t reg = DEV_LIGHT_REG_CMD_BUILD( DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_ALS,
                                                   DEV_LIGHT_INT_PERSIST,
                                                   DEV_LIGHT_RESOLUTION,
                                                   DEV_LIGHT_RANGE );

    dev_Light_i2cRegWrite( DEV_LIGHT_REG_CMD, reg, dev_Light_clearInterrupt );
    return;
}

//*************

/*===========================================================================*/
// Inline function to convert floating point to 10.6 fixed point.
inline uint32_t
dev_Light_cnvFloatToFixed24p8( float input )
{
    return (uint32_t)(input * (1 << 8));
}

/*===========================================================================*/
static void
dev_Light_thresholdReadHandler( void )
{
    dev_Light_MeasLight_t data = ((dev_Light_i2cBuffer[2] << 8) | dev_Light_i2cBuffer[1]);

    // Clear the interrupt
    dev_Light_i2cRegRead( DEV_LIGHT_REG_CMD, NULL );

    // Call the callback
    if( dev_Light_measCallback != NULL )
    {
        static const dev_Light_Range_t rangeAls[] = { DEV_LIGHT_REG_RANGE1,
                                                      DEV_LIGHT_REG_RANGE2,
                                                      DEV_LIGHT_REG_RANGE3,
                                                      DEV_LIGHT_REG_RANGE4 };
        static const float rangeAdc[] = { 65356, 4096, 256, 16 };

        dev_Light_Range_t range;

        if( (dev_Light_opMode ==  DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_ALS) ||
            (dev_Light_opMode ==  DEV_LIGHT_REG_OP_MODE_CONTINUOUS_ALS) )
        {
            range = rangeAls[DEV_LIGHT_RANGE];
        }
        else
        {
            range = DEV_LIGHT_REG_RANGE_IR;
        }

        float dataFloat = (((float)data * range) / rangeAdc[DEV_LIGHT_RESOLUTION]);

        dev_Light_measCallback( dev_Light_cnvFloatToFixed24p8(dataFloat) );
    }
}

/*===========================================================================*/
static void
dev_Light_intHandler( bsp_Gpio_PortId_t    portId,
                      bsp_Gpio_PinOffset_t pinOffset )
{
    dev_Light_i2cRegRead( DEV_LIGHT_REG_DATA, dev_Light_thresholdReadHandler );
    return;
}


/*===========================================================================*/
static void
dev_Light_measTriggerCommon( dev_Light_MeasCallback_t callback,
                             dev_Light_OpMode_t       mode )
{
    dev_Light_Reg_t reg = DEV_LIGHT_REG_CMD_BUILD( mode,
                                                   DEV_LIGHT_INT_PERSIST,
                                                   DEV_LIGHT_RESOLUTION,
                                                   DEV_LIGHT_RANGE );

    // Callback is called through interrupt line
    BSP_MCU_CRITICAL_SECTION_ENTER();
    dev_Light_measCallback = callback;
    dev_Light_opMode = mode;
    BSP_MCU_CRITICAL_SECTION_EXIT();

    // Trigger the measurement
    dev_Light_i2cRegWrite( DEV_LIGHT_REG_CMD, reg, NULL );
}


/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
dev_Light_init( void )
{
    dev_Light_measCallback = NULL;

    /* Disable the GPIO interrupt while configuring */
    bsp_Gpio_intControl( BSP_GPIO_PORT_ID_INT_LIGHT,
                         BSP_GPIO_BIT_MASK_INT_LIGHT,
                         BSP_GPIO_INT_CONTROL_DISABLE );

    /* Configure as input */
    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_INT_LIGHT,
                          BSP_GPIO_BIT_MASK_INT_LIGHT,
                          FALSE,
                          BSP_GPIO_PULL_NONE );

    bsp_Gpio_intConfig( BSP_GPIO_PORT_ID_INT_LIGHT,
                        BSP_GPIO_BIT_MASK_INT_LIGHT,
                        FALSE,
                        FALSE,
                        BSP_GPIO_INT_TYPE_EDGE_FALLING,
                        dev_Light_intHandler );

    /* Enable the GPIO interrupt after done configuring */
    bsp_Gpio_intControl( BSP_GPIO_PORT_ID_INT_LIGHT,
                         BSP_GPIO_BIT_MASK_INT_LIGHT,
                         BSP_GPIO_INT_CONTROL_ENABLE );

    /* make sure I2C is enabled */
    bsp_I2c_masterControl( BSP_PLATFORM_I2C_ISL29023, BSP_I2C_CONTROL_ENABLE );

    dev_Light_reset();

    return;
}


/*===========================================================================*/
void
dev_Light_measTriggerAls( dev_Light_MeasCallback_t callback )
{
    dev_Light_measTriggerCommon( callback, DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_ALS );
    return;
}


/*===========================================================================*/
void
dev_Light_measTriggerIr( dev_Light_MeasCallback_t callback )
{
    dev_Light_measTriggerCommon( callback, DEV_LIGHT_REG_OP_MODE_ONCE_PER_CYCLE_IR );
    return;
}
#endif
