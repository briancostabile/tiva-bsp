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
 * @file bsp_Interrupt_tm4c129.c
 * @brief Contains Macros and defines for the interrupt controller specific
 *        to the tm4c129 with SensorHub platform.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Interrupt.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"
#include "bsp_Reset.h"
#include "bsp_Gpio.h"
#include "bsp_I2c.h"
#include "bsp_Uart.h"
#include "bsp_Usb.h"
#include "bsp_TimerGp.h"


/*==============================================================================
 *                               Defines
 *============================================================================*/
#define BSP_INTERRUPT_HANDLER_NULL ((bsp_Interrupt_VectorHandler_t)NULL)

/*==============================================================================
 *                                Macros
 *============================================================================*/


/*==============================================================================
 * Forward declaration of the default fault handlers.
 */
static void bsp_Interrupt_rstHandler(void);
static void bsp_Interrupt_nmiHandler(void);
static void bsp_Interrupt_faultHandler(void);
static void bsp_Interrupt_defaultHandler(void);

void __attribute__((weak)) bsp_Interrupt_sysTickHandler(void)
{
    bsp_Interrupt_defaultHandler();
    return;
}

void __attribute__((weak)) bsp_Interrupt_svCallHandler(void)
{
    bsp_Interrupt_defaultHandler();
    return;
}

void __attribute__((weak)) bsp_Interrupt_pendSvCallHandler(void)
{
    bsp_Interrupt_defaultHandler();
    return;
}


//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************

/* 16 Exception Vectors */
extern uint32_t __STACK_TOP;
const bsp_Interrupt_VectorHandler_t BSP_ATTR_SECTION(".vecsException") BSP_ATTR_USED bsp_Interrupt_vectorTableExceptions[] =
{
    (bsp_Interrupt_VectorHandler_t)&(__STACK_TOP),
    bsp_Interrupt_rstHandler,                 // The reset handler
    bsp_Interrupt_nmiHandler,                 // The NMI handler
    bsp_Interrupt_faultHandler,               // The hard fault handler
    bsp_Interrupt_faultHandler,               // The MPU fault handler
    bsp_Interrupt_faultHandler,               // The bus fault handler
    bsp_Interrupt_faultHandler,               // The usage fault handler
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_svCallHandler,              // SVCall handler
    bsp_Interrupt_defaultHandler,             // Debug monitor handler
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_pendSvCallHandler,          // The PendSV handler
    bsp_Interrupt_sysTickHandler,             // The SysTick handler
};
BSP_PRAGMA_DATA_REQUIRED(bsp_Interrupt_vectorTableExceptions)

/* 139 Interrupt Vectors */
const bsp_Interrupt_VectorHandler_t BSP_ATTR_SECTION(".vecsInterrupt") BSP_ATTR_USED bsp_Interrupt_vectorTableInterrutps[] =
{
    bsp_Gpio_interruptHandlerPortA,           // GPIO Port A
    bsp_Gpio_interruptHandlerPortB,           // GPIO Port B
    bsp_Gpio_interruptHandlerPortC,           // GPIO Port C
    bsp_Gpio_interruptHandlerPortD,           // GPIO Port D
    bsp_Gpio_interruptHandlerPortE,           // GPIO Port E
    bsp_Uart_interruptHandler0,               // UART0 Rx and Tx
    bsp_Uart_interruptHandler1,               // UART1 Rx and Tx
    bsp_Interrupt_defaultHandler,             // SSI0 Rx and Tx
    bsp_I2c_interruptHandler0,                // I2C0 Master and Slave
    bsp_Interrupt_defaultHandler,             // PWM Fault
    bsp_Interrupt_defaultHandler,             // PWM Generator 0
    bsp_Interrupt_defaultHandler,             // PWM Generator 1
    bsp_Interrupt_defaultHandler,             // PWM Generator 2
    bsp_Interrupt_defaultHandler,             // Quadrature Encoder 0
    bsp_Interrupt_defaultHandler,             // ADC Sequence 0
    bsp_Interrupt_defaultHandler,             // ADC Sequence 1
    bsp_Interrupt_defaultHandler,             // ADC Sequence 2
    bsp_Interrupt_defaultHandler,             // ADC Sequence 3
    bsp_Interrupt_defaultHandler,             // Watchdog timer
    bsp_TimerGp_interruptHandlerTimer0A,      // Timer 0 subtimer A
    bsp_TimerGp_interruptHandlerTimer0B,      // Timer 0 subtimer B
    bsp_TimerGp_interruptHandlerTimer1A,      // Timer 1 subtimer A
    bsp_TimerGp_interruptHandlerTimer1B,      // Timer 1 subtimer B
    bsp_TimerGp_interruptHandlerTimer2A,      // Timer 2 subtimer A
    bsp_TimerGp_interruptHandlerTimer2B,      // Timer 2 subtimer B
    bsp_Interrupt_defaultHandler,             // Analog Comparator 0
    bsp_Interrupt_defaultHandler,             // Analog Comparator 1
    bsp_Interrupt_defaultHandler,             // Analog Comparator 2
    bsp_Interrupt_defaultHandler,             // System Control (PLL, OSC, BO)
    bsp_Interrupt_defaultHandler,             // FLASH Control
    bsp_Gpio_interruptHandlerPortF,           // GPIO Port F
    bsp_Gpio_interruptHandlerPortG,           // GPIO Port G
    bsp_Gpio_interruptHandlerPortH,           // GPIO Port H
    bsp_Uart_interruptHandler2,               // UART2 Rx and Tx
    bsp_Interrupt_defaultHandler,             // SSI1 Rx and Tx
    bsp_TimerGp_interruptHandlerTimer3A,      // Timer 3 subtimer A
    bsp_TimerGp_interruptHandlerTimer3B,      // Timer 3 subtimer B
    bsp_I2c_interruptHandler1,                // I2C1 Master and Slave
    bsp_Interrupt_defaultHandler,             // CAN0
    bsp_Interrupt_defaultHandler,             // CAN1
    bsp_Interrupt_defaultHandler,             // ETH0
    bsp_Interrupt_defaultHandler,             // Hibernate
    bsp_Usb_interruptHandler,                 // USB0
    bsp_Interrupt_defaultHandler,             // PWM Generator 3
    bsp_Interrupt_defaultHandler,             // uDMA Software Transfer
    bsp_Interrupt_defaultHandler,             // uDMA Error
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 0
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 1
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 2
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 3
    bsp_Interrupt_defaultHandler,             // EPI0
    bsp_Gpio_interruptHandlerPortJ,           // GPIO Port J
    bsp_Gpio_interruptHandlerPortK,           // GPIO Port K
    bsp_Gpio_interruptHandlerPortL,           // GPIO Port L
    bsp_Interrupt_defaultHandler,             // SSI2 Rx and Tx
    bsp_Interrupt_defaultHandler,             // SSI3 Rx and Tx
    bsp_Uart_interruptHandler3,               // UART3 Rx and Tx
    bsp_Uart_interruptHandler4,               // UART4 Rx and Tx
    bsp_Uart_interruptHandler5,               // UART5 Rx and Tx
    bsp_Uart_interruptHandler6,               // UART6 Rx and Tx
    bsp_Uart_interruptHandler7,               // UART7 Rx and Tx
    bsp_I2c_interruptHandler2,                // I2C2 Master and Slave
    bsp_I2c_interruptHandler3,                // I2C3 Master and Slave
    bsp_TimerGp_interruptHandlerTimer4A,      // Timer 4 subtimer A
    bsp_TimerGp_interruptHandlerTimer4B,      // Timer 4 subtimer B
    bsp_TimerGp_interruptHandlerTimer5A,      // Timer 5 subtimer A
    bsp_TimerGp_interruptHandlerTimer5B,      // Timer 5 subtimer B
    bsp_Interrupt_defaultHandler,             // FPU
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_I2c_interruptHandler4,                // I2C4 Master and Slave
    bsp_I2c_interruptHandler5,                // I2C5 Master and Slave
    bsp_Gpio_interruptHandlerPortM,           // GPIO Port M
    bsp_Gpio_interruptHandlerPortN,           // GPIO Port N
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // Tamper
    bsp_Gpio_interruptHandlerPortP0,          // GPIO Port P (Summary or P0)
    bsp_Gpio_interruptHandlerPortP1,          // GPIO Port P1
    bsp_Gpio_interruptHandlerPortP2,          // GPIO Port P2
    bsp_Gpio_interruptHandlerPortP3,          // GPIO Port P3
    bsp_Gpio_interruptHandlerPortP4,          // GPIO Port P4
    bsp_Gpio_interruptHandlerPortP5,          // GPIO Port P5
    bsp_Gpio_interruptHandlerPortP6,          // GPIO Port P6
    bsp_Gpio_interruptHandlerPortP7,          // GPIO Port P7
    bsp_Gpio_interruptHandlerPortQ0,          // GPIO Port Q (Summary or Q0)
    bsp_Gpio_interruptHandlerPortQ1,          // GPIO Port Q1
    bsp_Gpio_interruptHandlerPortQ2,          // GPIO Port Q2
    bsp_Gpio_interruptHandlerPortQ3,          // GPIO Port Q3
    bsp_Gpio_interruptHandlerPortQ4,          // GPIO Port Q4
    bsp_Gpio_interruptHandlerPortQ5,          // GPIO Port Q5
    bsp_Gpio_interruptHandlerPortQ6,          // GPIO Port Q6
    bsp_Gpio_interruptHandlerPortQ7,          // GPIO Port Q7
    bsp_Gpio_interruptHandlerPortR,           // GPIO Port R
    bsp_Gpio_interruptHandlerPortS,           // GPIO Port S
    bsp_Interrupt_defaultHandler,             // SHA/MD5
    bsp_Interrupt_defaultHandler,             // AES
    bsp_Interrupt_defaultHandler,             // DES
    bsp_Interrupt_defaultHandler,             // LCD
    bsp_Interrupt_defaultHandler,             // Wide Timer 6 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 6 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 7 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 7 subtimer B
    bsp_I2c_interruptHandler6,                // I2C6 Master and Slave
    bsp_I2c_interruptHandler7,                // I2C7 Master and Slave
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // 1-wire
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_I2c_interruptHandler8,                // I2C8 Master and Slave
    bsp_I2c_interruptHandler9,                // I2C9 Master and Slave
    bsp_Gpio_interruptHandlerPortT,           // GPIO Port T
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL                // Reserved
};
BSP_PRAGMA_DATA_REQUIRED(bsp_Interrupt_vectorTableInterrutps)

/* Table with all of the priority settings for each system exception */
const uint32_t bsp_Interrupt_groupPriorityTableExceptions[] =
{
    // 4-7: Memory Management, Bus Fault, Usage Fault, Reserved
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 8-11: Reserved, Reserved, Reserved, SVC Call
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 12-15: Debug Monitor, Reserved, Pending SV, SYS Tick
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) )
};


/* Table with all of the priority settings for each interrupt */
const uint32_t bsp_Interrupt_groupPriorityTableInterrupts[] =
{
    // 0-3: GPIO Port A, GPIO Port B, GPIO Port C, GPIO Port D
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 4-7: GPIO Port E, UART0 Rx and Tx, UART1 Rx and Tx, SSI0 Rx and Tx
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 8-11: I2C0 Master and Slave, PWM Fault, PWM Generator 0, PWM Generator 1
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 12-15: PWM Generator 2, Quadrature Encoder 0, ADC Sequence 0, ADC Sequence 1
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 16-19: ADC Sequence 2, ADC Sequence 3, Watchdog timer, Timer 0 subtimer A
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 20-23: Timer 0 subtimer B, Timer 1 subtimer A, Timer 1 subtimer B, Timer 2 subtimer A
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 24-27: Timer 2 subtimer B, Analog Comparator 0, Analog Comparator 1, Analog Comparator 2
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 28-31: System Control (PLL, OSC, BO), FLASH Control, GPIO Port F, GPIO Port G
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 32-35: GPIO Port H, UART2 Rx and Tx, SSI1 Rx and Tx, Timer 3 subtimer A
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 36-39: Timer 3 subtimer B, I2C1 Master and Slave, CAN0, CAN1
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 40-43: Ethernet MAC, Hibernate, USB0 MAC, PWM Generator 3
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 44-47: uDMA Software Transfer, uDMA Error, ADC1 Sequence 0, ADC1 Sequence 1
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 48-51: ADC1 Sequence 2 , ADC1 Sequence 3, EPI 0, GPIO Port J
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 52-55: GPIO Port K, GPIO Port L, SSI2 Rx and Tx, SSI3 Rx and Tx
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 56-59: UART3 Rx and Tx, UART4 Rx and Tx, UART5 Rx and Tx, UART6 Rx and Tx
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 60-63: UART7 Rx and Tx, I2C2 Master and Slave, I2C3 Master and Slave, Timer 4 subtimer A
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 64-67: Timer 4 subtimer B, Timer 5 subtimer A, Timer 5 subtimer B, FPU Exception
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 68-71: Reserved, Reserved, I2C4 Master and Slave, I2C5 Master and Slave
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 72-75: GPIO Port M, GPIO Port N, Reserved, Tamper
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 76-79: GPIO Port P (Summary or P0), GPIO Port P1, GPIO Port P2, GPIO Port P3
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 80-83: GPIO Port P4, GPIO Port P5, GPIO Port P6, GPIO Port P7
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 84-87: GPIO Port Q (Summary or Q0), GPIO Port Q1, GPIO Port Q2, GPIO Port Q3
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 88-91: GPIO Port Q4, GPIO Port Q5, GPIO Port Q6, GPIO Port Q7
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 92-95: GPIO Port R, GPIO Port S, SHA/MD5, AES
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 96-99: DES, LCD, Wide Timer 6 subtimer A, Wide Timer 6 subtimer B
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 100-103: Wide Timer 7 subtimer A, Wide Timer 7 subtimer B, I2C6 Master and Slave, I2C7 Master and Slave
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 104-107: Reserved, 1Wire, Reserved, Reserved
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
    // 108-111: Reserved, I2C8 Master and Slave, I2C9 Master and Slave, GPIO Port T
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 5, 5 ) ),
    // 112-115: Reserved, Reserved, Reserved, Reserved
    BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
                                 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),

};

/*==============================================================================
 *                                Local Functions
 *============================================================================*/
/*==============================================================================*/
#if BSP_PRAGMA_COMPILER_GNU
#include "hw_nvic.h"
extern int main(void);
extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
static void bsp_Interrupt_rstHandler(void)
{

    uint32_t *pui32Src, *pui32Dest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pui32Src = &__data_load__;
    for(pui32Dest = &__data_start__; pui32Dest < &__data_end__; )
    {
        *pui32Dest++ = *pui32Src++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =__bss_start__\n"
          "    ldr     r1, =__bss_end__\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    //
    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).  Any configuration of the floating-point unit using DriverLib
    // APIs must be done here prior to the floating-point unit being enabled.
    //
    // Note that this does not use DriverLib since it might not be included in
    // this project.
    //
    ADDR_TO_REG(NVIC_CPAC) = ((ADDR_TO_REG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                        NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);

    //
    // Call the application's entry point.
    //
    main();
}
#else
static void bsp_Interrupt_rstHandler(void)
{
    extern void _c_int00(void);
    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}
#endif

/*==============================================================================*/
static void
bsp_Interrupt_nmiHandler(void)
{
    bsp_Reset_systemReset( BSP_RESET_SWREASON_UNHANDLED_INT );
}

/*==============================================================================*/
static void
bsp_Interrupt_faultHandler(void)
{
    asm( " TST LR, #4 " );
    asm( " ITE EQ " );
    asm( " MRSEQ R0, MSP " );
    asm( " MRSNE R0, PSP " );
    asm( " LDR R1, [R0, #24] " );
    asm( " B prvGetRegistersFromStack " );

}

/*==============================================================================*/
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    bsp_Reset_systemFault( pulFaultStackAddress );
}

/*==============================================================================*/
static void
bsp_Interrupt_defaultHandler(void)
{
    bsp_Reset_systemReset( BSP_RESET_SWREASON_UNHANDLED_INT );
}
