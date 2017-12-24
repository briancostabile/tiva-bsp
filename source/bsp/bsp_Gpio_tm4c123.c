/*============================================================================*/
/**
 * @file bsp_Gpio_ektm4c123gxl.c
 * @brief Contains Functions for configuring and accessing the GPIOs
 */

#include "bsp_Gpio.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"
#include <string.h>

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

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
    tmpMis = BSP_GPIO_REG( portBaseAddr, MIS );

    /* Clear interrupts */
    BSP_GPIO_REG( portBaseAddr, ICR ) = tmpMis;

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
    tmpMis = BSP_GPIO_REG( portBaseAddr, MIS );

    /* for P0/Q0 interrupts occur when any of the other pins generate an interrupt so make sure
     * this function only handles the expected interrupt
     */
    bitMask = (1 << bitOffset);
    if( (tmpMis & bitMask) != 0 )
    {
        /* Clear interrupt */
        BSP_GPIO_REG( portBaseAddr, ICR ) = bitMask;

        /* Call handler */
        handlerTable[ bitOffset ]( portId, bitOffset );
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
