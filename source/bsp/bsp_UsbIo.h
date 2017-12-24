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
