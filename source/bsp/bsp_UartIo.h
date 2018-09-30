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
 * @file bsp_UartIo.h
 * @brief Contains types and prototypes for stdin/stdout interface on top of
 *        UART or USB
 */
#ifndef BSP_UARTIO_H
#define BSP_UARTIO_H

#include "bsp_Types.h"


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
typedef void (*bsp_UartIo_DataAvailCallback_t)( void );


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_UartIo_init( void );

/*============================================================================*/
void
bsp_UartIo_registerDataAvailableCallback( int                            file_descriptor,
                                          bsp_UartIo_DataAvailCallback_t callback );


/*============================================================================*/
int
bsp_UartIo_open( const char*  path,
                 unsigned int flags,
                 int          llv_fd );


/*============================================================================*/
int
bsp_UartIo_close( int file_descriptor );


/*============================================================================*/
int
bsp_UartIo_read( int    file_descriptor,
                 char*  buffer,
                 size_t count );


/*============================================================================*/
int
bsp_UartIo_write( int         file_descriptor,
                  const char* buffer,
                  size_t      count );


/*============================================================================*/
off_t
bsp_UartIo_lseek( int   file_descriptor,
                  off_t offset,
                  int   origin );


/*============================================================================*/
int
bsp_UartIo_unlink( const char* path );


/*============================================================================*/
int
bsp_UartIo_rename( const char* old_name,
                   const char* new_name );



#endif
