/*============================================================================*/
/**
 * @file bsp_UsbIo.c
 * @brief Contains Functions for hooking into a USB CDC driver
 */

#include "bsp_Types.h"
#include "bsp_UsbIo.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Assert.h"
#include "bsp_Interrupt.h"

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
    bsp_UsbIo_DataAvailableCallback_t dataAvailCallback;
    FILE                              file;
} bsp_UartIo_InternalInfo_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_UsbIo_langDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};


/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_UsbIo_manufacturerString[] =
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
const uint8_t bsp_UsbIo_productString[] =
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
const uint8_t bsp_UsbIo_pui8SerialNumberString[] =
{
    2 + (8 * 2),
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};


/*============================================================================*/
/**
 * The control interface description string.
 */
const uint8_t bsp_UsbIo_controlInterfaceString[] =
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
const uint8_t bsp_UsbIo_configString[] =
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
const uint8_t* const bsp_UsbIo_stringDescriptors[] =
{
    bsp_UsbIo_langDescriptor,
    bsp_UsbIo_manufacturerString,
    bsp_UsbIo_productString,
    bsp_UsbIo_pui8SerialNumberString,
    bsp_UsbIo_controlInterfaceString,
    bsp_UsbIo_configString
};

#define NUM_STRING_DESCRIPTORS (sizeof(bsp_UsbIo_stringDescriptors) / sizeof(uint8_t *))


/*============================================================================*/
/**
 * CDC device callback function prototypes.
 */
uint32_t bsp_UsbIo_rxHandler( void* pvCBData, uint32_t ui32Event,  uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbIo_txHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbIo_controlHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );


/*============================================================================*/
/**
 * dynamic table to hold all the info needed for a USB CDC device
 */
bsp_UartIo_InternalInfo_t bsp_UsbIo_InternalInfoTable[ BSP_PLATFORM_IO_USB_NUM ];


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
extern const tUSBBuffer bsp_UsbIo_txBufferInfoTable[ BSP_PLATFORM_IO_USB_NUM ];
extern const tUSBBuffer bsp_UsbIo_rxBufferInfoTable[ BSP_PLATFORM_IO_USB_NUM ];

tUSBDCDCDevice bsp_UsbIo_deviceInfoTable[ BSP_PLATFORM_IO_USB_NUM ] =
{
    {
        USB_VID_TI_1CBE,                            // VendorId
        USB_PID_SERIAL,                             // PID
        0,                                          // maxPower in mA
        USB_CONF_ATTR_SELF_PWR,                     // power attributes
        bsp_UsbIo_controlHandler,                   // control event handler
        (void *)&(bsp_UsbIo_InternalInfoTable[0]),  // parameter to pass into control event handler
        USBBufferEventCallback,                     // rx event handler
        (void *)&(bsp_UsbIo_rxBufferInfoTable[0]),  // parameter to pass into rx event handler
        USBBufferEventCallback,                     // tx event handler
        (void *)&(bsp_UsbIo_txBufferInfoTable[0]),  // parameter to pass into tx event handler
        bsp_UsbIo_stringDescriptors,                // Descriptor strings structure array
        NUM_STRING_DESCRIPTORS                      // Number of descriptors
    },
#if( BSP_PLATFORM_IO_USB_NUM > 1 )
    {
        USB_VID_TI_1CBE,                            // VendorId
        USB_PID_SERIAL,                             // PID
        0,                                          // maxPower in mA
        USB_CONF_ATTR_SELF_PWR,                     // power attributes
        bsp_UsbIo_controlHandler,                   // control event handler
        (void *)&(bsp_UsbIo_InternalInfoTable[1]),  // parameter to pass into control event handler
        USBBufferEventCallback,                     // rx event handler
        (void *)&(bsp_UsbIo_rxBufferInfoTable[1]),  // parameter to pass into rx event handler
        USBBufferEventCallback,                     // tx event handler
        (void *)&(bsp_UsbIo_txBufferInfoTable[1]),  // parameter to pass into tx event handler
        bsp_UsbIo_stringDescriptors,                // Descriptor strings structure array
        NUM_STRING_DESCRIPTORS                      // Number of descriptors
    }
#endif
};


/*============================================================================*/
/**
 * Receive buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbIo_usbRxBuffer0[BSP_PLATFORM_IO_USB0_RX_BUF_LEN];
uint8_t bsp_UsbIo_rxBufferWorkspace0[USB_BUFFER_WORKSPACE_SIZE];
#if( BSP_PLATFORM_IO_USB_NUM > 1 )
uint8_t bsp_UsbIo_usbRxBuffer1[BSP_PLATFORM_IO_USB1_RX_BUF_LEN];
uint8_t bsp_UsbIo_rxBufferWorkspace1[USB_BUFFER_WORKSPACE_SIZE];
#endif
const tUSBBuffer bsp_UsbIo_rxBufferInfoTable[ BSP_PLATFORM_IO_USB_NUM ] =
{
    {
        false,                                     // This is a receive buffer.
        bsp_UsbIo_rxHandler,                       // pfnCallback
        (void *)&(bsp_UsbIo_InternalInfoTable[0]), // Callback data is our device pointer.
        USBDCDCPacketRead,                         // pfnTransfer
        USBDCDCRxPacketAvailable,                  // pfnAvailable
        (void *)&(bsp_UsbIo_deviceInfoTable[0]),   // pvHandle
        bsp_UsbIo_usbRxBuffer0,                    // pui8Buffer
        sizeof(bsp_UsbIo_usbRxBuffer0),            // ui32BufferSize
        bsp_UsbIo_rxBufferWorkspace0               // pvWorkspace
    },
#if( BSP_PLATFORM_IO_USB_NUM > 1 )
    {
        false,                                     // This is a receive buffer.
        bsp_UsbIo_rxHandler,                       // pfnCallback
        (void *)&(bsp_UsbIo_InternalInfoTable[1]), // Callback data is our device pointer.
        USBDCDCPacketRead,                         // pfnTransfer
        USBDCDCRxPacketAvailable,                  // pfnAvailable
        (void *)&(bsp_UsbIo_deviceInfoTable[1]),   // pvHandle
        bsp_UsbIo_usbRxBuffer1,                    // pui8Buffer
        sizeof(bsp_UsbIo_usbRxBuffer1),            // ui32BufferSize
        bsp_UsbIo_rxBufferWorkspace1               // pvWorkspace
    }
#endif
};

/*============================================================================*/
/**
 * Transmit buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbIo_usbTxBuffer0[BSP_PLATFORM_IO_USB0_TX_BUF_LEN];
uint8_t bsp_UsbIo_txBufferWorkspace0[USB_BUFFER_WORKSPACE_SIZE];
#if( BSP_PLATFORM_IO_USB_NUM > 1 )
uint8_t bsp_UsbIo_usbTxBuffer1[BSP_PLATFORM_IO_USB1_TX_BUF_LEN];
uint8_t bsp_UsbIo_txBufferWorkspace1[USB_BUFFER_WORKSPACE_SIZE];
#endif
const tUSBBuffer bsp_UsbIo_txBufferInfoTable[ BSP_PLATFORM_IO_USB_NUM ] =
{
    {
        true,                                      // This is a transmit buffer.
        bsp_UsbIo_txHandler,                       // pfnCallback
        (void *)&(bsp_UsbIo_InternalInfoTable[0]), // Callback data is our device pointer.
        USBDCDCPacketWrite,                        // pfnTransfer
        USBDCDCTxPacketAvailable,                  // pfnAvailable
        (void *)&(bsp_UsbIo_deviceInfoTable[0]),   // pvHandle
        bsp_UsbIo_usbTxBuffer0,                    // pui8Buffer
        sizeof(bsp_UsbIo_usbTxBuffer0),            // ui32BufferSize
        bsp_UsbIo_txBufferWorkspace0               // pvWorkspace
    },
#if( BSP_PLATFORM_IO_USB_NUM > 1 )
    {
        true,                                      // This is a transmit buffer.
        bsp_UsbIo_txHandler,                       // pfnCallback
        (void *)&(bsp_UsbIo_InternalInfoTable[1]), // Callback data is our device pointer.
        USBDCDCPacketWrite,                        // pfnTransfer
        USBDCDCTxPacketAvailable,                  // pfnAvailable
        (void *)&(bsp_UsbIo_deviceInfoTable[1]),   // pvHandle
        bsp_UsbIo_usbTxBuffer1,                    // pui8Buffer
        sizeof(bsp_UsbIo_usbTxBuffer1),            // ui32BufferSize
        bsp_UsbIo_txBufferWorkspace1               // pvWorkspace
    }
#endif
};



//****************************************************************************
//
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//
//****************************************************************************
uint8_t bsp_UsbIo_descriptorData[ BSP_USBIO_DESCRIPTOR_DATA_SIZE ];

tCompositeEntry bsp_UsbIo_compositeEntries[ BSP_PLATFORM_IO_USB_NUM ];

//****************************************************************************
//
// Allocate the Device Data for the top level composite device class.
//
//****************************************************************************
tUSBDCompositeDevice bsp_UsbIo_compositeDevice =
{
    USB_VID_TI_1CBE,               // Stellaris VID.
    USB_PID_COMP_SERIAL,           // Stellaris PID for composite serial device.
    250,                           // This is in 2mA increments so 500mA.
    USB_CONF_ATTR_BUS_PWR,         // Bus powered device.
    0,                             // There is no need for a default composite event handler.
    bsp_UsbIo_stringDescriptors,   // The string table.
    NUM_STRING_DESCRIPTORS,        // The string table len
    DIM(bsp_UsbIo_compositeEntries), // Composite device array len
    bsp_UsbIo_compositeEntries       // The Composite device array.
};


/*==============================================================================
 *                                Local Functions
 *============================================================================*/
//*****************************************************************************
// Set the state of the RS232 RTS and DTR signals.
//*****************************************************************************
static void bsp_UsbIo_setControlLineState( uint16_t ui16State )
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
static bool bsp_UsbIo_setLineCoding( tLineCoding *psLineCoding )
{
    return( true );
}


//*****************************************************************************
// Get the communication parameters in use on the UART.
//*****************************************************************************
static void bsp_UsbIo_getLineCoding( tLineCoding *psLineCoding )
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
static void bsp_UsbIo_sendBreak( bool bSend )
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
uint32_t bsp_UsbIo_controlHandler( void*    pvCBData,
                                   uint32_t ui32Event,
                                   uint32_t ui32MsgValue,
                                   void*    pvMsgData )
{
    bsp_UartIo_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UartIo_InternalInfo_t*)pvCBData;

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
            bsp_UsbIo_getLineCoding( pvMsgData );
            break;

        /* Set the current serial communication parameters. */
        case USBD_CDC_EVENT_SET_LINE_CODING:
            bsp_UsbIo_setLineCoding( pvMsgData );
            break;

        /* Set the current serial communication parameters. */
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            bsp_UsbIo_setControlLineState( (uint16_t)ui32MsgValue );
            break;

        /* Send a break condition on the serial line. */
        case USBD_CDC_EVENT_SEND_BREAK:
            bsp_UsbIo_sendBreak( true );
            break;

        /* Clear the break condition on the serial line. */
        case USBD_CDC_EVENT_CLEAR_BREAK:
            bsp_UsbIo_sendBreak( false );
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
uint32_t bsp_UsbIo_txHandler( void*    pvCBData,
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
uint32_t bsp_UsbIo_rxHandler( void*    pvCBData,
                              uint32_t ui32Event,
                              uint32_t ui32MsgValue,
                              void*    pvMsgData )
{
    uint32_t                   retVal;
    bsp_UartIo_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UartIo_InternalInfo_t*)pvCBData;

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
void
bsp_UsbIo_init( void )
{
    uint8_t i;
    char filenameBuf[5] = "usbX";

    /* Configure USB pins as input no pull */
    bsp_Gpio_configInput(  BSP_GPIO_PORT_ID_USB_DP,
                           (BSP_GPIO_BIT_MASK_USB_DP | BSP_GPIO_BIT_MASK_USB_DM),
                           false, //openDrain
                           BSP_GPIO_PULL_NONE );

    bsp_Gpio_configInput(  BSP_GPIO_PORT_ID_USB_VB,
                           (BSP_GPIO_BIT_MASK_USB_VB),
                           false, //openDrain
                           BSP_GPIO_PULL_NONE );

    /* Configure USB pins to analog alternative function */
    bsp_Gpio_configAltFunction( BSP_GPIO_PORT_ID_USB_DP,
                                (BSP_GPIO_BIT_MASK_USB_DP | BSP_GPIO_BIT_MASK_USB_DM),
                                true, //analog
                                0 );

    bsp_Gpio_configAltFunction( BSP_GPIO_PORT_ID_USB_VB,
                                (BSP_GPIO_BIT_MASK_USB_VB),
                                true, //analog
                                0 );

    for( i=0; i<BSP_PLATFORM_IO_USB_NUM; i++ )
    {
        bsp_UsbIo_InternalInfoTable[i].deviceInfoPtr = &(bsp_UsbIo_deviceInfoTable[i]);
        bsp_UsbIo_InternalInfoTable[i].dataAvailCallback = NULL;

        snprintf( filenameBuf, sizeof(filenameBuf), "usb%d", i );

        /* Add IO device to be used in stdio */
        add_device( filenameBuf,
                    _MSA, /* Single stream open at a time (_MSA is for multiple) */
                    bsp_UsbIo_open,
                    bsp_UsbIo_close,
                    bsp_UsbIo_read,
                    bsp_UsbIo_write,
                    bsp_UsbIo_lseek,
                    bsp_UsbIo_unlink,
                    bsp_UsbIo_rename );

        bsp_UsbIo_InternalInfoTable[i].file.fd        = (int)&(bsp_UsbIo_InternalInfoTable[i]);
        bsp_UsbIo_InternalInfoTable[i].file.pos       = NULL;
        bsp_UsbIo_InternalInfoTable[i].file.bufend    = NULL;
        bsp_UsbIo_InternalInfoTable[i].file.buff_stop = NULL;
        bsp_UsbIo_InternalInfoTable[i].file.flags     = 0;

        /* Open the usb file for reading/writing */
        fopen( filenameBuf,"rw" );

        /* Initialize the transmit and receive buffers. */
        USBBufferInit( &(bsp_UsbIo_txBufferInfoTable[i]) );
        USBBufferInit( &(bsp_UsbIo_rxBufferInfoTable[i]) );

        /* Initialize the composite device */
        bsp_UsbIo_compositeDevice.psDevices[i].pvInstance =
            USBDCDCCompositeInit( 0, &(bsp_UsbIo_deviceInfoTable[i]), &(bsp_UsbIo_compositeEntries[i]) );

    }

    /* Set the USB stack mode to Device mode with no VBUS monitoring. */
    USBStackModeSet( NULL, eUSBModeForceDevice, NULL );

    /* Pass our device information to the USB library and place the device on the bus. */
    USBDCompositeInit( 0, &bsp_UsbIo_compositeDevice,
                       sizeof(bsp_UsbIo_descriptorData),
                       bsp_UsbIo_descriptorData );

    return;
}


/*============================================================================*/
void
bsp_UsbIo_registerDataAvailableCallback( int                               file_descriptor,
                                         bsp_UsbIo_DataAvailableCallback_t callback )
{
    bsp_UartIo_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UartIo_InternalInfo_t*)file_descriptor;
    internalInfoPtr->dataAvailCallback = callback;

    return;
}


/*============================================================================*/
int
bsp_UsbIo_open( const char*  path,
                unsigned int flags,
                int          llv_fd )
{
    uint8_t pathLen;
    int ret;

    pathLen = strlen(path);

    /* Todo: Do something smarter here */
    if( path[(pathLen-1)] == '0' )
    {
        ret = (int)&(bsp_UsbIo_InternalInfoTable[0]);
    }
    else
    {
        ret = (int)&(bsp_UsbIo_InternalInfoTable[1]);
    }

    return( ret );
}


/*============================================================================*/
int
bsp_UsbIo_close( int file_descriptor )
{
    return( 0 );
}


/*============================================================================*/
int
bsp_UsbIo_read( int    file_descriptor,
                char*  buffer,
                size_t count )
{
    bsp_UartIo_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UartIo_InternalInfo_t*)file_descriptor;

    return( USBBufferRead((tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData), (uint8_t*)buffer, count) );
}


/*============================================================================*/
int
bsp_UsbIo_write( int         file_descriptor,
                 const char* buffer,
                 size_t      count )
{
    bsp_UartIo_InternalInfo_t* internalInfoPtr;

    internalInfoPtr = (bsp_UartIo_InternalInfo_t*)file_descriptor;

    return( USBBufferWrite( (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvTxCBData), (uint8_t*)buffer, count ) );
}


/*============================================================================*/
off_t
bsp_UsbIo_lseek( int   file_descriptor,
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


/*============================================================================*/
void
bsp_UsbIo_interruptHandler( void )
{
    extern void USB0DeviceIntHandler(void);
    BSP_TRACE_USBIO_INT_ENTER();
    USB0DeviceIntHandler();
    BSP_TRACE_USBIO_INT_EXIT();
    return;
}


