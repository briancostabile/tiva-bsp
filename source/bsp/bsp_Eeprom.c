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
 * @file bsp_Eeprom.c
 * @brief Contains Functions for configuring and accessing the System CRC
 */
#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include "bsp_Assert.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/eeprom.h"
#include "driverlib/sysctl.h"


/*==============================================================================
 *                            Public Functions
 *============================================================================*/

/*============================================================================*/
void
bsp_Eeprom_init( void )
{
    MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_EEPROM0 );
    while( !MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0) );
    MAP_EEPROMInit();
    MAP_EEPROMIntDisable( EEPROM_INT_PROGRAM );
    MAP_EEPROMIntClear( EEPROM_INT_PROGRAM );
    return;
}

/*============================================================================*/
void
bsp_Eeprom_read( uint32_t* dataPtr,
                 uint32_t  eepromAddr,
                 size_t    len32 )
{
    BSP_ASSERT( (eepromAddr & 0x00000003) == 0 );
    MAP_EEPROMRead( dataPtr, eepromAddr, (len32 * 4) );
    return;
}

/*============================================================================*/
void
bsp_Eeprom_write( uint32_t  eepromAddr,
                  uint32_t* dataPtr,
                  size_t    len32 )
{
    BSP_ASSERT( (eepromAddr & 0x00000003) == 0 );
    MAP_EEPROMProgram( dataPtr, eepromAddr, (len32 * 4) );
    return;
}
