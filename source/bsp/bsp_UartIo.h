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
