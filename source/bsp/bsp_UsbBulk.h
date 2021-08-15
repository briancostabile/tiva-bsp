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
 * @file bsp_UsbBulk.h
 * @brief Contains types and defines for the USB Bulk driver
 */
#pragma once

#include "bsp_Types.h"

/*==================================================================================================
 *                                           Types
 *================================================================================================*/
typedef void (* bsp_UsbBulk_DataAvailableCallback_t)( size_t cnt );
typedef void (* bsp_UsbBulk_ConnectionCallback_t)( bool connected );
typedef void (* bsp_UsbBulk_TxDoneCallback_t)( size_t cnt );


/*==================================================================================================
 *                                        Public Functions
 *================================================================================================*/
extern void bsp_UsbBulk_init( void );
extern void bsp_UsbBulk_registerCallbackDataAvailable( int fd, bsp_UsbBulk_DataAvailableCallback_t callback );
extern bool bsp_UsbBulk_registerCallbackConnection( int fd, bsp_UsbBulk_ConnectionCallback_t callback );
extern void bsp_UsbBulk_registerCallbackTxDone( int fd, bsp_UsbBulk_TxDoneCallback_t callback );

extern int bsp_UsbBulk_open( const char* path, unsigned int flags, int llv_fd );
extern int bsp_UsbBulk_close( int fd );
extern int bsp_UsbBulk_read( int fd, char* buffer, size_t count );
extern int bsp_UsbBulk_write( int fd, const char* buffer, size_t count );


/*============================================================================*/
void bsp_UsbBulk_interruptHandler( void );

