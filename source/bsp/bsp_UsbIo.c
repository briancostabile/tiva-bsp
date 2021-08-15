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
 * @file bsp_UsbIo.c
 * @brief Contains Functions for hooking into a USB CDC driver
 */

#include "bsp_Types.h"
#include "bsp_UsbIo.h"
#include "bsp_UsbCdc.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Assert.h"
#include "bsp_Interrupt.h"
#include "bsp_Io.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
typedef struct {
    int cdcFd;
} bsp_UsbIo_descriptor_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
bsp_UsbIo_descriptor_t bsp_UsbIo_descriptor;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_UsbIo_init( void )
{
    char filenameBuf[5] = "usb0";

    bsp_UsbIo_descriptor.cdcFd = 0;

    bsp_Io_addDevice( filenameBuf,
                      &bsp_UsbIo_descriptor,
                      bsp_UsbIo_open,
                      bsp_UsbIo_close,
                      bsp_UsbIo_read,
                      bsp_UsbIo_write,
                      bsp_UsbIo_lseek,
                      bsp_UsbIo_unlink,
                      bsp_UsbIo_rename );

    return;
}


/*============================================================================*/
void
bsp_UsbIo_registerDataAvailableCallback( int fd, bsp_UsbIo_DataAvailableCallback_t callback )
{
    bsp_UsbCdc_registerCallbackDataAvailable( fd, (bsp_UsbCdc_DataAvailableCallback_t)callback );
    return;
}


/*============================================================================*/
int
bsp_UsbIo_open( const char*  path,
                unsigned int flags,
                int          llv_fd )
{
    uint8_t pathLen;

    pathLen = strlen(path);

    /* Todo: Do something smarter here */
    if( path[(pathLen-2)] == '0' )
    {
        bsp_UsbIo_descriptor.cdcFd = bsp_UsbCdc_open();
    }
    else
    {
        bsp_UsbIo_descriptor.cdcFd = bsp_UsbCdc_open();
    }

    return( (int)&bsp_UsbIo_descriptor );
}


/*============================================================================*/
int
bsp_UsbIo_close( int fd )
{
    return( bsp_UsbCdc_close( ((bsp_UsbIo_descriptor_t*)fd)->cdcFd ) );
}


/*============================================================================*/
int
bsp_UsbIo_read( int    fd,
                char*  buffer,
                size_t count )
{
    return( bsp_UsbIo_read( ((bsp_UsbIo_descriptor_t*)fd)->cdcFd, buffer, count ) );
}


/*============================================================================*/
int
bsp_UsbIo_write( int         fd,
                 const char* buffer,
                 size_t      count )
{
    return( bsp_UsbIo_write( ((bsp_UsbIo_descriptor_t*)fd)->cdcFd, buffer, count )  );
}


/*============================================================================*/
off_t
bsp_UsbIo_lseek( int   fd,
                 off_t offset,
                 int   origin )
{
    return( (off_t)-1 );
}


/*============================================================================*/
int
bsp_UsbIo_unlink( const char* path )
{
    return( -1 );
}


/*============================================================================*/
int
bsp_UsbIo_rename( const char* old_name,
                  const char* new_name )
{
    return( -1 );
}
