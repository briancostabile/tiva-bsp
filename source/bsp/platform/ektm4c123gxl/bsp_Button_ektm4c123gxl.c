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
 * @file bsp_Button_ektm4c123gxl.c
 * @brief Contains table of IOs used as simple switch-style buttons
 */

#include "bsp_Types.h"
#include "bsp_Button.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                               Globals
 *============================================================================*/
/*============================================================================*/
/* One entry for each IO port. */
const bsp_Button_IoInfo_t bsp_Button_ioInfoTable[BSP_PLATFORM_IO_BUTTON_NUM] = {
    {BSP_GPIO_PORT_ID(BUTTON_0), BSP_GPIO_MASK(BUTTON_0), BSP_BUTTON_PRESS_POLARITY_LOW},
    {BSP_GPIO_PORT_ID(BUTTON_1), BSP_GPIO_MASK(BUTTON_1), BSP_BUTTON_PRESS_POLARITY_LOW}};
