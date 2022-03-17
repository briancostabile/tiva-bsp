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
 * @file bsp_Led.c
 * @brief Contains Functions for configuring and accessing the GPIOs
 */
#include "bsp_Types.h"
#include "bsp_Led.h"
#include "bsp_Mcu.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include <string.h>

/*============================================================================*/
extern const bsp_Led_Info_t bsp_Led_infoTable[BSP_PLATFORM_LED_NUM];

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
//*============================================================================*/
void
bsp_Led_init( void )
{
    for( uint8_t i=0; i<DIM(bsp_Led_infoTable); i++ )
    {
        for( uint8_t j=0; j<bsp_Led_infoTable[i].cnt; j++ )
        {
            /* For each port configure the LED IO as output and set to low */
            const bsp_Led_GroupElementInfo_t* ledPtr = &bsp_Led_infoTable[i].groupTable[j];
            bsp_Gpio_configOutput( ledPtr->portId, ledPtr->mask, FALSE, ledPtr->drive );
            bsp_Gpio_write( ledPtr->portId, ledPtr->mask, (!(ledPtr->polarity) * ledPtr->mask) );
        }
    }
    return;
}

/*============================================================================*/
void
bsp_Led_setColor( bsp_Led_Id_t    id,
                  bsp_Led_Color_t color )
{
    // For now any color turns LED on full
    const bsp_Led_GroupElementInfo_t* ledPtr;
    for( uint8_t j=0; j<bsp_Led_infoTable[id].cnt; j++ )
    {
        ledPtr = &bsp_Led_infoTable[id].groupTable[j];
        bsp_Gpio_write( ledPtr->portId, ledPtr->mask,
                        (color & ledPtr->color) ?
                            ((ledPtr->polarity) * ledPtr->mask) :
                            (!(ledPtr->polarity) * ledPtr->mask) );
    }
    return;
}
