/*============================================================================*/
/**
 * @file bsp_Interrupt_tm4c123.c
 * @brief Contains Macros and defines for the interrupt controller specific
 *        to the tm4c123 with SensorHub platform.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Interrupt.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"
#include "bsp_Reset.h"
#include "bsp_Gpio.h"
#include "bsp_Uart.h"
#include "bsp_UsbIo.h"


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

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************

/* 16 Exception Vectors */
extern uint32_t __STACK_TOP;
BSP_PRAGMA_DATA_LOCATION(bsp_Interrupt_vectorTableExceptions, ".vecsException")
void (* const bsp_Interrupt_vectorTableExceptions[])(void) =
{
	(bsp_Interrupt_VectorHandler_t)&(__STACK_TOP),
    bsp_Interrupt_rstHandler,                 // The reset handler
    bsp_Interrupt_nmiHandler,                 // The NMI handler
    bsp_Interrupt_faultHandler,               // The hard fault handler
    bsp_Interrupt_defaultHandler,             // The MPU fault handler
    bsp_Interrupt_defaultHandler,             // The bus fault handler
    bsp_Interrupt_defaultHandler,             // The usage fault handler
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // SVCall handler
    bsp_Interrupt_defaultHandler,             // Debug monitor handler
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // The PendSV handler
    bsp_Interrupt_defaultHandler,             // The SysTick handler
};
BSP_PRAGMA_DATA_REQUIRED(bsp_Interrupt_vectorTableExceptions)

/* 139 Interrupt Vectors */
BSP_PRAGMA_DATA_LOCATION(bsp_Interrupt_vectorTableInterrutps, ".vecsInterrupt")
void (* const bsp_Interrupt_vectorTableInterrutps[])(void) =
{
	bsp_Gpio_interruptHandlerPortA,           // GPIO Port A
	bsp_Gpio_interruptHandlerPortB,           // GPIO Port B
	bsp_Gpio_interruptHandlerPortC,           // GPIO Port C
	bsp_Gpio_interruptHandlerPortD,           // GPIO Port D
	bsp_Gpio_interruptHandlerPortE,           // GPIO Port E
	bsp_Uart_interruptHandler0,               // UART0 Rx and Tx
	bsp_Uart_interruptHandler1,               // UART1 Rx and Tx
    bsp_Interrupt_defaultHandler,             // SSI0 Rx and Tx
    bsp_Interrupt_defaultHandler,             // I2C0 Master and Slave
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
    bsp_Interrupt_defaultHandler,             // Timer 0 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 0 subtimer B
    bsp_Interrupt_defaultHandler,             // Timer 1 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 1 subtimer B
    bsp_Interrupt_defaultHandler,             // Timer 2 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 2 subtimer B
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
    bsp_Interrupt_defaultHandler,             // Timer 3 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 3 subtimer B
    bsp_Interrupt_defaultHandler,             // I2C1 Master and Slave
    bsp_Interrupt_defaultHandler,             // Quadrature Encoder 1
    bsp_Interrupt_defaultHandler,             // CAN0
    bsp_Interrupt_defaultHandler,             // CAN1
    bsp_Interrupt_defaultHandler,             // CAN2
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // Hibernate
    bsp_UsbIo_interruptHandler,              // USB0
    bsp_Interrupt_defaultHandler,             // PWM Generator 3
    bsp_Interrupt_defaultHandler,             // uDMA Software Transfer
    bsp_Interrupt_defaultHandler,             // uDMA Error
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 0
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 1
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 2
    bsp_Interrupt_defaultHandler,             // ADC1 Sequence 3
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
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
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // I2C2 Master and Slave
    bsp_Interrupt_defaultHandler,             // I2C3 Master and Slave
    bsp_Interrupt_defaultHandler,             // Timer 4 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 4 subtimer B
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // Timer 5 subtimer A
    bsp_Interrupt_defaultHandler,             // Timer 5 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 0 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 0 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 1 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 1 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 2 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 2 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 3 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 3 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 4 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 4 subtimer B
    bsp_Interrupt_defaultHandler,             // Wide Timer 5 subtimer A
    bsp_Interrupt_defaultHandler,             // Wide Timer 5 subtimer B
    bsp_Interrupt_defaultHandler,             // FPU
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    bsp_Interrupt_defaultHandler,             // I2C4 Master and Slave
    bsp_Interrupt_defaultHandler,             // I2C5 Master and Slave
    bsp_Gpio_interruptHandlerPortM,           // GPIO Port M
    bsp_Gpio_interruptHandlerPortN,           // GPIO Port N
    bsp_Interrupt_defaultHandler,             // Quadrature Encoder 2
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
    BSP_INTERRUPT_HANDLER_NULL,               // Reserved
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
    bsp_Interrupt_defaultHandler,             // GPIO Port R
    bsp_Interrupt_defaultHandler,             // GPIO Port S
    bsp_Interrupt_defaultHandler,             // PWM 1 Generator 0
    bsp_Interrupt_defaultHandler,             // PWM 1 Generator 1
    bsp_Interrupt_defaultHandler,             // PWM 1 Generator 2
    bsp_Interrupt_defaultHandler,             // PWM 1 Generator 3
    bsp_Interrupt_defaultHandler              // PWM 1 Fault
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
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 4-7: GPIO Port E, UART0 Rx and Tx, UART1 Rx and Tx, SSI0 Rx and Tx
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 8-11: I2C0 Master and Slave, PWM Fault, PWM Generator 0, PWM Generator 1
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
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
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 20-23: Timer 0 subtimer B, Timer 1 subtimer A, Timer 1 subtimer B, Timer 2 subtimer A
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 24-27: Timer 2 subtimer B, Analog Comparator 0, Analog Comparator 1, Analog Comparator 2
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 28-31: System Control (PLL, OSC, BO), FLASH Control, GPIO Port F, GPIO Port G
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 32-35: GPIO Port H, UART2 Rx and Tx, SSI1 Rx and Tx, Timer 3 subtimer A
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 36-39: Timer 3 subtimer B, I2C1 Master and Slave, Quadrature Encoder 1, CAN0
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 40-43: CAN1, CAN2, Reserved, Hibernate
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 44-47: USB0, PWM Generator 3, uDMA Software Transfer, uDMA Error
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 48-51: ADC1 Sequence 0, ADC1 Sequence 1, ADC1 Sequence , ADC1 Sequence 3
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 52-55: Reserved, Reserved, GPIO Port J, GPIO Port K
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 56-59: GPIO Port L, SSI2 Rx and Tx, SSI3 Rx and Tx, UART3 Rx and Tx
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 60-63: UART4 Rx and Tx, UART5 Rx and Tx, UART6 Rx and Tx, UART7 Rx and Tx
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 64-67: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 68-71: I2C2 Master and Slave, I2C3 Master and Slave, Timer 4 subtimer A, Timer 4 subtimer B
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 72-75: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 76-79: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 80-83: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 84-87: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 88-91: Reserved, Reserved, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 92-95: Timer 5 subtimer A, Timer 5 subtimer B, Wide Timer 0 subtimer A, Wide Timer 0 subtimer B
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 96-99: Wide Timer 1 subtimer A, Wide Timer 1 subtimer B, Wide Timer 2 subtimer A, Wide Timer 2 subtimer B
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 100-103: Wide Timer 3 subtimer A, Wide Timer 3 subtimer B, Wide Timer 4 subtimer A, Wide Timer 4 subtimer B
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 104-107: Wide Timer 5 subtimer A, Wide Timer 5 subtimer B, FPU, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 108-111: Reserved, I2C4 Master and Slave, I2C5 Master and Slave, GPIO Port M
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 112-115: GPIO Port N, Quadrature Encoder 2, Reserved, Reserved
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 116-119: GPIO Port P (Summary or P0), GPIO Port P1, GPIO Port P2, GPIO Port P3
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 120-123: GPIO Port P4, GPIO Port P5, GPIO Port P6, GPIO Port P7
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 124-127: GPIO Port Q (Summary or Q0), GPIO Port Q1, GPIO Port Q2, GPIO Port Q3
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 128-131: GPIO Port Q4, GPIO Port Q5, GPIO Port Q6, GPIO Port Q7
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 132-135: GPIO Port R, GPIO Port S, PWM 1 Generator 0, PWM 1 Generator 1
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) ),
	// 136-138: PWM 1 Generator 2, PWM 1 Generator 3, PWM 1 Fault, Invalid
	BSP_INTERRUPT_BUILD_PRI_REG( BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ),
								 BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( 1, 1 ) )
};

/*==============================================================================
 *                                Local Functions
 *============================================================================*/
//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
static void
bsp_Interrupt_rstHandler(void)
{
	extern void _c_int00(void);
    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
bsp_Interrupt_nmiHandler(void)
{
    //bsp_Reset_systemReset( BSP_RESET_SWREASON_UNHANDLED_INT );
    while(1);
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
bsp_Interrupt_faultHandler(void)
{
    //bsp_Reset_systemReset( BSP_RESET_SWREASON_UNHANDLED_INT );
    asm( " TST LR, #4 " );
    asm( " ITE EQ " );
    asm( " MRSEQ R0, MSP " );
    asm( " MRSNE R0, PSP " );
    asm( " LDR R1, [R0, #24] " );
    asm( " B prvGetRegistersFromStack " );

}

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
bsp_Interrupt_defaultHandler(void)
{
    //bsp_Reset_systemReset( BSP_RESET_SWREASON_UNHANDLED_INT );
    while(1);
}
