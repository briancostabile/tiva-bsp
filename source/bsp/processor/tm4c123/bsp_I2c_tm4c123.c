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
 * @file bsp_I2c_tm4c123.c
 * @brief Contains processor specific I2C tables and functions
 */

#include "bsp_Gpio.h"
#include "bsp_I2c.h"
#include "bsp_Clk.h"
#include "bsp_Interrupt.h"
#include <string.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Global Data
 *============================================================================*/
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl0[] =
{
    {BSP_GPIO_PORT_ID(PB2), BSP_GPIO_MASK(PB2), BSP_GPIO_ALT_FUNC(PB2_I2C0SCL)} // Pin 47
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda0[] =
{
    {BSP_GPIO_PORT_ID(PB3), BSP_GPIO_MASK(PB3), BSP_GPIO_ALT_FUNC(PB3_I2C0SDA)} // Pin 48
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl1[] =
{
     {BSP_GPIO_PORT_ID(PA6), BSP_GPIO_MASK(PA6), BSP_GPIO_ALT_FUNC(PA6_I2C1SCL)} // Pin 23
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda1[] =
{
     {BSP_GPIO_PORT_ID(PA7), BSP_GPIO_MASK(PA7), BSP_GPIO_ALT_FUNC(PA7_I2C1SDA)}  // Pin 24
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl2[] =
{
     {BSP_GPIO_PORT_ID(PE4), BSP_GPIO_MASK(PE4), BSP_GPIO_ALT_FUNC(PE4_I2C2SCL)}, // Pin 59
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda2[] =
{
     {BSP_GPIO_PORT_ID(PE5), BSP_GPIO_MASK(PE5), BSP_GPIO_ALT_FUNC(PE5_I2C2SDA)}  // Pin 60
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl3[] =
{
     {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_I2C3SCL)}, // Pin 61
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda3[] =
{
     {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_I2C3SDA)}  // Pin 62
};

const bsp_I2c_StaticInfo_t bsp_I2c_staticInfo[] =
{
   { I2C0_BASE, SYSCTL_PERIPH_I2C0, BSP_INTERRUPT_ID_I2C0,
     DIM(bsp_I2c_pinInfoTableScl0), bsp_I2c_pinInfoTableScl0,
     DIM(bsp_I2c_pinInfoTableSda0), bsp_I2c_pinInfoTableSda0,
   },
   { I2C1_BASE, SYSCTL_PERIPH_I2C1, BSP_INTERRUPT_ID_I2C1,
     DIM(bsp_I2c_pinInfoTableScl1), bsp_I2c_pinInfoTableScl1,
     DIM(bsp_I2c_pinInfoTableSda1), bsp_I2c_pinInfoTableSda1,
   },
   { I2C2_BASE, SYSCTL_PERIPH_I2C2, BSP_INTERRUPT_ID_I2C2,
     DIM(bsp_I2c_pinInfoTableScl2), bsp_I2c_pinInfoTableScl2,
     DIM(bsp_I2c_pinInfoTableSda2), bsp_I2c_pinInfoTableSda2,
   },
   { I2C3_BASE, SYSCTL_PERIPH_I2C3, BSP_INTERRUPT_ID_I2C3,
     DIM(bsp_I2c_pinInfoTableScl3), bsp_I2c_pinInfoTableScl3,
     DIM(bsp_I2c_pinInfoTableSda3), bsp_I2c_pinInfoTableSda3,
   }
};


/*==============================================================================
 *                              Public Functions
 *============================================================================*/
/*============================================================================*/
extern void bsp_I2c_isrCommon( bsp_I2c_Id_t id );

/*============================================================================*/
void
bsp_I2c_interruptHandler0( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID0 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C0 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler1( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID1 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C1 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler2( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID2 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C2 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler3( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID3 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C3 );
    return;
}
