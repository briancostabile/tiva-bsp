/*============================================================================*/
/**
 * @file bsp_Platform_ektm4c123gxl.h
 */
#ifndef BSP_PLATFORM_EKTM4C123GXL_H
#define BSP_PLATFORM_EKTM4C123GXL_H

#include "inc/tm4c123gh6pm.h"


/*==============================================================================
 *                            Clock related defines
 *============================================================================*/
/*============================================================================*/
#define BSP_PLATFORM_OSC_PRESENT TRUE
#define BSP_PLATFORM_OSC_FREQ_HZ 16000000

/*============================================================================*/
#define BSP_PLATFORM_XOSC_PRESENT TRUE
#define BSP_PLATFORM_XOSC_FREQ_HZ 32768

/*============================================================================*/
#define BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ 80000000

/*==============================================================================
 *                            IO defines
 *============================================================================*/
#define BSP_PLATFORM_IO_UART_NUM         1
#define BSP_PLATFORM_IO_USB_NUM          2

#define BSP_PLATFORM_IO_UART0_ID         0
#define BSP_PLATFORM_IO_UART0_RX_PIN_SEL 0
#define BSP_PLATFORM_IO_UART0_TX_PIN_SEL 0
#define BSP_PLATFORM_IO_UART0_BAUD       (115200 * 1)
#define BSP_PLATFORM_IO_UART0_RX_BUF_LEN 32
#define BSP_PLATFORM_IO_UART0_TX_BUF_LEN 256

#define BSP_PLATFORM_IO_USB0_RX_BUF_LEN  32
#define BSP_PLATFORM_IO_USB0_TX_BUF_LEN  256
#define BSP_PLATFORM_IO_USB1_RX_BUF_LEN  32
#define BSP_PLATFORM_IO_USB1_TX_BUF_LEN  256

/*==============================================================================
 *                            STDIO Mapping
 *============================================================================*/
#define BSP_PLATFORM_STDOUT_MAPPING "usb0"
#define BSP_PLATFORM_STDIN_MAPPING  "usb0"
#define BSP_PLATFORM_STDERR_MAPPING "usb1"

#endif
