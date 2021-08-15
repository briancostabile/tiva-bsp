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
 * @file bsp_UsbCdc.c
 * @brief Contains Functions for hooking into a USB CDC driver
 */

#include "bsp_Types.h"
#include "bsp_UsbCdc.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Assert.h"
#include "bsp_Interrupt.h"
#include "bsp_Io.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdcdc.h"

#include <stdio.h>

#define BSP_USBIO_DESCRIPTOR_DATA_SIZE    (COMPOSITE_DCDC_SIZE + COMPOSITE_DCDC_SIZE)

/*==============================================================================
 *                                  Types
 *============================================================================*/
/*============================================================================*/
/*
 * Flag to track whether or not Rx is currently enabled
 * Flag to track whether or not Tx is currently enabled
 *
 * Global used to track the amount of data sent to the USART driver each time
 * data is sent. This is not a running count of the num bytes sent in the
 * previous call to the USART driver. This is needed so the serial component
 * knows how much to free from the tx buffer once the USART calls the callback
 * indicating that the transmit was completed.
 *
 * Global used to track the amount of tx data dropped due to buffer overflows
 *
 * Global to hold the read callback function pointer.
 */
typedef struct
{
    tUSBDCDCDevice*                   deviceInfoPtr;
    bsp_UsbCdc_DataAvailableCallback_t dataAvailCallback;
} bsp_UsbCdc_InternalInfo_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_UsbCdc_langDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};


/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_UsbCdc_manufacturerString[] =
{
    2 + (17 * 2),
    USB_DTYPE_STRING,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0,
};


/*============================================================================*/
/**
 * The product string.
 */
const uint8_t bsp_UsbCdc_productString[] =
{
    2 + (16 * 2),
    USB_DTYPE_STRING,
    'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0,
    'C', 0, 'O', 0, 'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0
};


/*============================================================================*/
/**
 * The serial number string.
 */
const uint8_t bsp_UsbCdc_pui8SerialNumberString[] =
{
    2 + (8 * 2),
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};


/*============================================================================*/
/**
 * The control interface description string.
 */
const uint8_t bsp_UsbCdc_controlInterfaceString[] =
{
    2 + (21 * 2),
    USB_DTYPE_STRING,
    'A', 0, 'C', 0, 'M', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 't', 0,
    'r', 0, 'o', 0, 'l', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};


/*============================================================================*/
/**
 * The configuration description string.
 */
const uint8_t bsp_UsbCdc_configString[] =
{
    2 + (26 * 2),
    USB_DTYPE_STRING,
    'S', 0, 'e', 0, 'l', 0, 'f', 0, ' ', 0, 'P', 0, 'o', 0, 'w', 0,
    'e', 0, 'r', 0, 'e', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
    'o', 0, 'n', 0
};

/*============================================================================*/
/**
 * The descriptor string table.
 */
const uint8_t* const bsp_UsbCdc_stringDescriptors[] =
{
    bsp_UsbCdc_langDescriptor,
    bsp_UsbCdc_manufacturerString,
    bsp_UsbCdc_productString,
    bsp_UsbCdc_pui8SerialNumberString,
    bsp_UsbCdc_controlInterfaceString,
    bsp_UsbCdc_configString
};

#define NUM_STRING_DESCRIPTORS DIM(bsp_UsbCdc_stringDescriptors)


/*============================================================================*/
/**
 * CDC device callback function prototypes.
 */
uint32_t bsp_UsbCdc_rxHandler( void* pvCBData, uint32_t ui32Event,  uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbCdc_txHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbCdc_controlHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );


/*============================================================================*/
/**
 * dynamic table to hold all the info needed for a USB CDC device
 */
bsp_UsbCdc_InternalInfo_t bsp_UsbCdc_InternalInfo;


/*============================================================================*/
/**
 * The CDC device initialization and customization structures. In this case,
 * we are using USBBuffers between the CDC device class driver and the
 * application code. The function pointers and callback data values are set
 * to insert a buffer in each of the data channels, transmit and receive.
 *
 * With the buffer in place, the CDC channel callback is set to the relevant
 * channel function and the callback data is set to point to the channel
 * instance data. The buffer, in turn, has its callback set to the application
 * function and the callback data set to our CDC instance structure.
 */
extern tUSBBuffer bsp_UsbCdc_txBufferInfo;
extern tUSBBuffer bsp_UsbCdc_rxBufferInfo;

tUSBDCDCDevice bsp_UsbCdc_deviceInfo =
{
    USB_VID_TI_1CBE,                    // VendorId
    USB_PID_SERIAL,                     // PID
    0,                                  // maxPower in mA
    USB_CONF_ATTR_SELF_PWR,             // power attributes
    bsp_UsbCdc_controlHandler,          // control event handler
    (void *)&(bsp_UsbCdc_InternalInfo), // parameter to pass into control event handler
    USBBufferEventCallback,             // rx event handler
    (void *)&(bsp_UsbCdc_rxBufferInfo), // parameter to pass into rx event handler
    USBBufferEventCallback,             // tx event handler
    (void *)&(bsp_UsbCdc_txBufferInfo), // parameter to pass into tx event handler
    bsp_UsbCdc_stringDescriptors,       // Descriptor strings structure array
    NUM_STRING_DESCRIPTORS              // Number of descriptors
};


/*============================================================================*/
/**
 * Receive buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbCdc_usbRxBuffer0[BSP_PLATFORM_IO_USB0_RX_BUF_LEN];
tUSBBuffer bsp_UsbCdc_rxBufferInfo =
{
    false,                              // This is a receive buffer.
    bsp_UsbCdc_rxHandler,               // pfnCallback
    (void *)&(bsp_UsbCdc_InternalInfo), // Callback data is our device pointer.
    USBDCDCPacketRead,                  // pfnTransfer
    USBDCDCRxPacketAvailable,           // pfnAvailable
    (void *)&(bsp_UsbCdc_deviceInfo),   // pvHandle
    bsp_UsbCdc_usbRxBuffer0,            // pui8Buffer
    sizeof(bsp_UsbCdc_usbRxBuffer0),    // ui32BufferSize
};

/*============================================================================*/
/**
 * Transmit buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbCdc_usbTxBuffer0[BSP_PLATFORM_IO_USB0_TX_BUF_LEN];
tUSBBuffer bsp_UsbCdc_txBufferInfo =
{
    true,                               // This is a transmit buffer.
    bsp_UsbCdc_txHandler,               // pfnCallback
    (void *)&(bsp_UsbCdc_InternalInfo), // Callback data is our device pointer.
    USBDCDCPacketWrite,                 // pfnTransfer
    USBDCDCTxPacketAvailable,           // pfnAvailable
    (void *)&(bsp_UsbCdc_deviceInfo),   // pvHandle
    bsp_UsbCdc_usbTxBuffer0,            // pui8Buffer
    sizeof(bsp_UsbCdc_usbTxBuffer0),    // ui32BufferSize
};


/*==============================================================================
 *                                Local Functions
 *============================================================================*/
//*****************************************************************************
// Set the state of the RS232 RTS and DTR signals.
//*****************************************************************************
static void bsp_UsbCdc_setControlLineState( uint16_t ui16State )
{
    //
    // TODO: If configured with GPIOs controlling the handshake lines,
    // set them appropriately depending upon the flags passed in the wValue
    // field of the request structure passed.
    //
}


//*****************************************************************************
// Set the communication parameters to use on the UART.
//*****************************************************************************
static bool bsp_UsbCdc_setLineCoding( tLineCoding *psLineCoding )
{
    return( true );
}


//*****************************************************************************
// Get the communication parameters in use on the UART.
//*****************************************************************************
static void bsp_UsbCdc_getLineCoding( tLineCoding *psLineCoding )
{
    /* Send back dummy UART parameters */
    psLineCoding->ui32Rate    = 921600;
    psLineCoding->ui8Databits = 8;
    psLineCoding->ui8Parity   = USB_CDC_PARITY_NONE;
    psLineCoding->ui8Stop     = USB_CDC_STOP_BITS_1;

    return;
}


//*****************************************************************************
// This function sets or clears a break condition on the redirected UART RX
// line.  A break is started when the function is called with \e bSend set to
// \b true and persists until the function is called again with \e bSend set
// to \b false.
//*****************************************************************************
static void bsp_UsbCdc_sendBreak( bool bSend )
{
    return;
}


//*****************************************************************************
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//*****************************************************************************
uint32_t bsp_UsbCdc_controlHandler( void*    pvCBData,
                                   uint32_t ui32Event,
                                   uint32_t ui32MsgValue,
                                   void*    pvMsgData )
{
    bsp_UsbCdc_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UsbCdc_InternalInfo_t*)pvCBData;

    /* Which event are we being asked to process? */
    switch( ui32Event )
    {
        /* We are connected to a host and communication is now possible. */
        case USB_EVENT_CONNECTED:
            /* Flush our buffers. */
            USBBufferFlush( (const tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData) );
            USBBufferFlush( (const tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvTxCBData) );
            break;

        /* The host has disconnected. */
        case USB_EVENT_DISCONNECTED:
            break;

        /* Return the current serial communication parameters. */
        case USBD_CDC_EVENT_GET_LINE_CODING:
            bsp_UsbCdc_getLineCoding( pvMsgData );
            break;

        /* Set the current serial communication parameters. */
        case USBD_CDC_EVENT_SET_LINE_CODING:
            bsp_UsbCdc_setLineCoding( pvMsgData );
            break;

        /* Set the current serial communication parameters. */
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            bsp_UsbCdc_setControlLineState( (uint16_t)ui32MsgValue );
            break;

        /* Send a break condition on the serial line. */
        case USBD_CDC_EVENT_SEND_BREAK:
            bsp_UsbCdc_sendBreak( true );
            break;

        /* Clear the break condition on the serial line. */
        case USBD_CDC_EVENT_CLEAR_BREAK:
            bsp_UsbCdc_sendBreak( false );
            break;

        /* Ignore SUSPEND and RESUME for now. */
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        /* We don't expect to receive any other events.  Ignore any that show
         * up in a release build or hang in a debug build.
         */
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }

    return(0);
}

//*****************************************************************************
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param ui32CBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//*****************************************************************************
uint32_t bsp_UsbCdc_txHandler( void*    pvCBData,
                              uint32_t ui32Event,
                              uint32_t ui32MsgValue,
                              void*    pvMsgData )
{
    switch( ui32Event )
    {
        // Previous packet was sent.
        case USB_EVENT_TX_COMPLETE:
        {
        }
        break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }
    return(0);
}

//*****************************************************************************
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param ui32CBData is the client-supplied callback data value for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//*****************************************************************************
uint32_t bsp_UsbCdc_rxHandler( void*    pvCBData,
                              uint32_t ui32Event,
                              uint32_t ui32MsgValue,
                              void*    pvMsgData )
{
    uint32_t                   retVal;
    bsp_UsbCdc_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UsbCdc_InternalInfo_t*)pvCBData;

    switch( ui32Event )
    {
        // A new packet has been received.
        case USB_EVENT_RX_AVAILABLE:
        {
            if( internalInfoPtr->dataAvailCallback != NULL )
            {
                internalInfoPtr->dataAvailCallback( USBBufferDataAvailable( (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData) ) );
            }
            retVal = 0;
        }
        break;

        //
        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            retVal = 0;
        }
        break;

        //
        // We are being asked to provide a buffer into which the next packet
        // can be read. We do not support this mode of receiving data so let
        // the driver know by returning 0. The CDC driver should not be sending
        // this message but this is included just for illustration and
        // completeness.
        //
        case USB_EVENT_REQUEST_BUFFER:
        {
            retVal = 0;
        }
        break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(retVal);
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void*
bsp_UsbCdc_init( void* compositeEntry )
{
    bsp_UsbCdc_InternalInfo.deviceInfoPtr = &(bsp_UsbCdc_deviceInfo);
    bsp_UsbCdc_InternalInfo.dataAvailCallback = NULL;

    /* Initialize the transmit and receive buffers. */
    USBBufferInit( &(bsp_UsbCdc_txBufferInfo) );
    USBBufferInit( &(bsp_UsbCdc_rxBufferInfo) );

    return( USBDCDCCompositeInit( 0, &(bsp_UsbCdc_deviceInfo), (tCompositeEntry *)compositeEntry ) );
}


/*============================================================================*/
bsp_UsbCdc_DataAvailableCallback_t
bsp_UsbCdc_registerCallbackDataAvailable( int fd, bsp_UsbCdc_DataAvailableCallback_t callback )
{
    bsp_UsbCdc_DataAvailableCallback_t oldCb;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    oldCb = ((bsp_UsbCdc_InternalInfo_t*)fd)->dataAvailCallback;
    ((bsp_UsbCdc_InternalInfo_t*)fd)->dataAvailCallback = callback;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return( oldCb );
}


/*============================================================================*/
int
bsp_UsbCdc_open( void )
{
    return( (int)&(bsp_UsbCdc_InternalInfo) );
}


/*============================================================================*/
int
bsp_UsbCdc_close( int fd )
{
    return( 0 );
}


/*============================================================================*/
int
bsp_UsbCdc_read( int    fd,
                char*  buffer,
                size_t count )
{
    bsp_UsbCdc_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbCdc_InternalInfo_t*)fd;
    return( USBBufferRead((tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData), (uint8_t*)buffer, count) );
}


/*============================================================================*/
int
bsp_UsbCdc_write( int         fd,
                 const char* buffer,
                 size_t      count )
{
    bsp_UsbCdc_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbCdc_InternalInfo_t*)fd;
    return( USBBufferWrite( (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvTxCBData), (uint8_t*)buffer, count ) );
}
