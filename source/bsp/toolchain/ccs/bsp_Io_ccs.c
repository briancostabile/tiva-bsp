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
 * @file bsp_Io_ccs.c
 * @brief Contains Functions for connecting serial interfaces with libc IO
 */
#include "bsp_Types.h"
#include "bsp_Io.h"
#include "bsp_Platform.h"

// CCS standard library doesn't have fileno, so the low-level device interface
// is needed to get access to the file descriptor of the selected IO device
#if defined(ccs)
#include "lowlev.h"
#endif

/*============================================================================*/
void
bsp_Io_init( void )
{
    return;
}

/*============================================================================*/
/**
 * CCS uses device tables to save information about IO devices. There are a
 * a limited number of device table entries available for the user. The TI
 * library allows for 3 devices
 */
void
bsp_Io_addDevice( const char*     path,
                  void*           cookie,
                  bsp_Io_Open_t   open,
                  bsp_Io_Close_t  close,
                  bsp_Io_Read_t   read,
                  bsp_Io_Write_t  write,
                  bsp_Io_Lseek_t  lseek,
                  bsp_Io_Unlink_t unlink,
                  bsp_Io_Rename_t rename )
{
    char reopen_path[7];
    int ret = add_device( (char*)path, _SSA, open, close, read, write, lseek, unlink, rename );

    if (ret < 0)
    {
        // Device not added
        return;
    }

    memset( reopen_path, 0, sizeof(reopen_path) );
    strcat( reopen_path, path );
    strcat( reopen_path, ":" );

    FILE* dev_fp = fopen( reopen_path, "w" );

    if( strcmp( BSP_PLATFORM_IO_MAP_STDIN, path ) == 0 )
    {
        freopen( reopen_path, "r", stdin );
        setvbuf( stdin, NULL, _IONBF, 0 );
        stdin->flags &= ~(_IOFBF | _IOLBF | _IONBF);
        stdin->flags |= _IONBF;
        stdin->fd = dev_fp->fd;
    }
    if( strcmp( BSP_PLATFORM_IO_MAP_STDOUT, path ) == 0 )
    {
        freopen( reopen_path, "w", stdout );
        setvbuf( stdout, NULL, _IONBF, 0 );
        stdout->flags &= ~(_IOFBF | _IOLBF | _IONBF);
        stdout->flags |= _IONBF;
        stdout->fd = dev_fp->fd;
    }
    if( strcmp( BSP_PLATFORM_IO_MAP_STDERR, path ) == 0 )
    {
        freopen( reopen_path, "w", stderr );
        setvbuf( stderr, NULL, _IONBF, 0 );
        stderr->flags &= ~(_IOFBF | _IOLBF | _IONBF);
        stderr->flags |= _IONBF;
        stderr->fd = dev_fp->fd;
    }

    return;
}


/*============================================================================*/
int
bsp_Io_fileToDfd( FILE* file )
{
    return( _stream[(file)->fd].dfd );
}

