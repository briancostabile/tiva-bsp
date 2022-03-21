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
 * @file bsp_Uart_tm4c123.c
 * @brief Contains processor specific UART tables and functions
 */

#include "bsp_Gpio.h"
#include "bsp_Uart.h"
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
/*============================================================================*/
/*** UART 0 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx0[] = {
    {BSP_GPIO_PORT_ID(PA0), BSP_GPIO_MASK(PA0), BSP_GPIO_ALT_FUNC(PA0_U0RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx0[] = {
    {BSP_GPIO_PORT_ID(PA1), BSP_GPIO_MASK(PA1), BSP_GPIO_ALT_FUNC(PA1_U0TX)}};

/*** UART 1 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx1[] = {
    {BSP_GPIO_PORT_ID(PB0), BSP_GPIO_MASK(PB0), BSP_GPIO_ALT_FUNC(PB0_U1RX)},
    {BSP_GPIO_PORT_ID(PC4), BSP_GPIO_MASK(PC4), BSP_GPIO_ALT_FUNC(PC4_U1RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx1[] = {
    {BSP_GPIO_PORT_ID(PB1), BSP_GPIO_MASK(PB1), BSP_GPIO_ALT_FUNC(PB1_U1TX)},
    {BSP_GPIO_PORT_ID(PC5), BSP_GPIO_MASK(PC5), BSP_GPIO_ALT_FUNC(PC5_U1TX)}};

/*** UART 2 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx2[] = {
    {BSP_GPIO_PORT_ID(PD6), BSP_GPIO_MASK(PD6), BSP_GPIO_ALT_FUNC(PD6_U2RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx2[] = {
    {BSP_GPIO_PORT_ID(PD7), BSP_GPIO_MASK(PD7), BSP_GPIO_ALT_FUNC(PD7_U2TX)}};

/*** UART 3 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx3[] = {
    {BSP_GPIO_PORT_ID(PC6), BSP_GPIO_MASK(PC6), BSP_GPIO_ALT_FUNC(PC6_U3RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx3[] = {
    {BSP_GPIO_PORT_ID(PC7), BSP_GPIO_MASK(PC7), BSP_GPIO_ALT_FUNC(PC7_U3TX)}};

/*** UART 4 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx4[] = {
    {BSP_GPIO_PORT_ID(PC4), BSP_GPIO_MASK(PC4), BSP_GPIO_ALT_FUNC(PC4_U4RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx4[] = {
    {BSP_GPIO_PORT_ID(PC5), BSP_GPIO_MASK(PC5), BSP_GPIO_ALT_FUNC(PC5_U4TX)}};

/*** UART 5 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx5[] = {
    {BSP_GPIO_PORT_ID(PE4), BSP_GPIO_MASK(PE4), BSP_GPIO_ALT_FUNC(PE4_U5RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx5[] = {
    {BSP_GPIO_PORT_ID(PE5), BSP_GPIO_MASK(PE5), BSP_GPIO_ALT_FUNC(PE5_U5TX)}};

/*** UART 6 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx6[] = {
    {BSP_GPIO_PORT_ID(PD4), BSP_GPIO_MASK(PD4), BSP_GPIO_ALT_FUNC(PD4_U6RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx6[] = {
    {BSP_GPIO_PORT_ID(PD5), BSP_GPIO_MASK(PD5), BSP_GPIO_ALT_FUNC(PD5_U6TX)}};

/*** UART 7 Pin Options */
const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableRx7[] = {
    {BSP_GPIO_PORT_ID(PE0), BSP_GPIO_MASK(PE0), BSP_GPIO_ALT_FUNC(PE0_U7RX)}};

const bsp_Uart_PinInfo_t bsp_Uart_pinInfoTableTx7[] = {
    {BSP_GPIO_PORT_ID(PE1), BSP_GPIO_MASK(PE1), BSP_GPIO_ALT_FUNC(PE1_U7TX)}};

/* clang-format off */
const bsp_Uart_StaticInfo_t bsp_Uart_staticInfoTable[BSP_UART_PLATFORM_NUM_UARTS] = {
    { UART0_BASE, SYSCTL_PERIPH_UART0, BSP_INTERRUPT_ID_UART0,
      DIM(bsp_Uart_pinInfoTableRx0), bsp_Uart_pinInfoTableRx0,
      DIM(bsp_Uart_pinInfoTableTx0), bsp_Uart_pinInfoTableTx0,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL },
    { UART1_BASE, SYSCTL_PERIPH_UART1, BSP_INTERRUPT_ID_UART1,
      DIM(bsp_Uart_pinInfoTableRx1), bsp_Uart_pinInfoTableRx1,
      DIM(bsp_Uart_pinInfoTableTx1), bsp_Uart_pinInfoTableTx1,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL },
    { UART2_BASE, SYSCTL_PERIPH_UART2, BSP_INTERRUPT_ID_UART2,
      DIM(bsp_Uart_pinInfoTableRx2), bsp_Uart_pinInfoTableRx2,
      DIM(bsp_Uart_pinInfoTableTx2), bsp_Uart_pinInfoTableTx2,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  },
    { UART3_BASE, SYSCTL_PERIPH_UART3, BSP_INTERRUPT_ID_UART3,
      DIM(bsp_Uart_pinInfoTableRx3), bsp_Uart_pinInfoTableRx3,
      DIM(bsp_Uart_pinInfoTableTx3), bsp_Uart_pinInfoTableTx3,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  },
    { UART4_BASE, SYSCTL_PERIPH_UART4, BSP_INTERRUPT_ID_UART4,
      DIM(bsp_Uart_pinInfoTableRx4), bsp_Uart_pinInfoTableRx4,
      DIM(bsp_Uart_pinInfoTableTx4), bsp_Uart_pinInfoTableTx4,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  },
    { UART5_BASE, SYSCTL_PERIPH_UART5, BSP_INTERRUPT_ID_UART5,
      DIM(bsp_Uart_pinInfoTableRx5), bsp_Uart_pinInfoTableRx5,
      DIM(bsp_Uart_pinInfoTableTx5), bsp_Uart_pinInfoTableTx5,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  },
    { UART6_BASE, SYSCTL_PERIPH_UART6, BSP_INTERRUPT_ID_UART6,
      DIM(bsp_Uart_pinInfoTableRx6), bsp_Uart_pinInfoTableRx6,
      DIM(bsp_Uart_pinInfoTableTx6), bsp_Uart_pinInfoTableTx6,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  },
    { UART7_BASE, SYSCTL_PERIPH_UART7, BSP_INTERRUPT_ID_UART7,
      DIM(bsp_Uart_pinInfoTableRx7), bsp_Uart_pinInfoTableRx7,
      DIM(bsp_Uart_pinInfoTableTx7), bsp_Uart_pinInfoTableTx7,
      0, NULL,0, NULL,0, NULL,0, NULL,0, NULL, 0, NULL  }
};
/* clang-format on */

/*==============================================================================
 *                              Public Functions
 *============================================================================*/
/*============================================================================*/
extern void bsp_Uart_isrCommon(bsp_Uart_Id_t id);

/*============================================================================*/
void bsp_Uart_interruptHandler0(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID0);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART0);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler1(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID1);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART1);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler2(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID2);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART2);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler3(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID3);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART3);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler4(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID4);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART4);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler5(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID5);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART5);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler6(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID6);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART6);
    return;
}

/*============================================================================*/
void bsp_Uart_interruptHandler7(void)
{
    bsp_Uart_isrCommon(BSP_UART_ID7);
    bsp_Interrupt_clearPending(BSP_INTERRUPT_ID_UART7);
    return;
}
