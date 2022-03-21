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
 * @file bsp_Ssi_tm4c123.c
 * @brief Contains processor specific SSI tables and functions
 */

#include "bsp_Gpio.h"
#include "bsp_Interrupt.h"
#include "bsp_Ssi_tm4c123.h"
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
/* clang-format off */
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Clk[] = {
    {BSP_GPIO_PORT_ID(PA2), BSP_GPIO_MASK(PA2), BSP_GPIO_ALT_FUNC(PA2_SSI0CLK)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Fss[] = {
    {BSP_GPIO_PORT_ID(PA3), BSP_GPIO_MASK(PA3), BSP_GPIO_ALT_FUNC(PA3_SSI0FSS)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat0[] = {
    {BSP_GPIO_PORT_ID(PA4), BSP_GPIO_MASK(PA4), BSP_GPIO_ALT_FUNC(PA4_SSI0RX)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi0Dat1[] = {
    {BSP_GPIO_PORT_ID(PA5), BSP_GPIO_MASK(PA5), BSP_GPIO_ALT_FUNC(PA5_SSI0TX)}
};

static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Clk[] = {
    {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_SSI1CLK)},
    {BSP_GPIO_PORT_ID(PF2), BSP_GPIO_MASK(PF2), BSP_GPIO_ALT_FUNC(PF2_SSI1CLK)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Fss[] = {
    {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_SSI1FSS)},
    {BSP_GPIO_PORT_ID(PF3), BSP_GPIO_MASK(PF3), BSP_GPIO_ALT_FUNC(PF3_SSI1FSS)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat0[] = {
    {BSP_GPIO_PORT_ID(PD2), BSP_GPIO_MASK(PD2), BSP_GPIO_ALT_FUNC(PD2_SSI1RX)},
    {BSP_GPIO_PORT_ID(PF0), BSP_GPIO_MASK(PF0), BSP_GPIO_ALT_FUNC(PF0_SSI1RX)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi1Dat1[] = {
    {BSP_GPIO_PORT_ID(PD3), BSP_GPIO_MASK(PD3), BSP_GPIO_ALT_FUNC(PD3_SSI1TX)},
    {BSP_GPIO_PORT_ID(PF1), BSP_GPIO_MASK(PF1), BSP_GPIO_ALT_FUNC(PF1_SSI1TX)}
};

static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Clk[] = {
    {BSP_GPIO_PORT_ID(PB4), BSP_GPIO_MASK(PB4), BSP_GPIO_ALT_FUNC(PB4_SSI2CLK)},
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Fss[] = {
    {BSP_GPIO_PORT_ID(PB5), BSP_GPIO_MASK(PB5), BSP_GPIO_ALT_FUNC(PB5_SSI2FSS)},
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat0[] = {
    {BSP_GPIO_PORT_ID(PB6), BSP_GPIO_MASK(PB6), BSP_GPIO_ALT_FUNC(PB6_SSI2RX)},
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi2Dat1[] = {
    {BSP_GPIO_PORT_ID(PB7), BSP_GPIO_MASK(PB7), BSP_GPIO_ALT_FUNC(PB7_SSI2TX)},
};

static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Clk[] = {
    {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_SSI3CLK)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Fss[] = {
    {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_SSI3FSS)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat0[] = {
    {BSP_GPIO_PORT_ID(PD2), BSP_GPIO_MASK(PD2), BSP_GPIO_ALT_FUNC(PD2_SSI3RX)}
};
static const bsp_Ssi_PinInfo_t bsp_Ssi_pinInfoTableSsi3Dat1[] = {
    {BSP_GPIO_PORT_ID(PD3), BSP_GPIO_MASK(PD3), BSP_GPIO_ALT_FUNC(PD3_SSI3TX)}
};
/* clang-format on */

const bsp_Ssi_StaticInfo_t bsp_Ssi_staticInfo[] = {
    {SSI0_BASE,
     SYSCTL_PERIPH_SSI0,
     BSP_INTERRUPT_ID_SSI0,
     UDMA_CH10_SSI0RX,
     UDMA_CH11_SSI0TX,
     DIM(bsp_Ssi_pinInfoTableSsi0Clk),
     bsp_Ssi_pinInfoTableSsi0Clk,
     DIM(bsp_Ssi_pinInfoTableSsi0Fss),
     bsp_Ssi_pinInfoTableSsi0Fss,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat0),
     bsp_Ssi_pinInfoTableSsi0Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi0Dat1),
     bsp_Ssi_pinInfoTableSsi0Dat1,
     0,
     NULL,
     0,
     NULL},
    {SSI1_BASE,
     SYSCTL_PERIPH_SSI1,
     BSP_INTERRUPT_ID_SSI1,
     UDMA_CH24_SSI1RX,
     UDMA_CH25_SSI1TX,
     DIM(bsp_Ssi_pinInfoTableSsi1Clk),
     bsp_Ssi_pinInfoTableSsi1Clk,
     DIM(bsp_Ssi_pinInfoTableSsi1Fss),
     bsp_Ssi_pinInfoTableSsi1Fss,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat0),
     bsp_Ssi_pinInfoTableSsi1Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi1Dat1),
     bsp_Ssi_pinInfoTableSsi1Dat1,
     0,
     NULL,
     0,
     NULL},
    {SSI2_BASE,
     SYSCTL_PERIPH_SSI2,
     BSP_INTERRUPT_ID_SSI2,
     UDMA_CH12_SSI2RX,
     UDMA_CH13_SSI2TX,
     DIM(bsp_Ssi_pinInfoTableSsi2Clk),
     bsp_Ssi_pinInfoTableSsi2Clk,
     DIM(bsp_Ssi_pinInfoTableSsi2Fss),
     bsp_Ssi_pinInfoTableSsi2Fss,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat0),
     bsp_Ssi_pinInfoTableSsi2Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi2Dat1),
     bsp_Ssi_pinInfoTableSsi2Dat1,
     0,
     NULL,
     0,
     NULL},
    {SSI3_BASE,
     SYSCTL_PERIPH_SSI3,
     BSP_INTERRUPT_ID_SSI3,
     UDMA_CH14_SSI3RX,
     UDMA_CH15_SSI3TX,
     DIM(bsp_Ssi_pinInfoTableSsi3Clk),
     bsp_Ssi_pinInfoTableSsi3Clk,
     DIM(bsp_Ssi_pinInfoTableSsi3Fss),
     bsp_Ssi_pinInfoTableSsi3Fss,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat0),
     bsp_Ssi_pinInfoTableSsi3Dat0,
     DIM(bsp_Ssi_pinInfoTableSsi3Dat1),
     bsp_Ssi_pinInfoTableSsi3Dat1,
     0,
     NULL,
     0,
     NULL}};

/*==============================================================================
 *                              Public Functions
 *============================================================================*/
/*============================================================================*/
extern void bsp_Ssi_isrCommon(bsp_Ssi_Id_t id);

/*============================================================================*/
void bsp_Ssi_interruptHandler0(void)
{
    bsp_Ssi_isrCommon(BSP_SSI_ID0);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_SSI0);
    return;
}

/*============================================================================*/
void bsp_Ssi_interruptHandler1(void)
{
    bsp_Ssi_isrCommon(BSP_SSI_ID1);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_SSI1);
    return;
}

/*============================================================================*/
void bsp_Ssi_interruptHandler2(void)
{
    bsp_Ssi_isrCommon(BSP_SSI_ID2);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_SSI2);
    return;
}

/*============================================================================*/
void bsp_Ssi_interruptHandler3(void)
{
    bsp_Ssi_isrCommon(BSP_SSI_ID3);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_SSI3);
    return;
}
