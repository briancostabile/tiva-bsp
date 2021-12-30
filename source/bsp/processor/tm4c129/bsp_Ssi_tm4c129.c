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
 * @file bsp_Ssi_tm4c129.c
 * @brief Contains processor specific SSI tables and functions
 */

#include "bsp_Gpio.h"
#include "bsp_Interrupt.h"
#include "bsp_Ssi_tm4c129.h"
#include <string.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/udma.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

/*==============================================================================
 *                              Global Data
 *============================================================================*/
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Clk[] =
{
  {BSP_GPIO_PORT_ID(PA2), BSP_GPIO_MASK(PA2), BSP_GPIO_ALT_FUNC(PA2_SSI0CLK)}, // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Fss[] =
{
  {BSP_GPIO_PORT_ID(PA3), BSP_GPIO_MASK(PA3), BSP_GPIO_ALT_FUNC(PA3_SSI0FSS)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat0[] =
{
  {BSP_GPIO_PORT_ID(PA4), BSP_GPIO_MASK(PA4), BSP_GPIO_ALT_FUNC(PA4_SSI0XDAT0)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat1[] =
{
  {BSP_GPIO_PORT_ID(PA5), BSP_GPIO_MASK(PA5), BSP_GPIO_ALT_FUNC(PA5_SSI0XDAT1)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat2[] =
{
  {BSP_GPIO_PORT_ID(PA6), BSP_GPIO_MASK(PA6), BSP_GPIO_ALT_FUNC(PA6_SSI0XDAT2)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat3[] =
{
  {BSP_GPIO_PORT_ID(PA7), BSP_GPIO_MASK(PA7), BSP_GPIO_ALT_FUNC(PA7_SSI0XDAT3)}  // Pin
};



static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Clk[] =
{
  {BSP_GPIO_PORT_ID(PB5), BSP_GPIO_MASK(PB5), BSP_GPIO_ALT_FUNC(PB5_SSI1CLK)}, // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Fss[] =
{
  {BSP_GPIO_PORT_ID(PB4), BSP_GPIO_MASK(PB4), BSP_GPIO_ALT_FUNC(PB4_SSI1FSS)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat0[] =
{
  {BSP_GPIO_PORT_ID(PE4), BSP_GPIO_MASK(PE4), BSP_GPIO_ALT_FUNC(PE4_SSI1XDAT0)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat1[] =
{
  {BSP_GPIO_PORT_ID(PE5), BSP_GPIO_MASK(PE5), BSP_GPIO_ALT_FUNC(PE5_SSI1XDAT1)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat2[] =
{
  {BSP_GPIO_PORT_ID(PD4), BSP_GPIO_MASK(PD4), BSP_GPIO_ALT_FUNC(PD4_SSI1XDAT2)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat3[] =
{
  {BSP_GPIO_PORT_ID(PD5), BSP_GPIO_MASK(PD5), BSP_GPIO_ALT_FUNC(PD5_SSI1XDAT3)}  // Pin
};


static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Clk[] =
{
  {BSP_GPIO_PORT_ID(PD3), BSP_GPIO_MASK(PD3), BSP_GPIO_ALT_FUNC(PD3_SSI2CLK)}, // Pin
  {BSP_GPIO_PORT_ID(PG7), BSP_GPIO_MASK(PG7), BSP_GPIO_ALT_FUNC(PG7_SSI2CLK)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Fss[] =
{
  {BSP_GPIO_PORT_ID(PD2), BSP_GPIO_MASK(PD2), BSP_GPIO_ALT_FUNC(PD2_SSI2FSS)}, // Pin
  {BSP_GPIO_PORT_ID(PG6), BSP_GPIO_MASK(PG6), BSP_GPIO_ALT_FUNC(PG6_SSI2FSS)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat0[] =
{
  {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_SSI2XDAT0)}, // Pin
  {BSP_GPIO_PORT_ID(PG5), BSP_GPIO_MASK(PG5), BSP_GPIO_ALT_FUNC(PG5_SSI2XDAT0)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat1[] =
{
  {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_SSI2XDAT1)}, // Pin
  {BSP_GPIO_PORT_ID(PG4), BSP_GPIO_MASK(PG4), BSP_GPIO_ALT_FUNC(PG4_SSI2XDAT1)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat2[] =
{
  {BSP_GPIO_PORT_ID(PD7), BSP_GPIO_MASK(PD7), BSP_GPIO_ALT_FUNC(PD7_SSI2XDAT2)}, // Pin
  {BSP_GPIO_PORT_ID(PG3), BSP_GPIO_MASK(PG3), BSP_GPIO_ALT_FUNC(PG3_SSI2XDAT2)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat3[] =
{
  {BSP_GPIO_PORT_ID(PD6), BSP_GPIO_MASK(PD6), BSP_GPIO_ALT_FUNC(PD6_SSI2XDAT3)}, // Pin
  {BSP_GPIO_PORT_ID(PG2), BSP_GPIO_MASK(PG2), BSP_GPIO_ALT_FUNC(PG2_SSI2XDAT3)}  // Pin
};



static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Clk[] =
{
  {BSP_GPIO_PORT_ID(PF3), BSP_GPIO_MASK(PF3), BSP_GPIO_ALT_FUNC(PF3_SSI3CLK)}, // Pin
  {BSP_GPIO_PORT_ID(PQ0), BSP_GPIO_MASK(PQ0), BSP_GPIO_ALT_FUNC(PQ0_SSI3CLK)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Fss[] =
{
  {BSP_GPIO_PORT_ID(PF2), BSP_GPIO_MASK(PF2), BSP_GPIO_ALT_FUNC(PF2_SSI3FSS)},  // Pin
  {BSP_GPIO_PORT_ID(PQ1), BSP_GPIO_MASK(PQ1), BSP_GPIO_ALT_FUNC(PQ1_SSI3FSS)}   // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat0[] =
{
  {BSP_GPIO_PORT_ID(PF1), BSP_GPIO_MASK(PF1), BSP_GPIO_ALT_FUNC(PF1_SSI3XDAT0)}, // Pin
  {BSP_GPIO_PORT_ID(PQ2), BSP_GPIO_MASK(PQ2), BSP_GPIO_ALT_FUNC(PQ2_SSI3XDAT0)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat1[] =
{
  {BSP_GPIO_PORT_ID(PF0), BSP_GPIO_MASK(PF0), BSP_GPIO_ALT_FUNC(PF0_SSI3XDAT1)}, // Pin
  {BSP_GPIO_PORT_ID(PQ3), BSP_GPIO_MASK(PQ3), BSP_GPIO_ALT_FUNC(PQ3_SSI3XDAT1)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat2[] =
{
  {BSP_GPIO_PORT_ID(PF4), BSP_GPIO_MASK(PF4), BSP_GPIO_ALT_FUNC(PF4_SSI3XDAT2)}, // Pin
  {BSP_GPIO_PORT_ID(PP0), BSP_GPIO_MASK(PP0), BSP_GPIO_ALT_FUNC(PP0_SSI3XDAT2)}  // Pin
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat3[] =
{
  {BSP_GPIO_PORT_ID(PF5), BSP_GPIO_MASK(PF5), BSP_GPIO_ALT_FUNC(PF5_SSI3XDAT3)}, // Pin
  {BSP_GPIO_PORT_ID(PP1), BSP_GPIO_MASK(PP1), BSP_GPIO_ALT_FUNC(PP1_SSI3XDAT3)}  // Pin
};


const bsp_Ssi_StaticInfo_t bsp_Ssi_staticInfo[] =
{
   { SSI0_BASE, SYSCTL_PERIPH_SSI0, BSP_INTERRUPT_ID_SSI0, UDMA_CH10_SSI0RX, UDMA_CH11_SSI0TX,
     DIM(bsp_Ssi_pinInfoTableSsi0Clk), bsp_Ssi_pinInfoTableSsi0Clk,
     DIM(bsp_Ssi_pinInfoTableSsi0Fss), bsp_Ssi_pinInfoTableSsi0Fss,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat0), bsp_Ssi_pinInfoTableSsi0Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat1), bsp_Ssi_pinInfoTableSsi0Dat1,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat2), bsp_Ssi_pinInfoTableSsi0Dat2,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat3), bsp_Ssi_pinInfoTableSsi0Dat3
   },
   { SSI1_BASE, SYSCTL_PERIPH_SSI1, BSP_INTERRUPT_ID_SSI1, UDMA_CH24_SSI1RX, UDMA_CH25_SSI1TX,
     DIM(bsp_Ssi_pinInfoTableSsi1Clk), bsp_Ssi_pinInfoTableSsi1Clk,
     DIM(bsp_Ssi_pinInfoTableSsi1Fss), bsp_Ssi_pinInfoTableSsi1Fss,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat0), bsp_Ssi_pinInfoTableSsi1Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat1), bsp_Ssi_pinInfoTableSsi1Dat1,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat2), bsp_Ssi_pinInfoTableSsi1Dat2,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat3), bsp_Ssi_pinInfoTableSsi1Dat3
   },
   { SSI2_BASE, SYSCTL_PERIPH_SSI2, BSP_INTERRUPT_ID_SSI2, UDMA_CH12_SSI2RX, UDMA_CH13_SSI2TX,
     DIM(bsp_Ssi_pinInfoTableSsi2Clk), bsp_Ssi_pinInfoTableSsi2Clk,
     DIM(bsp_Ssi_pinInfoTableSsi2Fss), bsp_Ssi_pinInfoTableSsi2Fss,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat0), bsp_Ssi_pinInfoTableSsi2Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat1), bsp_Ssi_pinInfoTableSsi2Dat1,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat2), bsp_Ssi_pinInfoTableSsi2Dat2,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat3), bsp_Ssi_pinInfoTableSsi2Dat3
   },
   { SSI3_BASE, SYSCTL_PERIPH_SSI3, BSP_INTERRUPT_ID_SSI3, UDMA_CH14_SSI3RX, UDMA_CH15_SSI3TX,
     DIM(bsp_Ssi_pinInfoTableSsi3Clk), bsp_Ssi_pinInfoTableSsi3Clk,
     DIM(bsp_Ssi_pinInfoTableSsi3Fss), bsp_Ssi_pinInfoTableSsi3Fss,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat0), bsp_Ssi_pinInfoTableSsi3Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat1), bsp_Ssi_pinInfoTableSsi3Dat1,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat2), bsp_Ssi_pinInfoTableSsi3Dat2,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat3), bsp_Ssi_pinInfoTableSsi3Dat3
   }
};



/*==============================================================================
 *                              Public Functions
 *============================================================================*/
/*============================================================================*/
extern void bsp_Ssi_isrCommon( bsp_Ssi_Id_t id );

/*============================================================================*/
void
bsp_Ssi_interruptHandler0( void )
{
    bsp_Ssi_isrCommon( BSP_SSI_ID0 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_SSI0 );
    return;
}


/*============================================================================*/
void
bsp_Ssi_interruptHandler1( void )
{
    bsp_Ssi_isrCommon( BSP_SSI_ID1 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_SSI1 );
    return;
}


/*============================================================================*/
void
bsp_Ssi_interruptHandler2( void )
{
    bsp_Ssi_isrCommon( BSP_SSI_ID2 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_SSI2 );
    return;
}


/*============================================================================*/
void
bsp_Ssi_interruptHandler3( void )
{
    bsp_Ssi_isrCommon( BSP_SSI_ID3 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_SSI3 );
    return;
}
