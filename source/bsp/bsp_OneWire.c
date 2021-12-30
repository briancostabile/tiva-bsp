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

#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include "bsp_Assert.h"
#include "bsp_OneWire.h"
#include "bsp_Gpio.h"

#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/onewire.h"

/*=============================================================================
 *                               Types
 *===========================================================================*/

#if 0
/*=============================================================================
 *                               Functions
 *===========================================================================*/
/*===========================================================================*/
void
bsp_OneWire_init( void )
{
    GPIOPinTypeOneWire( BSP_GPIO_BASE_ADDR_1WIRE,
                        BSP_GPIO_BIT_MASK_1WIRE );

    // MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_ONEWIRE0 );
    // while( !MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ONEWIRE0) );

    MAP_OneWireInit( ONEWIRE0_BASE,
                     (ONEWIRE_INIT_SPD_STD |
                      ONEWIRE_INIT_READ_STD |
                      ONEWIRE_INIT_ATR |
                      ONEWIRE_INIT_STD_POL |
                      ONEWIRE_INIT_1_WIRE_CFG) );

    // Disable all interrupts
    MAP_OneWireIntDisable(ONEWIRE0_BASE,
                         (ONEWIRE_INT_RESET_DONE |
                          ONEWIRE_INT_OP_DONE |
                          ONEWIRE_INT_NO_SLAVE |
                          ONEWIRE_INT_STUCK |
                          ONEWIRE_INT_DMA_DONE));

    // Clear all interrupts
    MAP_OneWireIntClear( ONEWIRE0_BASE,
                         (ONEWIRE_INT_RESET_DONE |
                          ONEWIRE_INT_OP_DONE |
                          ONEWIRE_INT_NO_SLAVE |
                          ONEWIRE_INT_STUCK |
                          ONEWIRE_INT_DMA_DONE) );

    MAP_OneWireBusReset(ONEWIRE0_BASE);
    return;
}


/*===========================================================================*/
bool
bsp_OneWire_snd( uint32_t sndData,
                 uint8_t  sndNumBits )
{
    MAP_OneWireTransaction( ONEWIRE0_BASE,
                            (ONEWIRE_OP_RESET | ONEWIRE_OP_WRITE),
                            sndData,
                            sndNumBits );
    return true;
}

/*===========================================================================*/
bool
bsp_OneWire_sndRcv( uint32_t sndData,
                    uint8_t  sndNumBits,
                    uint8_t* rcvDataPtr,
                    uint8_t  rcvDataLen )
{
    MAP_OneWireTransaction( ONEWIRE0_BASE,
                            (ONEWIRE_OP_RESET | ONEWIRE_OP_WRITE),
                            sndData,
                            sndNumBits );

    uint32_t tmp32 = 0;
    uint8_t rcvIdx = 0;
    while (rcvIdx < rcvDataLen)
    {
        MAP_OneWireDataGet( ONEWIRE0_BASE, &tmp32 );
        rcvDataPtr[rcvIdx++] = ((tmp32 >>  0) & 0xFF);
        rcvDataPtr[rcvIdx++] = ((tmp32 >>  8) & 0xFF);
        rcvDataPtr[rcvIdx++] = ((tmp32 >> 16) & 0xFF);
        rcvDataPtr[rcvIdx++] = ((tmp32 >> 24) & 0xFF);
    }

    return true;
}
#endif