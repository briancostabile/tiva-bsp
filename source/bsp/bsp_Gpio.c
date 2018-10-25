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
 * @file bsp_Gpio.c
 * @brief Contains Functions for configuring and accessing the GPIOs
 */
#include "bsp_Types.h"
#include "bsp_Gpio.h"
#include "bsp_Mcu.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include <string.h>

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Gpio_init( void )
{
    bsp_Gpio_PortId_t portId;
    bsp_Gpio_PinOffset_t numInts;
    bsp_Gpio_PinOffset_t i;

    /* Every port is enabled and the interrupts cleared */
    for( portId=0; portId<DIM(bsp_Gpio_platformPortInfoTable); portId++ )
    {
        /* Enable all of the GPIO peripheral blocks */
        MAP_SysCtlPeripheralEnable( bsp_Gpio_platformPortInfoTable[portId].sysCtrlAddr );

        if( bsp_Gpio_platformPortInfoTable[portId].bitInterruptable == TRUE )
        {
            numInts = BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT;
        }
        else
        {
            numInts = 1;
        }

        /* Disable and clear all interrupts */
        for( i=0; i<numInts; i++ )
        {
            bsp_Interrupt_disable( (bsp_Gpio_platformPortInfoTable[portId].intId + i) );
            bsp_Interrupt_clearPending( (bsp_Gpio_platformPortInfoTable[portId].intId + i));
        }

        /* Clear the callback table */
        memset( bsp_Gpio_platformPortInfoTable[portId].handlerTable,
                0,
                BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT );

        /* Wait for the peripheral to be ready in the system controller before moving on */
        while( MAP_SysCtlPeripheralReady( bsp_Gpio_platformPortInfoTable[portId].sysCtrlAddr ) == FALSE );
    }

    bsp_Gpio_initPlatform();
    
    return;
}


/*============================================================================*/
void
bsp_Gpio_write( bsp_Gpio_PortId_t  portId,
		        bsp_Gpio_BitMask_t mask,
		        bsp_Gpio_BitMask_t val )
{
    uint32_t portAddr;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    /* Convert to pointer first then add mask so it adds in increments of 32-bits */
	*((ADDR_TO_PTR(portAddr + BSP_GPIO_BASE_ADDR_OFFSET_DATA)) + mask) = val;

	return;
}


/*============================================================================*/
bsp_Gpio_BitMask_t
bsp_Gpio_read( bsp_Gpio_PortId_t  portId,
		       bsp_Gpio_BitMask_t mask )
{
	bsp_Gpio_BitMask_t returnVal = 0x00;
	uint32_t portAddr;

	portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    /* Convert to pointer first then add mask so it adds in increments of 32-bits */
	returnVal = (bsp_Gpio_BitMask_t)*((ADDR_TO_PTR(portAddr + BSP_GPIO_BASE_ADDR_OFFSET_DATA)) + mask);

	return( returnVal );
}


/*============================================================================*/
void
bsp_Gpio_configAltFunction( bsp_Gpio_PortId_t    portId,
                            bsp_Gpio_BitMask_t   mask,
                            bool_t               analog,
                            bsp_Gpio_AltFuncId_t altFuncId )
{
    uint32_t portAddr;
    uint32_t regMask;
    uint32_t regVal;
    bsp_Gpio_BitMask_t tmpMask;
    uint8_t bit;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();
    if( analog == TRUE )
    {
        /* Disable the digital select bit */
        BSP_GPIO_REG(portAddr, DEN) &= ~mask;
        BSP_GPIO_REG(portAddr, AMSEL) |= mask;
    }
    else
    {
        /* Each IO pin has a 4 bit selector in the Port Control register */
        /* Build a mask and new value to apply in place of the mask bits */
        regMask = 0x00000000;
        regVal  = 0x00000000;
        tmpMask = mask;
        bit = 0;
        while( tmpMask != 0x00 )
        {
            regMask <<= 4;
            regVal <<= 4;
            if( (tmpMask & 0x01) == 0x01 )
            {
                regMask |= (0x0000000F << (bit * 4));
                regVal  |= (altFuncId << (bit * 4));
            }
            tmpMask >>= 1;
            bit++;
        }

        BSP_GPIO_REG(portAddr, PCTL) &= ~regMask;
        BSP_GPIO_REG(portAddr, PCTL) |= regVal;

        /* Make sure the digital select bit is chosen */
        BSP_GPIO_REG(portAddr, DEN) |= mask;
        BSP_GPIO_REG(portAddr, AFSEL) |= mask;
    }

    BSP_GPIO_COMMIT( portAddr, mask );

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
void
bsp_Gpio_configOutput( bsp_Gpio_PortId_t  portId,
                       bsp_Gpio_BitMask_t mask,
                       bool_t             openDrain,
                       bsp_Gpio_Drive_t   drive )
{
    uint32_t portAddr;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    BSP_GPIO_REG(portAddr, DEN) |= mask;
    BSP_GPIO_REG(portAddr, DIR) |= mask;

    /* Disable Pull Up/Down resistors */
    BSP_GPIO_REG(portAddr, PUR) &= ~mask;
    BSP_GPIO_REG(portAddr, PDR) &= ~mask;

    /* Select main IO function */
    BSP_GPIO_REG(portAddr, AFSEL) &= ~mask;

    if( openDrain == TRUE )
    {
        BSP_GPIO_REG(portAddr, ODR) |= mask;
    }
    else
    {
        BSP_GPIO_REG(portAddr, ODR) &= ~mask;
    }

    if( drive == BSP_GPIO_DRIVE_2MA )
    {
        BSP_GPIO_REG(portAddr, DR2D) |= mask;
    }
    else if( drive == BSP_GPIO_DRIVE_4MA )
    {
        BSP_GPIO_REG(portAddr, DR4D) |= mask;
    }
    else if( drive == BSP_GPIO_DRIVE_8MA )
    {
        BSP_GPIO_REG(portAddr, DR8D) |= mask;
        BSP_GPIO_REG(portAddr, SLR)  &= ~mask;
    }
    else
    {
        BSP_GPIO_REG(portAddr, DR8D) |= mask;
        BSP_GPIO_REG(portAddr, SLR)  |= mask;
    }

    BSP_GPIO_COMMIT( portAddr, mask );

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
void
bsp_Gpio_configInput( bsp_Gpio_PortId_t  portId,
                      bsp_Gpio_BitMask_t mask,
                      bool_t             openDrain,
                      bsp_Gpio_Pull_t    pull )
{
    uint32_t portAddr;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    BSP_GPIO_REG(portAddr, DEN) |= mask;
    BSP_GPIO_REG(portAddr, DIR) &= ~mask;

    if( openDrain == TRUE )
    {
        BSP_GPIO_REG(portAddr, ODR) |= mask;
    }
    else
    {
        BSP_GPIO_REG(portAddr, ODR) &= ~mask;
    }

    if( pull == BSP_GPIO_PULL_NONE )
    {
        BSP_GPIO_REG(portAddr, PUR) &= ~mask;
        BSP_GPIO_REG(portAddr, PDR) &= ~mask;
    }
    else if( pull == BSP_GPIO_PULL_UP )
    {
        BSP_GPIO_REG(portAddr, PUR) |= mask;
        BSP_GPIO_REG(portAddr, PDR) &= ~mask;
    }
    else if( pull == BSP_GPIO_PULL_DOWN )
    {
        BSP_GPIO_REG(portAddr, PUR) &= ~mask;
        BSP_GPIO_REG(portAddr, PDR) |= mask;
    }

    BSP_GPIO_COMMIT( portAddr, mask );

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
void
bsp_Gpio_intControl( bsp_Gpio_PortId_t     portId,
                     bsp_Gpio_BitMask_t    mask,
                     bsp_Gpio_IntControl_t control )
{
    uint32_t portAddr;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    if( control == BSP_GPIO_INT_CONTROL_ENABLE )
    {
        BSP_GPIO_REG(portAddr, IM) |= mask;
    }
    else
    {
        BSP_GPIO_REG(portAddr, IM) &= ~mask;
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
void
bsp_Gpio_intConfig( bsp_Gpio_PortId_t       portId,
                    bsp_Gpio_BitMask_t      mask,
                    bool_t                  dmaTriggerEnable,
                    bool_t                  adcTriggerEnable,
                    bsp_Gpio_IntType_t      type,
                    bsp_Gpio_InputHandler_t callback )
{
    uint32_t portAddr;

    portAddr = bsp_Gpio_platformPortInfoTable[portId].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    if( dmaTriggerEnable == TRUE )
    {
        BSP_GPIO_REG(portAddr, DMACTL) |= mask;
    }

    if( adcTriggerEnable == TRUE )
    {
        BSP_GPIO_REG(portAddr, ADCCTL) |= mask;
    }

    if( type == BSP_GPIO_INT_TYPE_EDGE_RISING )
    {
        BSP_GPIO_REG(portAddr, IS)  &= ~mask;
        BSP_GPIO_REG(portAddr, IBE) &= ~mask;
        BSP_GPIO_REG(portAddr, IEV) |= mask;
    }
    else if( type == BSP_GPIO_INT_TYPE_EDGE_FALLING )
    {
        BSP_GPIO_REG(portAddr, IS)  &= ~mask;
        BSP_GPIO_REG(portAddr, IBE) &= ~mask;
        BSP_GPIO_REG(portAddr, IEV) &= ~mask;
    }
    else if( type == BSP_GPIO_INT_TYPE_EDGE_BOTH )
    {
        BSP_GPIO_REG(portAddr, IS)  &= ~mask;
        BSP_GPIO_REG(portAddr, IBE) |= mask;
    }
    else if( type == BSP_GPIO_INT_TYPE_LEVEL_LOW )
    {
        BSP_GPIO_REG(portAddr, IS)  |= mask;
        BSP_GPIO_REG(portAddr, IEV) |= mask;
    }
    else if( type == BSP_GPIO_INT_TYPE_LEVEL_HIGH )
    {
        BSP_GPIO_REG(portAddr, IS)  |= mask;
        BSP_GPIO_REG(portAddr, IEV) &= ~mask;
    }

    /* Register handler for each masked bit passed in */
    for( uint8_t pin=0; pin<BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT; pin++ )
    {
        if( ((1 << pin) & mask) != 0 )
        {
            bsp_Gpio_platformPortInfoTable[portId].handlerTable[pin] = callback;
        }
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

