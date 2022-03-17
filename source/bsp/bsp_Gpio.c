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
#include "driverlib/gpio.h"

#include <string.h>

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Gpio_init( void )
{
    bsp_Gpio_PortId_t    portId;
    bsp_Gpio_PinOffset_t numInts;
    bsp_Gpio_PinOffset_t i;

    /* Every port is enabled and the interrupts cleared */
    for( portId=0; portId<DIM(bsp_Gpio_platformPortInfoTable); portId++ )
    {
        if( MAP_SysCtlPeripheralPresent( bsp_Gpio_platformPortInfoTable[portId].sysCtrlAddr ) == true )
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

            /* Re-Enable all interrupts */
            for( i=0; i<numInts; i++ )
            {
                bsp_Interrupt_enable( (bsp_Gpio_platformPortInfoTable[portId].intId + i) );
            }

            /* Wait for the peripheral to be ready in the system controller before moving on */
            while( MAP_SysCtlPeripheralReady( bsp_Gpio_platformPortInfoTable[portId].sysCtrlAddr ) == FALSE );

            /* Unlock All pins */
            MAP_GPIOUnlockPin( bsp_Gpio_platformPortInfoTable[portId].baseAddr, 0xFF );
        }
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
    MAP_GPIOPinWrite( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask, val );
    return;
}


/*============================================================================*/
bsp_Gpio_BitMask_t
bsp_Gpio_read( bsp_Gpio_PortId_t  portId,
               bsp_Gpio_BitMask_t mask )
{
    bsp_Gpio_BitMask_t returnVal;
    returnVal = MAP_GPIOPinRead( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
    return( returnVal );
}


/*============================================================================*/
void
bsp_Gpio_configAnalog( bsp_Gpio_PortId_t    portId,
                       bsp_Gpio_BitMask_t   mask )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    MAP_GPIOPinTypeADC( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}


/*============================================================================*/
void
bsp_Gpio_configAltFunction( bsp_Gpio_PortId_t    portId,
                            bsp_Gpio_BitMask_t   mask,
                            bsp_Gpio_AltFuncId_t altFuncId )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    GPIODirModeSet( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask, GPIO_DIR_MODE_HW );
    MAP_GPIOPinConfigure( altFuncId );
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
    uint32_t pinType = GPIO_PIN_TYPE_STD;
    uint32_t strength = GPIO_STRENGTH_2MA;
    if( openDrain == true )
    {
        pinType = GPIO_PIN_TYPE_OD;
    }
    else
    {
        if( drive == BSP_GPIO_DRIVE_2MA )
        {
            strength = GPIO_STRENGTH_2MA;
        }
        else if( drive == BSP_GPIO_DRIVE_4MA )
        {
            strength = GPIO_STRENGTH_4MA;
        }
        else if( drive == BSP_GPIO_DRIVE_8MA )
        {
            strength = GPIO_STRENGTH_8MA;
        }
        else if( drive == BSP_GPIO_DRIVE_8MA_SC )
        {
            strength = GPIO_STRENGTH_8MA_SC;
        }
        else if( drive == BSP_GPIO_DRIVE_6MA )
        {
            strength = GPIO_STRENGTH_6MA;
        }
        else if( drive == BSP_GPIO_DRIVE_10MA )
        {
            strength = GPIO_STRENGTH_10MA;
        }
        else
        {
            strength = GPIO_STRENGTH_12MA;
        }
    }
    BSP_MCU_CRITICAL_SECTION_ENTER();
    MAP_GPIOPinTypeGPIOOutput( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
    MAP_GPIOPadConfigSet( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask, strength, pinType );
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
    uint32_t pinType = GPIO_PIN_TYPE_STD;
    if( openDrain == true )
    {
        pinType = GPIO_PIN_TYPE_OD;
    }
    else if( pull == BSP_GPIO_PULL_UP )
    {
        pinType = GPIO_PIN_TYPE_STD_WPU;
    }
    else if( pull == BSP_GPIO_PULL_DOWN )
    {
        pinType = GPIO_PIN_TYPE_STD_WPD;
    }
    BSP_MCU_CRITICAL_SECTION_ENTER();
    MAP_GPIOPinTypeGPIOInput( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
    MAP_GPIOPadConfigSet( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask, BSP_GPIO_DRIVE_2MA, pinType );
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}

/*============================================================================*/
void
bsp_Gpio_intControl( bsp_Gpio_PortId_t     portId,
                     bsp_Gpio_BitMask_t    mask,
                     bsp_Gpio_IntControl_t control )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    if( control == BSP_GPIO_INT_CONTROL_ENABLE )
    {
        MAP_GPIOIntEnable( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
    }
    else
    {
        MAP_GPIOIntDisable( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );
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
        MAP_GPIODMATriggerEnable( portAddr, mask );
    }
    else
    {
        MAP_GPIODMATriggerDisable( portAddr, mask );
    }

    if( adcTriggerEnable == TRUE )
    {
        MAP_GPIOADCTriggerEnable( portAddr, mask );
    }
    else
    {
        MAP_GPIOADCTriggerDisable( portAddr, mask );
    }

    uint32_t intType = (bsp_Gpio_platformPortInfoTable[portId].bitInterruptable) ? GPIO_DISCRETE_INT : 0;;
    if( type == BSP_GPIO_INT_TYPE_EDGE_RISING )
    {
        intType |= GPIO_RISING_EDGE;
    }
    else if( type == BSP_GPIO_INT_TYPE_EDGE_FALLING )
    {
        intType |= GPIO_FALLING_EDGE;
    }
    else if( type == BSP_GPIO_INT_TYPE_EDGE_BOTH )
    {
        intType |= GPIO_BOTH_EDGES;
    }
    else if( type == BSP_GPIO_INT_TYPE_LEVEL_LOW )
    {
        intType |= GPIO_LOW_LEVEL;
    }
    else if( type == BSP_GPIO_INT_TYPE_LEVEL_HIGH )
    {
        intType |= GPIO_HIGH_LEVEL;
    }
    MAP_GPIOIntTypeSet( portAddr, mask, intType );

    /* Register handler for each masked bit passed in */
    for( uint8_t pin=0; pin<BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT; pin++ )
    {
        if( ((1 << pin) & mask) != 0 )
        {
            bsp_Gpio_platformPortInfoTable[portId].handlerTable[pin] = callback;
        }
    }

    MAP_GPIOIntClear( bsp_Gpio_platformPortInfoTable[portId].baseAddr, mask );

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

