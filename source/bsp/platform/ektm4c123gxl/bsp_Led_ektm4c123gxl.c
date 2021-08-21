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
 * @file bsp_Led_ektm4c123gxl.c
 * @brief Contains table of IOs used as LEDs
 */

#include "bsp_Types.h"
#include "bsp_Led.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define BSP_LED_INFO_ENTRY(_name) {  \
    BSP_PLATFORM_NAME_##_name,       \
    BSP_PLATFORM_COLOR_##_name,      \
    BSP_PLATFORM_PORT_ID_##_name,    \
    BSP_PLATFORM_BASE_ADDR_##_name,  \
    BSP_PLATFORM_BIT_MASK_##_name,   \
    BSP_PLATFORM_DRIVE_##_name       \
}

/*==============================================================================
 *                               Globals
 *============================================================================*/
/*============================================================================*/
static const bsp_Led_GroupElementInfo_t bsp_Led_status[] =
{
    BSP_LED_INFO_ENTRY(LED_0),
    BSP_LED_INFO_ENTRY(LED_1),
    BSP_LED_INFO_ENTRY(LED_2)
};

/*============================================================================*/
/* One entry for each IO port. */
const bsp_Led_Info_t bsp_Led_infoTable[BSP_PLATFORM_LED_NUM] =
{
    {"status", bsp_Led_status, DIM(bsp_Led_status)}
};
