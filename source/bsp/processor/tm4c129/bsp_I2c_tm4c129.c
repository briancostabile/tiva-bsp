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
 * @file bsp_I2c_tm4c129.c
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
  {BSP_GPIO_PORT_ID(PB2), BSP_GPIO_MASK(PB2), BSP_GPIO_ALT_FUNC(PB2_I2C0SCL)}, // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda0[] =
{
  {BSP_GPIO_PORT_ID(PB3), BSP_GPIO_MASK(PB3), BSP_GPIO_ALT_FUNC(PB3_I2C0SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl1[] =
{
  {BSP_GPIO_PORT_ID(PG0), BSP_GPIO_MASK(PG0), BSP_GPIO_ALT_FUNC(PG0_I2C1SCL)}, // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda1[] =
{
  {BSP_GPIO_PORT_ID(PG1), BSP_GPIO_MASK(PG1), BSP_GPIO_ALT_FUNC(PG1_I2C1SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl2[] =
{
  {BSP_GPIO_PORT_ID(PG2), BSP_GPIO_MASK(PG2), BSP_GPIO_ALT_FUNC(PG2_I2C2SCL)}, // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda2[] =
{
  {BSP_GPIO_PORT_ID(PG3), BSP_GPIO_MASK(PG3), BSP_GPIO_ALT_FUNC(PG3_I2C2SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl3[] =
{
  {BSP_GPIO_PORT_ID(PG4), BSP_GPIO_MASK(PG4), BSP_GPIO_ALT_FUNC(PG4_I2C3SCL)}, // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda3[] =
{
  {BSP_GPIO_PORT_ID(PG5), BSP_GPIO_MASK(PG5), BSP_GPIO_ALT_FUNC(PG5_I2C3SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl4[] =
{
  {BSP_GPIO_PORT_ID(PG6), BSP_GPIO_MASK(PG6), BSP_GPIO_ALT_FUNC(PG6_I2C4SCL)}, // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda4[] =
{
  {BSP_GPIO_PORT_ID(PG7), BSP_GPIO_MASK(PG7), BSP_GPIO_ALT_FUNC(PG7_I2C4SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl5[] =
{
  {BSP_GPIO_PORT_ID(PB0), BSP_GPIO_MASK(PB0), BSP_GPIO_ALT_FUNC(PB0_I2C5SCL)}, // Pin
  {BSP_GPIO_PORT_ID(PB4), BSP_GPIO_MASK(PB4), BSP_GPIO_ALT_FUNC(PB4_I2C5SCL)}  // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda5[] =
{
  {BSP_GPIO_PORT_ID(PB1), BSP_GPIO_MASK(PB1), BSP_GPIO_ALT_FUNC(PB1_I2C5SDA)}, // Pin
  {BSP_GPIO_PORT_ID(PB5), BSP_GPIO_MASK(PB5), BSP_GPIO_ALT_FUNC(PB5_I2C5SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl6[] =
{
  {BSP_GPIO_PORT_ID(PA6), BSP_GPIO_MASK(PA6), BSP_GPIO_ALT_FUNC(PA6_I2C6SCL)}, // Pin
  {BSP_GPIO_PORT_ID(PB6), BSP_GPIO_MASK(PB6), BSP_GPIO_ALT_FUNC(PB6_I2C6SCL)}  // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda6[] =
{
  {BSP_GPIO_PORT_ID(PA7), BSP_GPIO_MASK(PA7), BSP_GPIO_ALT_FUNC(PA7_I2C6SDA)}, // Pin
  {BSP_GPIO_PORT_ID(PB7), BSP_GPIO_MASK(PB7), BSP_GPIO_ALT_FUNC(PB7_I2C6SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl7[] =
{
  {BSP_GPIO_PORT_ID(PA4), BSP_GPIO_MASK(PA4), BSP_GPIO_ALT_FUNC(PA4_I2C7SCL)}, // Pin
  {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_I2C7SCL)}  // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda7[] =
{
  {BSP_GPIO_PORT_ID(PA5), BSP_GPIO_MASK(PA5), BSP_GPIO_ALT_FUNC(PA5_I2C7SDA)}, // Pin
  {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_I2C7SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl8[] =
{
  {BSP_GPIO_PORT_ID(PA2), BSP_GPIO_MASK(PA2), BSP_GPIO_ALT_FUNC(PA2_I2C8SCL)}, // Pin
  {BSP_GPIO_PORT_ID(PD2), BSP_GPIO_MASK(PD2), BSP_GPIO_ALT_FUNC(PD2_I2C8SCL)}  // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda8[] =
{
  {BSP_GPIO_PORT_ID(PA3), BSP_GPIO_MASK(PA3), BSP_GPIO_ALT_FUNC(PA3_I2C8SDA)}, // Pin
  {BSP_GPIO_PORT_ID(PD3), BSP_GPIO_MASK(PD3), BSP_GPIO_ALT_FUNC(PD3_I2C8SDA)}  // Pin
};

static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableScl9[] =
{
  {BSP_GPIO_PORT_ID(PA0), BSP_GPIO_MASK(PA0), BSP_GPIO_ALT_FUNC(PA0_I2C9SCL)}, // Pin
  {BSP_GPIO_PORT_ID(PE6), BSP_GPIO_MASK(PE6), BSP_GPIO_ALT_FUNC(PE6_I2C9SCL)}  // Pin
};
static const bsp_I2c_PinInfo_t bsp_I2c_pinInfoTableSda9[] =
{
  {BSP_GPIO_PORT_ID(PA1), BSP_GPIO_MASK(PA1), BSP_GPIO_ALT_FUNC(PA1_I2C9SDA)}, // Pin
  {BSP_GPIO_PORT_ID(PE7), BSP_GPIO_MASK(PE7), BSP_GPIO_ALT_FUNC(PE7_I2C9SDA)}  // Pin
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
   },
   { I2C4_BASE, SYSCTL_PERIPH_I2C4, BSP_INTERRUPT_ID_I2C4,
     DIM(bsp_I2c_pinInfoTableScl4), bsp_I2c_pinInfoTableScl4,
     DIM(bsp_I2c_pinInfoTableSda4), bsp_I2c_pinInfoTableSda4,
   },
   { I2C5_BASE, SYSCTL_PERIPH_I2C5, BSP_INTERRUPT_ID_I2C5,
     DIM(bsp_I2c_pinInfoTableScl5), bsp_I2c_pinInfoTableScl5,
     DIM(bsp_I2c_pinInfoTableSda5), bsp_I2c_pinInfoTableSda5,
   },
   { I2C6_BASE, SYSCTL_PERIPH_I2C6, BSP_INTERRUPT_ID_I2C6,
     DIM(bsp_I2c_pinInfoTableScl6), bsp_I2c_pinInfoTableScl6,
     DIM(bsp_I2c_pinInfoTableSda6), bsp_I2c_pinInfoTableSda6,
   },
   { I2C7_BASE, SYSCTL_PERIPH_I2C7, BSP_INTERRUPT_ID_I2C7,
     DIM(bsp_I2c_pinInfoTableScl7), bsp_I2c_pinInfoTableScl7,
     DIM(bsp_I2c_pinInfoTableSda7), bsp_I2c_pinInfoTableSda7,
   },
   { I2C8_BASE, SYSCTL_PERIPH_I2C8, BSP_INTERRUPT_ID_I2C8,
     DIM(bsp_I2c_pinInfoTableScl8), bsp_I2c_pinInfoTableScl8,
     DIM(bsp_I2c_pinInfoTableSda8), bsp_I2c_pinInfoTableSda8,
   },
   { I2C9_BASE, SYSCTL_PERIPH_I2C9, BSP_INTERRUPT_ID_I2C9,
     DIM(bsp_I2c_pinInfoTableScl9), bsp_I2c_pinInfoTableScl9,
     DIM(bsp_I2c_pinInfoTableSda9), bsp_I2c_pinInfoTableSda9,
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
