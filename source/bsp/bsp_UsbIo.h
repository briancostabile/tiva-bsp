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
 * @file bsp_UsbIo.h
 * @brief Contains types and defines for the USB CDC driver
 */
#ifndef BSP_USBIO_H
#define BSP_USBIO_H

#include "bsp_Types.h"

/*==================================================================================================
 *                                           Types
 *================================================================================================*/
typedef void (* bsp_UsbIo_DataAvailableCallback_t)( size_t cnt );


/*==================================================================================================
 *                                        Public Functions
 *================================================================================================*/
extern void bsp_UsbIo_init( void );
extern void bsp_UsbIo_registerDataAvailableCallback( int                               file_descriptor,
                                                     bsp_UsbIo_DataAvailableCallback_t callback );

extern int bsp_UsbIo_open( const char* path, unsigned int flags, int llv_fd );
extern int bsp_UsbIo_close( int file_descriptor );
extern int bsp_UsbIo_read( int file_descriptor, char* buffer, size_t count );
extern int bsp_UsbIo_write( int file_descriptor, const char* buffer, size_t count );
extern off_t bsp_UsbIo_lseek( int file_descriptor, off_t offset, int origin );
extern int bsp_UsbIo_unlink( const char* path );
extern int bsp_UsbIo_rename( const char* old_name, const char* new_name );


/*============================================================================*/
void
bsp_UsbIo_interruptHandler( void );

#endif
