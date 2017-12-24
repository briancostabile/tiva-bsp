/*============================================================================*/
/**
 * @file bsp_Trace_ektm4c123gxl.h
 * @brief Contains platform specific trace mappings to IO pins
 */
#ifndef BSP_TRACE_EKTM4C123GXL_H
#define BSP_TRACE_EKTM4C123GXL_H

#include "bsp_Gpio.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define BSP_TRACE_ENABLE
//#define BSP_TRACE_INT
//#define BSP_TRACE_UARTS
//#define BSP_TRACE_USBIO

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_INT )
#define BSP_TRACE_INT_ENTER()      {BSP_GPIO_OUT_SET_HIGH( TPA2 );}
#define BSP_TRACE_INT_EXIT()       {BSP_GPIO_OUT_SET_LOW( TPA2 );}
#define BSP_TRACE_INT_LOCK_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_INT_LOCK_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#else
#define BSP_TRACE_INT_ENTER()
#define BSP_TRACE_INT_EXIT()
#define BSP_TRACE_INT_LOCK_ENTER()
#define BSP_TRACE_INT_LOCK_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_UART )
#define BSP_TRACE_UART_INT_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_UART_INT_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_UART_SND_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_UART_SND_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_UART_RCV_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_UART_RCV_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#else
#define BSP_TRACE_UART_INT_ENTER()
#define BSP_TRACE_UART_INT_EXIT()
#define BSP_TRACE_UART_SND_ENTER()
#define BSP_TRACE_UART_SND_EXIT()
#define BSP_TRACE_UART_RCV_ENTER()
#define BSP_TRACE_UART_RCV_EXIT()
#endif

#if defined(BSP_TRACE_ENABLE) && defined( BSP_TRACE_USBIO )
#define BSP_TRACE_USBIO_INT_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA3 );}
#define BSP_TRACE_USBIO_INT_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA3 );}
#define BSP_TRACE_USBIO_SND_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA4 );}
#define BSP_TRACE_USBIO_SND_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA4 );}
#define BSP_TRACE_USBIO_RCV_ENTER()         {BSP_GPIO_OUT_SET_HIGH( TPA5 );}
#define BSP_TRACE_USBIO_RCV_EXIT()          {BSP_GPIO_OUT_SET_LOW( TPA5 );}
#define BSP_TRACE_USBIO_TX_COMPLETE_ENTER() {BSP_GPIO_OUT_SET_HIGH( TPA6 );}
#define BSP_TRACE_USBIO_TX_COMPLETE_EXIT()  {BSP_GPIO_OUT_SET_LOW( TPA6 );}
#define BSP_TRACE_USBIO_RX_AVAIL_ENTER()    {BSP_GPIO_OUT_SET_HIGH( TPA7 );}
#define BSP_TRACE_USBIO_RX_AVAIL_EXIT()     {BSP_GPIO_OUT_SET_LOW( TPA7 );}
#else
#define BSP_TRACE_USBIO_INT_ENTER()
#define BSP_TRACE_USBIO_INT_EXIT()
#define BSP_TRACE_USBIO_SND_ENTER()
#define BSP_TRACE_USBIO_SND_EXIT()
#define BSP_TRACE_USBIO_RCV_ENTER()
#define BSP_TRACE_USBIO_RCV_EXIT()
#define BSP_TRACE_USBIO_TX_COMPLETE_ENTER()
#define BSP_TRACE_USBIO_TX_COMPLETE_EXIT()
#define BSP_TRACE_USBIO_RX_AVAIL_ENTER()
#define BSP_TRACE_USBIO_RX_AVAIL_EXIT()
#endif

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
extern const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS];

#endif
