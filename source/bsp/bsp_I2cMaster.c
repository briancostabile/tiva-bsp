/**
 * Copyright 2017 Brian Costabile
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
/*============================================================================*/
/**
 * @file bsp_I2cMaster.c
 * @brief Contains Functions for configuring and accessing the I2C peripheral
 *        bus
 */

#include "bsp_Types.h"
#include "bsp_I2cMaster.h"
#include "bsp_Gpio.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"



/*============================================================================*/
#define BSP_I2CMASTER_REG( _name ) ADDR_TO_REG((I2C3_BASE + I2C_O_##_name))


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_I2cMaster_init( void )
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID(I2C_SCL),
                          BSP_GPIO_MASK(I2C_SCL),
                          FALSE,
                          BSP_GPIO_PULL_NONE );

    bsp_Gpio_configAltFunction( BSP_GPIO_PORT_ID(I2C_SCL),
                                BSP_GPIO_MASK(I2C_SCL),
                                FALSE,
                                BSP_GPIO_ALT_FUNC(PD0_I2C3SCL) );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID(I2C_SDA),
                          BSP_GPIO_MASK(I2C_SDA),
                          TRUE,
                          BSP_GPIO_PULL_UP );

    bsp_Gpio_configAltFunction( BSP_GPIO_PORT_ID(I2C_SDA),
                                BSP_GPIO_MASK(I2C_SDA),
                                FALSE,
                                BSP_GPIO_ALT_FUNC(PD1_I2C3SDA) );

    /* Enable Master mode */
    BSP_I2CMASTER_REG(MCR) = 0x10;

    /* Set Timer Period */
    BSP_I2CMASTER_REG(MTPR) = 0x03;

    /* Set Master address to 0x00 */
    BSP_I2CMASTER_REG(MSA) = 0x80;

    BSP_I2CMASTER_REG(MDR) = 0x55;

    BSP_I2CMASTER_REG(MCS) = 0x07;
    //I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    // Wait until done transmitting
    //while(I2CMasterBusy(I2C_BASE));

    return;
}

/*============================================================================*/
void
bsp_I2cMaster_snd( uint8_t addr,
                   size_t  len,
                   void*   data )
{
    return;
}

/*============================================================================*/
void
bsp_I2cMaster_rcv( uint8_t addr,
                   size_t  len,
                   void*   data )
{
    return;
}
