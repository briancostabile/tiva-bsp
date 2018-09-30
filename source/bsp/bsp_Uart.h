/**
 * Copyright 2017 Brian Costabile
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
 * @file bsp_Uart.h
 * @brief Contains types and defines for the UART subsystem
 */
#ifndef BSP_UART_H
#define BSP_UART_H

#include "bsp_Types.h"
#include "inc/hw_uart.h"

#include "bsp_Platform.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
#define BSP_UART_RX_FIFO_SIZE 16
#define BSP_UART_PLATFORM_NUM_UARTS 8

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
#define BSP_UART_ID0 ((bsp_Uart_Id_t)0)
#define BSP_UART_ID1 ((bsp_Uart_Id_t)1)
#define BSP_UART_ID2 ((bsp_Uart_Id_t)2)
#define BSP_UART_ID3 ((bsp_Uart_Id_t)3)
#define BSP_UART_ID4 ((bsp_Uart_Id_t)4)
#define BSP_UART_ID5 ((bsp_Uart_Id_t)5)
#define BSP_UART_ID6 ((bsp_Uart_Id_t)6)
#define BSP_UART_ID7 ((bsp_Uart_Id_t)7)
typedef uint8_t bsp_Uart_Id_t;


/*============================================================================*/
#define BSP_UART_PIN_SEL_0    ((bsp_Uart_PinSel_t)0)
#define BSP_UART_PIN_SEL_1    ((bsp_Uart_PinSel_t)1)
#define BSP_UART_PIN_SEL_2    ((bsp_Uart_PinSel_t)2)
#define BSP_UART_PIN_SEL_3    ((bsp_Uart_PinSel_t)3)
#define BSP_UART_PIN_SEL_4    ((bsp_Uart_PinSel_t)4)
#define BSP_UART_PIN_SEL_5    ((bsp_Uart_PinSel_t)5)
#define BSP_UART_PIN_SEL_6    ((bsp_Uart_PinSel_t)6)
#define BSP_UART_PIN_SEL_7    ((bsp_Uart_PinSel_t)7)
#define BSP_UART_PIN_SEL_NONE ((bsp_Uart_PinSel_t)8)
typedef uint8_t bsp_Uart_PinSel_t;


/*============================================================================*/
/**
 * @brief Unsigned Integer value for baud rate
 */
typedef uint32_t bsp_Uart_Baud_t;


/*============================================================================*/
/**
 * @brief Enumeration for the parity options in UART mode
 */
enum
{
    BSP_UART_PARITY_NONE = 0,
    BSP_UART_PARITY_ODD  = 1,
    BSP_UART_PARITY_EVEN = 2,
    BSP_UART_PARITY_ONE  = 3,
    BSP_UART_PARITY_ZERO = 4
};
typedef uint8_t bsp_Uart_Parity_t;


/*============================================================================*/
/**
 * @brief Enumeration for the stop bit options in UART mode
 */
enum
{
    BSP_UART_STOP_BIT_1,
    BSP_UART_STOP_BIT_2
};
typedef uint8_t bsp_Uart_StopBit_t;


/*============================================================================*/
/**
 * @brief Enumeration for the data bit length options in UART mode
 */
enum
{
    BSP_UART_DATA_BIT_5 = 0,
    BSP_UART_DATA_BIT_6 = 1,
    BSP_UART_DATA_BIT_7 = 2,
    BSP_UART_DATA_BIT_8 = 3
};
typedef uint8_t bsp_Uart_DataBit_t;


/*============================================================================*/
/**
 * @brief Enumeration for the flow control options in UART mode.
 *
 * This implementation doesn't support flow control so the only option is NONE
 */
enum
{
    BSP_UART_FLOW_NONE
};
typedef uint8_t bsp_Uart_Flow_t;


/*============================================================================*/
/**
 * @brief this type defines the client supplied callback for Rx operations.
 */
typedef void (*bsp_Uart_RxCallback_t)( void*  callbackArg,
                                       void*  rxDataPtr,
                                       size_t numBytes );


/*============================================================================*/
/**
 * @brief this type defines the client supplied callback for Tx operations.
 */
typedef void (*bsp_Uart_TxCallback_t)( void* callbackArg );


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Uart_init( void );


/*============================================================================*/
/**
 * @brief This function is used to do the basic setup of a USART.
 *
 * This function selects the port to use for the USART and the mode (SPI or UART)
 *
 * @param id - The enumerated USART Id to setup.
 * @param port - The enumerated port to use for the selected USART
 * @param mode - The enumerated mode (SPI or UART) to use for the USART
 *
 * Note: This implementation only supports UART
 */
void
bsp_Uart_control( bsp_Uart_Id_t     id,
                  bsp_Uart_PinSel_t rxSel,
                  bsp_Uart_PinSel_t txSel,
                  bsp_Uart_PinSel_t rtsSel,
                  bsp_Uart_PinSel_t ctsSel );


/*============================================================================*/
/**
 * @brief This function is used to configure the UART mode settings.
 *
 * This function sets the typical UART settings. baud, parity, etc.
 *
 * @param baud - The integer baud rate
 * @param parity - The enumerated parity setting
 * @param stop - The enumerated number of stop bits setting
 * @param data - The enumerated number of data bit length setting
 * @param flow - The enumerated flow control setting
 */
void
bsp_Uart_config( bsp_Uart_Id_t      id,
                 bsp_Uart_Baud_t    baud,
                 bsp_Uart_Parity_t  parity,
                 bsp_Uart_StopBit_t stop,
                 bsp_Uart_DataBit_t data,
                 bsp_Uart_Flow_t    flow );


/*============================================================================*/
/**
 * @brief This function us used to send data out the UART.
 *
 * This function will setup a DMA to send out the passed in data. When the
 * data has been sent the client supplied callback will be called. If the client
 * callback is NULL then the data will be sent inline instead of using DMA and
 * the function won't return until the data has been sent.
 * The passed in buffer must not be reused by the client until the callback has
 * been called or (in the case of NULL callback) the function returns.
 *
 * @param sndDataPtr - A pointer to the data to send
 * @param numBytes - The number of bytes to send from the passed in pointer
 * @param callback - The client supplied callback to be called after the data
 *                   has been sent. NULL can be used to force data to be sent
 *                   manually vs. interrupt driven.
 */
void
bsp_Uart_snd( bsp_Uart_Id_t         id,
              void*                 sndDataPtr,
              size_t                numBytes,
              void*                 callbackArg,
              bsp_Uart_TxCallback_t callback );


/*============================================================================*/
/**
 * @brief This function us used to setup a buffer to receive data from the UART.
 *
 * This function will enable the UART Rx and pull data into the passed in
 * buffer. The passed in callback is called when the buffer has been filled. The
 * UART always uses Interrupt mode for receiving data
 *
 * @param bufPtr - A pointer to the data buffer to receive data into.
 * @param cnt - The length of the passed in buffer. After cnt data has been read
 *              the user supplied callback will be called,.
 * @param callback - The client supplied callback to be called after the data
 *                   has been sent. If NULL is passed in then the UART driver
 *                   will still setup a receive, but it won't notify the client
 *                   when it is done.
 */
size_t
bsp_Uart_rcv( bsp_Uart_Id_t         id,
              void*                 bufPtr,
              size_t                cnt,
              void*                 callbackArg,
              bsp_Uart_RxCallback_t callback );


/*============================================================================*/
void bsp_Uart_interruptHandler0( void );
void bsp_Uart_interruptHandler1( void );
void bsp_Uart_interruptHandler2( void );
void bsp_Uart_interruptHandler3( void );
void bsp_Uart_interruptHandler4( void );
void bsp_Uart_interruptHandler5( void );
void bsp_Uart_interruptHandler6( void );
void bsp_Uart_interruptHandler7( void );



#endif
