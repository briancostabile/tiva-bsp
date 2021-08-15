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
 * @file bsp_Gpio_tm4c123.c
 * @brief Contains Functions for configuring and accessing the GPIOs
 */
#include "bsp_Gpio.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"
#include <string.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
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

/*==============================================================================
 *                            Public Functions
 *============================================================================*/

/*============================================================================*/
void
bsp_Gpio_isrCommon( bsp_Gpio_PortId_t portId )
{
    bsp_Gpio_InputHandler_t* handlerTable;
    uint32_t                 portBaseAddr;
    uint32_t                 tmpMis;
    uint8_t                  bitOffset;

    portBaseAddr = bsp_Gpio_platformPortInfoTable[ portId ].baseAddr;
    handlerTable = bsp_Gpio_platformPortInfoTable[ portId ].handlerTable;

    /* Read masked interrupt register */
    tmpMis = MAP_GPIOIntStatus( portBaseAddr, false );

    /* Clear interrupts */
    MAP_GPIOIntClear( portBaseAddr, tmpMis );

    bitOffset = 0;
    while( tmpMis != 0 )
    {
        if( ((tmpMis & 0x00000001) != 0) &&
            (handlerTable[ bitOffset ] != NULL) )
        {
            handlerTable[ bitOffset ]( portId, bitOffset );
        }
        tmpMis = (tmpMis >> 1);
        bitOffset++;
    }
    return;
}

/*============================================================================*/
void
bsp_Gpio_isrPinCommon( bsp_Gpio_PortId_t    portId,
                       bsp_Gpio_PinOffset_t bitOffset )
{
    bsp_Gpio_InputHandler_t* handlerTable;
    uint32_t                 portBaseAddr;
    uint32_t                 tmpMis;
    uint32_t                 bitMask;

    portBaseAddr = bsp_Gpio_platformPortInfoTable[ portId ].baseAddr;
    handlerTable = bsp_Gpio_platformPortInfoTable[ portId ].handlerTable;

    /* Read masked interrupt register */
    tmpMis = MAP_GPIOIntStatus( portBaseAddr, false );

    /* for P0/Q0 interrupts occur when any of the other pins generate an interrupt so make sure
     * this function only handles the expected interrupt
     */
    bitMask = (1 << bitOffset);
    if( (tmpMis & bitMask) != 0 )
    {
        /* Clear interrupt */
        MAP_GPIOIntClear( portBaseAddr, bitMask );

        /* Call handler */
        if( handlerTable[ bitOffset ] != NULL )
        {
            handlerTable[ bitOffset ]( portId, bitOffset );
        }
    }

    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortA( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_A );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortB( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_B );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortC( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_C );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortD( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_D );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortE( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_E );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortF( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_F );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortG( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_G );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortH( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_H );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortJ( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_J );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortK( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_K );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortL( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_L );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortM( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_M );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortN( void )
{
    bsp_Gpio_isrCommon( BSP_GPIO_PORT_ID_N );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP0( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 0 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP1( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 1 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP2( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 2 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP3( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 3 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP4( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 4 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP5( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 5 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP6( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 6 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortP7( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_P, 7 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ0( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 0 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ1( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 1 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ2( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 2 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ3( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 3 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ4( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 4 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ5( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 5 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ6( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 6 );
    return;
}

/*============================================================================*/
void
bsp_Gpio_interruptHandlerPortQ7( void )
{
    bsp_Gpio_isrPinCommon( BSP_GPIO_PORT_ID_Q, 7 );
    return;
}
