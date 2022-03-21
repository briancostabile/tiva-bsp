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
 * @file bsp_Crc.c
 * @brief Contains Functions for configuring and accessing the System CRC
 */
#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include "bsp_Crc.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/crc.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Types
 *============================================================================*/
#define BSP_CRC_32P04C11DB7_SEED 0x5A5A5A5A

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Crc_init(void)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_CCM0);
    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_CCM0))
        ;

    return;
}

/*============================================================================*/
uint32_t bsp_Crc_32p04C11DB7(uint32_t *dataPtr, size_t len32)
{
    MAP_CRCConfigSet(CCM0_BASE, (CRC_CFG_INIT_SEED | CRC_CFG_TYPE_P4C11DB7 | CRC_CFG_SIZE_32BIT));

    MAP_CRCSeedSet(CCM0_BASE, BSP_CRC_32P04C11DB7_SEED);

    return (MAP_CRCDataProcess(CCM0_BASE, dataPtr, len32, false));
}
