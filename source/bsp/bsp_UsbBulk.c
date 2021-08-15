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
 * @file bsp_UsbBulk.c
 * @brief Contains Functions for hooking into a USB Bulk driver
 */

#include "bsp_Types.h"
#include "bsp_UsbBulk.h"
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
#include "usblib/device/usbdbulk.h"

#include <stdio.h>

#define BSP_USBBULK_DESCRIPTOR_DATA_SIZE    (COMPOSITE_DBULK_SIZE)

/*==============================================================================
 *                                  Types
 *============================================================================*/
/*============================================================================*/
typedef struct
{
    tUSBDBulkDevice*                    deviceInfoPtr;
    bsp_UsbBulk_ConnectionCallback_t    connectionCallback;
    bsp_UsbBulk_DataAvailableCallback_t dataAvailCallback;
    bsp_UsbBulk_TxDoneCallback_t        txDoneCallback;
} bsp_UsbBulk_InternalInfo_t;

#if defined( BSP_PLATFORM_USB_BULK )
/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_UsbBulk_langDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};


/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_UsbBulk_manufacturerString[] =
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
const uint8_t bsp_UsbBulk_productString[] =
{
 (19 + 1) * 2,
 USB_DTYPE_STRING,
 'G', 0, 'e', 0, 'n', 0, 'e', 0, 'r', 0, 'i', 0, 'c', 0, ' ', 0, 'B', 0,
 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0,
 'e', 0
};


/*============================================================================*/
/**
 * The serial number string.
 */
const uint8_t bsp_UsbBulk_serialNumberString[] =
{
    2 + (8 * 2),
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};


/*============================================================================*/
/**
 * The data interface description string.
 */
const uint8_t bsp_UsbBulk_dataInterfaceString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0,
    'a', 0, 'c', 0, 'e', 0
};



/*============================================================================*/
/**
 * The configuration description string.
 */
const uint8_t bsp_UsbBulk_configString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};

/*============================================================================*/
/**
 * The descriptor string table.
 */
const uint8_t* const bsp_UsbBulk_stringDescriptors[] =
{
    bsp_UsbBulk_langDescriptor,
    bsp_UsbBulk_manufacturerString,
    bsp_UsbBulk_productString,
    bsp_UsbBulk_serialNumberString,
    bsp_UsbBulk_dataInterfaceString,
    bsp_UsbBulk_configString
};

#define NUM_STRING_DESCRIPTORS (sizeof(bsp_UsbBulk_stringDescriptors) / sizeof(uint8_t *))


/*============================================================================*/
/**
 * Bulk device callback function prototypes.
 */
uint32_t bsp_UsbBulk_rxHandler( void* pvCBData, uint32_t ui32Event,  uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbBulk_txHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );
uint32_t bsp_UsbBulk_controlHandler( void* pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void* pvMsgData );


/*============================================================================*/
/**
 * dynamic table to hold all the info needed for a USB Bulk device
 */
bsp_UsbBulk_InternalInfo_t bsp_UsbBulk_InternalInfo;


/*============================================================================*/
/**
 * The Bulk device initialization and customization structures. In this case,
 * we are using USBBuffers between the Bulk device class driver and the
 * application code. The function pointers and callback data values are set
 * to insert a buffer in each of the data channels, transmit and receive.
 *
 * With the buffer in place, the Bulk channel callback is set to the relevant
 * channel function and the callback data is set to point to the channel
 * instance data. The buffer, in turn, has its callback set to the application
 * function and the callback data set to our Bulk instance structure.
 */
extern const tUSBBuffer bsp_UsbBulk_txBufferInfo;
extern const tUSBBuffer bsp_UsbBulk_rxBufferInfo;

tUSBDBulkDevice bsp_UsbBulk_deviceInfo =
{
    USB_VID_TI_1CBE,                     // VendorId
    USB_PID_SERIAL,                      // PID
    500,                                 // maxPower in mA
    USB_CONF_ATTR_SELF_PWR,              // power attributes
    USBBufferEventCallback,              // rx event handler
    (void *)&(bsp_UsbBulk_rxBufferInfo), // parameter to pass into rx event handler
    USBBufferEventCallback,              // tx event handler
    (void *)&(bsp_UsbBulk_txBufferInfo), // parameter to pass into tx event handler
    bsp_UsbBulk_stringDescriptors,       // Descriptor strings structure array
    NUM_STRING_DESCRIPTORS               // Number of descriptors
};



/*============================================================================*/
/**
 * Receive buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbBulk_usbRxBuffer0[BSP_PLATFORM_IO_USB0_RX_BUF_LEN];
const tUSBBuffer bsp_UsbBulk_rxBufferInfo =
{
    false,                             // This is a receive buffer.
    bsp_UsbBulk_rxHandler,               // pfnCallback
    (void *)&(bsp_UsbBulk_InternalInfo), // Callback data is our device pointer.
    USBDBulkPacketRead,                // pfnTransfer
    USBDBulkRxPacketAvailable,         // pfnAvailable
    (void *)&(bsp_UsbBulk_deviceInfo),   // pvHandle
    bsp_UsbBulk_usbRxBuffer0,            // pui8Buffer
    sizeof(bsp_UsbBulk_usbRxBuffer0),    // ui32BufferSize
};


/*============================================================================*/
/**
 * Transmit buffer structure (from the USB perspective).
 */
uint8_t bsp_UsbBulk_usbTxBuffer0[BSP_PLATFORM_IO_USB0_TX_BUF_LEN];
const tUSBBuffer bsp_UsbBulk_txBufferInfo =
{
    true,                                // This is a transmit buffer.
    bsp_UsbBulk_txHandler,               // pfnCallback
    (void *)&(bsp_UsbBulk_InternalInfo), // Callback data is our device pointer.
    USBDBulkPacketWrite,                 // pfnTransfer
    USBDBulkTxPacketAvailable,           // pfnAvailable
    (void *)&(bsp_UsbBulk_deviceInfo),   // pvHandle
    bsp_UsbBulk_usbTxBuffer0,            // pui8Buffer
    sizeof(bsp_UsbBulk_usbTxBuffer0),    // ui32BufferSize
};


#endif
//****************************************************************************
//
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//
//****************************************************************************
//uint8_t bsp_UsbData_descriptorData[ BSP_USBBULK_DESCRIPTOR_DATA_SIZE ];

/* Flag to track whether USB is connected or not */
bool bsp_UsbBulk_connected = false;


#if defined( BSP_PLATFORM_USB_BULK )
/*==============================================================================
 *                                Local Functions
 *============================================================================*/
//*****************************************************************************
//
// Handles bulk driver notifications related to the transmit channel (data to
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the bulk driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t bsp_UsbBulk_txHandler( void*    pvCBData,
                                uint32_t ui32Event,
                                uint32_t ui32MsgValue,
                                void*    pvMsgData )
{
    bsp_UsbBulk_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t*)pvCBData;

    switch( ui32Event )
    {
        // Previous packet was sent.
        case USB_EVENT_TX_COMPLETE:
        {
            //ui32MsgValue has the number of bytes transmitted
            if( internalInfoPtr->txDoneCallback != NULL )
            {
                internalInfoPtr->txDoneCallback( ui32MsgValue );
            }
        }
        break;

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
//
// Handles bulk driver notifications related to the receive channel (data from
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the bulk driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t bsp_UsbBulk_rxHandler( void*    pvCBData,
                                uint32_t ui32Event,
                                uint32_t ui32MsgValue,
                                void*    pvMsgData )
{
    uint32_t                    retVal;
    bsp_UsbBulk_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t*)pvCBData;

    switch( ui32Event )
    {
        /* The host has connected. */
        case USB_EVENT_CONNECTED:
        {
            /* Flush our buffers. */
            USBBufferFlush( &bsp_UsbBulk_txBufferInfo );
            USBBufferFlush( &bsp_UsbBulk_rxBufferInfo );

            bsp_UsbBulk_connected = true;
            if( internalInfoPtr->connectionCallback != NULL )
            {
                internalInfoPtr->connectionCallback( bsp_UsbBulk_connected );
            }
            retVal = 0;
            break;
        }

        /* The host has disconnected. */
        case USB_EVENT_DISCONNECTED:
        {
            bsp_UsbBulk_connected = false;
            if( internalInfoPtr->connectionCallback != NULL )
            {
                internalInfoPtr->connectionCallback( bsp_UsbBulk_connected );
            }
            retVal = 0;
            break;
        }

        /* A new packet has been received. */
        case USB_EVENT_RX_AVAILABLE:
        {
            if( internalInfoPtr->dataAvailCallback != NULL )
            {
                internalInfoPtr->dataAvailCallback( USBBufferDataAvailable( (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData) ) );
            }
            retVal = 0;
        }
        break;

        /* Ignore SUSPEND and RESUME for now. */
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            retVal = 0;
            break;

        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(retVal);
}

#endif

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_UsbBulk_init( void )
{
#if defined( BSP_PLATFORM_USB_BULK )
    /* Configure USB pins as input no pull analog */
    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_USB_DP,
                          BSP_GPIO_BIT_MASK_USB_DP,
                          false, //openDrain
                          BSP_GPIO_PULL_NONE );
    bsp_Gpio_configAnalog( BSP_GPIO_PORT_ID_USB_DP, BSP_GPIO_BIT_MASK_USB_DP );

    bsp_Gpio_configInput( BSP_GPIO_PORT_ID_USB_DM,
                          BSP_GPIO_BIT_MASK_USB_DM,
                          false, //openDrain
                          BSP_GPIO_PULL_NONE );
    bsp_Gpio_configAnalog( BSP_GPIO_PORT_ID_USB_DM, BSP_GPIO_BIT_MASK_USB_DM );


    /* Set the USB stack mode to Device mode with no VBUS monitoring. */
    USBStackModeSet( 0, eUSBModeForceDevice, NULL );

    /* Tell the USB library the CPU clock and the PLL frequency. */
#if defined( BSP_PLATFORM_PROCESSOR_TM4C129 )
    uint32_t feature = bsp_Clk_sysClkGet();
    USBDCDFeatureSet( 0, USBLIB_FEATURE_CPUCLK, &feature );
    feature = bsp_Clk_vcoFreqGet();
    USBDCDFeatureSet( 0, USBLIB_FEATURE_USBPLL, &feature );
#endif

    /* Initialize the transmit and receive buffers. */
    USBBufferInit( (tUSBBuffer*)&(bsp_UsbBulk_txBufferInfo) );
    USBBufferInit( (tUSBBuffer*)&(bsp_UsbBulk_rxBufferInfo) );

    /* Initialize the device */
    USBDBulkInit( 0, &(bsp_UsbBulk_deviceInfo) );
#endif
    return;
}


/*============================================================================*/
void
bsp_UsbBulk_registerCallbackDataAvailable( int fd, bsp_UsbBulk_DataAvailableCallback_t callback )
{
    ((bsp_UsbBulk_InternalInfo_t*)fd)->dataAvailCallback = callback;
    return;
}


/*============================================================================*/
bool
bsp_UsbBulk_registerCallbackConnection( int fd, bsp_UsbBulk_ConnectionCallback_t callback )
{
    ((bsp_UsbBulk_InternalInfo_t*)fd)->connectionCallback = callback;
    return( bsp_UsbBulk_connected );
}


/*============================================================================*/
void
bsp_UsbBulk_registerTxDoneCallback( int fd, bsp_UsbBulk_TxDoneCallback_t callback )
{
    ((bsp_UsbBulk_InternalInfo_t*)fd)->txDoneCallback = callback;
    return;
}


/*============================================================================*/
int
bsp_UsbBulk_open( const char*  path,
                  unsigned int flags,
                  int          llv_fd )
{
#if defined( BSP_PLATFORM_USB_BULK )
    return( (int)&(bsp_UsbBulk_InternalInfo) );
#else
    return( 0 );
#endif
}


/*============================================================================*/
int
bsp_UsbBulk_close( int fd )
{
    return( 0 );
}


/*============================================================================*/
int
bsp_UsbBulk_read( int    fd,
                  char*  buffer,
                  size_t count )
{
    bsp_UsbBulk_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t*)fd;
    return( USBBufferRead((tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData), (uint8_t*)buffer, count) );
}


/*============================================================================*/
int
bsp_UsbBulk_write( int         fd,
                   const char* buffer,
                   size_t      count )
{
    bsp_UsbBulk_InternalInfo_t* internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t*)fd;
    return( USBBufferWrite( (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvTxCBData), (uint8_t*)buffer, count ) );
}


/*============================================================================*/
void
bsp_UsbBulk_interruptHandler( void )
{
    extern void USB0DeviceIntHandler(void);
    BSP_TRACE_USBIO_INT_ENTER();
    USB0DeviceIntHandler();
    BSP_TRACE_USBIO_INT_EXIT();
    return;
}


