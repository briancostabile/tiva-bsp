/*============================================================================*/
/**
 * @file bsp_Gpio_ektm4c123gxl.c
 * @brief Contains Configuration table for the supported IO ports on this platform
 */

#include "bsp_Gpio.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"

#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Global Data
 *============================================================================*/
/*============================================================================*/
bsp_Gpio_InputHandler_t bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_NUM_PORTS][BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT];

/*============================================================================*/
const bsp_Gpio_PlatformPortInfo_t bsp_Gpio_platformPortInfoTable[ BSP_GPIO_PORT_ID_NUM_PORTS ] =
{
    { GPIO_PORTA_BASE, SYSCTL_PERIPH_GPIOA, BSP_INTERRUPT_ID_GPIOA, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_A][0]) },
    { GPIO_PORTB_BASE, SYSCTL_PERIPH_GPIOB, BSP_INTERRUPT_ID_GPIOB, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_B][0]) },
    { GPIO_PORTC_BASE, SYSCTL_PERIPH_GPIOC, BSP_INTERRUPT_ID_GPIOC, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_C][0]) },
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, BSP_INTERRUPT_ID_GPIOD, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_D][0]) },
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, BSP_INTERRUPT_ID_GPIOE, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_E][0]) },
    { GPIO_PORTF_BASE, SYSCTL_PERIPH_GPIOF, BSP_INTERRUPT_ID_GPIOF, FALSE, &(bsp_Gpio_inputHandlerTable[BSP_GPIO_PORT_ID_F][0]) }
};
