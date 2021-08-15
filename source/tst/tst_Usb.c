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
 * @file tst_Usb.c
 * @brief Test functions to read/write arbitrary memory regions
 */

#include "bsp_Types.h"
#include "bsp_UsbBulk.h"
#include "tst.h"
#include "tst_Sys.h"
#include "osapi.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// String globals
TST_STR_CMD( TST_USB_START_STR_CMD, "start" );
TST_STR_HLP( TST_USB_START_STR_HLP, "Start USB testing" );
TST_STR_CMD( TST_USB_SEND_STR_CMD,  "send" );
TST_STR_HLP( TST_USB_SEND_STR_HLP,  "Send Test Packet" );
TST_STR_CMD( TST_USB_STOP_STR_CMD,  "stop" );
TST_STR_CMD( TST_USB_STOP_STR_HLP,  "Stop USB testing" );
TST_STR_CMD( TST_USB_STATUS_STR_CMD,  "status" );
TST_STR_CMD( TST_USB_STATUS_STR_HLP,  "Dump USB Stats" );

TST_STR_OUT( TST_USB_ERROR_STR,  "Error: Expecting %d parameters"NL );

/*============================================================================*/
bsp_UsbBulk_DataAvailableCallback_t tst_Usb_savedCbDataAvailable;
bsp_UsbBulk_ConnectionCallback_t    tst_Usb_savedCbConnection;
bsp_UsbBulk_TxDoneCallback_t        tst_Usb_savedCbTxDone;

/*============================================================================*/
int tst_Usb_fd;
int32_t tst_Usb_txCnt = 0;
int32_t tst_Usb_rxCnt = 0;

uint8_t tst_Usb_cmdBuffer[32];
uint8_t tst_Usb_dataBuffer[4096];

/*==============================================================================
 *                            Private Functions
 *============================================================================*/
/*============================================================================*/
void tst_Usb_handlerDataAvailable( size_t cnt )
{
    int rd_cnt = bsp_UsbBulk_read(tst_Usb_fd, tst_Usb_cmdBuffer, sizeof(tst_Usb_cmdBuffer));
    tst_Usb_rxCnt += rd_cnt;

    if( tst_Usb_cmdBuffer[0] == 0x03 )
    {
        int pkt_len = sizeof(tst_Usb_dataBuffer);
        int snt = 0;
        while( pkt_len > 0 )
        {
            snt += bsp_UsbBulk_write(tst_Usb_fd, &tst_Usb_dataBuffer[snt], pkt_len);
            pkt_len -= snt;
        }
    }
}

/*============================================================================*/
void tst_Usb_handlerConnection( bool connected )
{
    if( connected )
    {
        printf("Host Connected\n");
    }
    else
    {
        printf("Host Disconnected\n");
    }
}

/*============================================================================*/
void tst_Usb_handlerTxDone( size_t cnt )
{
    tst_Usb_txCnt += cnt;
}

/*============================================================================*/
static tst_Status_t
tst_Usb_start( int argc, char** argv )
{
    tst_Usb_fd = bsp_UsbBulk_open();

    for( int i=1;i<DIM(tst_Usb_dataBuffer); i++ )
    {
        tst_Usb_dataBuffer[i]=i;
    }

    tst_Usb_savedCbDataAvailable = bsp_UsbBulk_registerCallbackDataAvailable( tst_Usb_fd, tst_Usb_handlerDataAvailable );
    tst_Usb_savedCbConnection    = bsp_UsbBulk_registerCallbackConnection( tst_Usb_fd, tst_Usb_handlerConnection );
    tst_Usb_savedCbTxDone        = bsp_UsbBulk_registerCallbackTxDone( tst_Usb_fd, tst_Usb_handlerTxDone );

    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Usb_stop( int argc, char** argv )
{
    /* Restore the previous callbacks */
    bsp_UsbBulk_registerCallbackDataAvailable( tst_Usb_fd, tst_Usb_savedCbDataAvailable );
    bsp_UsbBulk_registerCallbackConnection( tst_Usb_fd, tst_Usb_savedCbConnection );
    bsp_UsbBulk_registerCallbackTxDone( tst_Usb_fd, tst_Usb_savedCbTxDone );
    bsp_UsbBulk_close( tst_Usb_fd );
    tst_Usb_fd = 0;
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Usb_send( int argc, char** argv )
{
    if( argc < 1 )
    {
        printf( TST_USB_ERROR_STR, 1 );
        return( TST_STATUS_ERROR );
    }
    int pkt_len = (uint32_t)strtol(argv[0], NULL, 10);
    int snt = 0;
    while( pkt_len > 0 )
    {
        snt += bsp_UsbBulk_write(tst_Usb_fd, &tst_Usb_dataBuffer[snt], pkt_len);
        pkt_len -= snt;
    }

    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Usb_status( int argc, char** argv )
{
    printf("USB Stats\n");
    printf("\tNum Tx Bytes: %ld\n", tst_Usb_txCnt);
    printf("\tNum Rx Bytes: %ld\n", tst_Usb_rxCnt);
    return( TST_STATUS_OK );
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
// Helper macro to cleanup the table
#define TST_USB_CMD( _uname, _lname ) TST_HANDLER_ELEMENT( TST_USB_##_uname##_STR_CMD, \
                                                           TST_USB_##_uname##_STR_HLP, \
                                                           tst_Usb_##_lname )

/*============================================================================*/
const tst_TableElement_t tst_Usb_menu[] =
{
    TST_USB_CMD( START,  start ),
    TST_USB_CMD( STOP,   stop ),
    TST_USB_CMD( SEND,   send ),
    TST_USB_CMD( STATUS, status ),
    TST_END_ELEMENT
};
