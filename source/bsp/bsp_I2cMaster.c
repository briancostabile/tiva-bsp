/*============================================================================*/
/**
 * @file bsp_I2cPerIf.c
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
