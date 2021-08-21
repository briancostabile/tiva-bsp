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
 * @file bsp_Io.c
 * @brief Contains Functions for connecting serial interfaces with libc IO
 */
#include "bsp_Types.h"
#include "bsp_Io.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"

#include <reent.h>

#define BSP_IO_DEV_IDX_VALID(_idx) ((uint8_t)(_idx) < BSP_IO_DEV_IDX_NUM)

typedef struct bsp_Io_devInfo_n{
    void*           cookie;
    bsp_Io_Open_t   open;
    bsp_Io_Close_t  close;
    bsp_Io_Read_t   read;
    bsp_Io_Write_t  write;
    bsp_Io_Lseek_t  lseek;
    bsp_Io_Unlink_t unlink;
    bsp_Io_Rename_t rename;
} bsp_Io_devInfo_t;

enum
{
    BSP_IO_DEV_IDX_STDOUT = 0,
    BSP_IO_DEV_IDX_STDERR = 1,
    BSP_IO_DEV_IDX_STDIN  = 2,
    BSP_IO_DEV_IDX_NUM    = 3
};
bsp_Io_devInfo_t bsp_Io_devInfo_list[BSP_IO_DEV_IDX_NUM];

static const char* bsp_Io_lookup[] = { BSP_PLATFORM_IO_MAP_STDOUT,
                                       BSP_PLATFORM_IO_MAP_STDERR,
                                       BSP_PLATFORM_IO_MAP_STDIN };

/*============================================================================*/
static uint8_t
bsp_Io_devPathToIdx( const char* path )
{
    uint8_t idx;

    for( idx=0; idx<DIM(bsp_Io_lookup); idx++ )
    {
        if( strcmp( bsp_Io_lookup[idx], path ) == 0 )
        {
            break;
        }
    }

    return idx;
}

/*============================================================================*/
static uint8_t
bsp_Io_fdToIdx( int fd )
{
    uint8_t idx = BSP_IO_DEV_IDX_NUM;
    if( fd == fileno(stdout) )      { idx = BSP_IO_DEV_IDX_STDOUT; }
    else if( fd == fileno(stderr) ) { idx = BSP_IO_DEV_IDX_STDERR; }
    else if( fd == fileno(stdin) )  { idx = BSP_IO_DEV_IDX_STDIN;  }
    return idx;
}

/*============================================================================*/
void exit(int code)
{
    (void)code;
    bsp_Reset_systemReset( BSP_RESET_SWREASON_EXIT );
    while(1); // Will never reach this while loop
}

/*============================================================================*/
int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    uint8_t idx = bsp_Io_devPathToIdx( file );

    if( (BSP_IO_DEV_IDX_VALID(idx) == true) &&
        (bsp_Io_devInfo_list[idx].open != NULL) )
    {
        return bsp_Io_devInfo_list[idx].open( file, flags, mode );
    }
    return -1;
}

/*============================================================================*/
int _close_r(struct _reent *ptr, int fd)
{
    uint8_t idx = bsp_Io_fdToIdx( fd );

    if( (BSP_IO_DEV_IDX_VALID(idx) == true) &&
        (bsp_Io_devInfo_list[idx].close != NULL) )
    {
        return bsp_Io_devInfo_list[idx].close( (int)bsp_Io_devInfo_list[idx].cookie );
    }
    return -1;
}

/*============================================================================*/
_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t cnt)
{
    uint8_t idx = bsp_Io_fdToIdx( fd );

    if( (BSP_IO_DEV_IDX_VALID(idx) == true) &&
        (bsp_Io_devInfo_list[idx].read != NULL) )
    {
        return bsp_Io_devInfo_list[idx].read( (int)bsp_Io_devInfo_list[idx].cookie, buf, cnt );
    }
    return -1;
}

/*============================================================================*/
_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t cnt)
{
    uint8_t idx = bsp_Io_fdToIdx( fd );

    if( (BSP_IO_DEV_IDX_VALID(idx) == true) &&
        (bsp_Io_devInfo_list[idx].write != NULL) )
    {
        return bsp_Io_devInfo_list[idx].write( (int)bsp_Io_devInfo_list[idx].cookie, buf, cnt );
    }
    return -1;
}

/*============================================================================*/
extern uint32_t __heap_start__;
void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    static void *heap_ptr = 0;
    if (heap_ptr == 0) {heap_ptr = (void *)&__heap_start__;}
    heap_ptr += incr;
    return (void*)(heap_ptr);
}

/*============================================================================*/
int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    return 0;
}

/*============================================================================*/
off_t _lseek_r(struct _reent *ptr, int fd, off_t pos, int whence)
{
    uint8_t idx = bsp_Io_fdToIdx( fd );

    if( (BSP_IO_DEV_IDX_VALID(idx) == true) &&
        (bsp_Io_devInfo_list[idx].lseek != NULL) )
    {
        return bsp_Io_devInfo_list[idx].lseek( (int)bsp_Io_devInfo_list[idx].cookie, pos, whence );
    }
    return -1;
}

/*============================================================================*/
int _isatty_r(struct _reent *ptr, int file)
{
    return 1;
}

/*============================================================================*/
static void
bsp_Io_mapDevice( FILE*           fp,
                  bsp_Io_devInfo_t* dev_ptr,
                  void*           cookie,
                  bsp_Io_Open_t   open,
                  bsp_Io_Close_t  close,
                  bsp_Io_Read_t   read,
                  bsp_Io_Write_t  write,
                  bsp_Io_Lseek_t  lseek,
                  bsp_Io_Unlink_t unlink,
                  bsp_Io_Rename_t rename )
{
    dev_ptr->cookie = cookie;
    dev_ptr->open   = open;
    dev_ptr->close  = close;
    dev_ptr->read   = read;
    dev_ptr->write  = write;
    dev_ptr->lseek  = lseek;
    dev_ptr->unlink = unlink;
    dev_ptr->rename = rename;
    setvbuf( fp, NULL, _IONBF, 0 );
}

/*============================================================================*/
void
bsp_Io_init( void )
{
    memset( bsp_Io_devInfo_list, 0, sizeof(bsp_Io_devInfo_list) );
    return;
}

/*============================================================================*/
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
    if( strcmp( BSP_PLATFORM_IO_MAP_STDOUT, path ) == 0 )
    {
        bsp_Io_mapDevice( stdout, &bsp_Io_devInfo_list[BSP_IO_DEV_IDX_STDOUT],
                          cookie, open, close, read, write, lseek, unlink, rename );
    }
    if( strcmp( BSP_PLATFORM_IO_MAP_STDERR, path ) == 0 )
    {
        bsp_Io_mapDevice( stderr, &bsp_Io_devInfo_list[BSP_IO_DEV_IDX_STDERR],
                          cookie, open, close, read, write, lseek, unlink, rename );
    }
    if( strcmp( BSP_PLATFORM_IO_MAP_STDIN, path ) == 0 )
    {
        bsp_Io_mapDevice( stdin, &bsp_Io_devInfo_list[BSP_IO_DEV_IDX_STDIN],
                          cookie, open, close, read, write, lseek, unlink, rename );
    }

    return;
}

/*============================================================================*/
int
bsp_Io_fileToDfd( FILE* file )
{
    int ret = 0;
    uint8_t idx = bsp_Io_fdToIdx( fileno(file) );

    if( BSP_IO_DEV_IDX_VALID(idx) == true )
    {
        ret = (int)(bsp_Io_devInfo_list[idx].cookie);
    }
    return( ret );
}
