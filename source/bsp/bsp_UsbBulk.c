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
 * @file bsp_UsbBulk.c
 * @brief Contains Functions for hooking into a USB Bulk driver
 */

#include "bsp_Platform.h"
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
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdbulk.h"

#include <stdio.h>

/*==============================================================================
 *                                  Types
 *============================================================================*/
/*============================================================================*/
typedef struct {
    tUSBDBulkDevice *                   deviceInfoPtr;
    bsp_UsbBulk_ConnectionCallback_t    connectionCallback;
    bsp_UsbBulk_DataAvailableCallback_t dataAvailCallback;
    bsp_UsbBulk_TxDoneCallback_t        txDoneCallback;
} bsp_UsbBulk_InternalInfo_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/* clang-format off */
/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_UsbBulk_langDescriptor[] = {4, USB_DTYPE_STRING, USBShort(USB_LANG_EN_US)};

/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_UsbBulk_manufacturerString[] =
#if defined(BSP_PLATFORM_USB_DEVICE_MANUFACTURER_STR)
BSP_PLATFORM_USB_DEVICE_MANUFACTURER_STR
#else
{
    2 + (17 * 2),
    USB_DTYPE_STRING,
    'T', 0,
    'e', 0,
    'x', 0,
    'a', 0,
    's', 0,
    ' ', 0,
    'I', 0,
    'n', 0,
    's', 0,
    't', 0,
    'r', 0,
    'u', 0,
    'm', 0,
    'e', 0,
    'n', 0,
    't', 0,
    's', 0,
};
#endif

/*============================================================================*/
/**
 * The product string.
 */
const uint8_t bsp_UsbBulk_productString[] =
#if defined(BSP_PLATFORM_USB_DEVICE_PRODUCT_STR)
BSP_PLATFORM_USB_DEVICE_PRODUCT_STR
#else
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'G', 0,
    'e', 0,
    'n', 0,
    'e', 0,
    'r', 0,
    'i', 0,
    'c', 0,
    ' ', 0,
    'B', 0,
    'u', 0,
    'l', 0,
    'k', 0,
    ' ', 0,
    'D', 0,
    'e', 0,
    'v', 0,
    'i', 0,
    'c', 0,
    'e', 0
};
#endif

/*============================================================================*/
/**
 * The serial number string.
 * Not constant because it gets updated at runtime
 */
uint8_t bsp_UsbBulk_serialNumberString[] =
{
    2 + (12 * 2),
    USB_DTYPE_STRING,
    '1', 0,
    '2', 0,
    '3', 0,
    '4', 0,
    '5', 0,
    '6', 0,
    '7', 0,
    '8', 0,
    '9', 0,
    'A', 0,
    'B', 0,
    'C', 0
};

/*============================================================================*/
/**
 * The data interface description string.
 */
const uint8_t bsp_UsbBulk_dataInterfaceString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0,
    'u', 0,
    'l', 0,
    'k', 0,
    ' ', 0,
    'D', 0,
    'a', 0,
    't', 0,
    'a', 0,
    ' ', 0,
    'I', 0,
    'n', 0,
    't', 0,
    'e', 0,
    'r', 0,
    'f', 0,
    'a', 0,
    'c', 0,
    'e', 0
};

/*============================================================================*/
/**
 * The configuration description string.
 */
const uint8_t bsp_UsbBulk_configString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0,
    'u', 0,
    'l', 0,
    'k', 0,
    ' ', 0,
    'D', 0,
    'a', 0,
    't', 0,
    'a', 0,
    ' ', 0,
    'C', 0,
    'o', 0,
    'n', 0,
    'f', 0,
    'i', 0,
    'g', 0,
    'u', 0,
    'r', 0,
    'a', 0,
    't', 0,
    'i', 0,
    'o', 0,
    'n', 0
};
/* clang-format on */
/*============================================================================*/
/**
 * The descriptor string table.
 */
const uint8_t *const bsp_UsbBulk_stringDescriptors[] = {
    bsp_UsbBulk_langDescriptor,
    bsp_UsbBulk_manufacturerString,
    bsp_UsbBulk_productString,
    bsp_UsbBulk_serialNumberString,
    bsp_UsbBulk_dataInterfaceString,
    bsp_UsbBulk_configString};

#define NUM_STRING_DESCRIPTORS (sizeof(bsp_UsbBulk_stringDescriptors) / sizeof(uint8_t *))

/*============================================================================*/
/**
 * Bulk device callback function prototypes.
 */
uint32_t
bsp_UsbBulk_rxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData);
uint32_t
bsp_UsbBulk_txHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData);
uint32_t bsp_UsbBulk_controlHandler(
    void *   pvCBData,
    uint32_t ui32Event,
    uint32_t ui32MsgValue,
    void *   pvMsgData);

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
extern tUSBBuffer bsp_UsbBulk_txBufferInfo;
extern tUSBBuffer bsp_UsbBulk_rxBufferInfo;

tUSBDBulkDevice bsp_UsbBulk_deviceInfo = {
    USB_VID_TI_1CBE,                        // VendorId
    USB_PID_BULK,                           // PID
    500,                                    // maxPower in mA
    USB_CONF_ATTR_SELF_PWR,                 // power attributes
    USBBufferEventCallback,                 // rx event handler
    (void *)&(bsp_UsbBulk_rxBufferInfo),    // parameter to pass into rx event handler
    USBBufferEventCallback,                 // tx event handler
    (void *)&(bsp_UsbBulk_txBufferInfo),    // parameter to pass into tx event handler
    bsp_UsbBulk_stringDescriptors,          // Descriptor strings structure array
    NUM_STRING_DESCRIPTORS                  // Number of descriptors
};

/*============================================================================*/
/**
 * Receive buffer structure (from the USB perspective).
 */
uint8_t    bsp_UsbBulk_usbRxBuffer0[BSP_PLATFORM_IO_USB0_RX_BUF_LEN];
tUSBBuffer bsp_UsbBulk_rxBufferInfo = {
    false,                                  // This is a receive buffer.
    bsp_UsbBulk_rxHandler,                  // pfnCallback
    (void *)&(bsp_UsbBulk_InternalInfo),    // Callback data is our device pointer.
    USBDBulkPacketRead,                     // pfnTransfer
    USBDBulkRxPacketAvailable,              // pfnAvailable
    (void *)&(bsp_UsbBulk_deviceInfo),      // pvHandle
    bsp_UsbBulk_usbRxBuffer0,               // pui8Buffer
    sizeof(bsp_UsbBulk_usbRxBuffer0),       // ui32BufferSize
};

/*============================================================================*/
/**
 * Transmit buffer structure (from the USB perspective).
 */
uint8_t    bsp_UsbBulk_usbTxBuffer0[BSP_PLATFORM_IO_USB0_TX_BUF_LEN];
tUSBBuffer bsp_UsbBulk_txBufferInfo = {
    true,                                   // This is a transmit buffer.
    bsp_UsbBulk_txHandler,                  // pfnCallback
    (void *)&(bsp_UsbBulk_InternalInfo),    // Callback data is our device pointer.
    USBDBulkPacketWrite,                    // pfnTransfer
    USBDBulkTxPacketAvailable,              // pfnAvailable
    (void *)&(bsp_UsbBulk_deviceInfo),      // pvHandle
    bsp_UsbBulk_usbTxBuffer0,               // pui8Buffer
    sizeof(bsp_UsbBulk_usbTxBuffer0),       // ui32BufferSize
};

/* Flag to track whether USB is connected or not */
bool bsp_UsbBulk_connected = false;

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
uint32_t
bsp_UsbBulk_txHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData)
{
    bsp_UsbBulk_InternalInfo_t *internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t *)pvCBData;

    switch (ui32Event) {
        // Previous packet was sent.
        case USB_EVENT_TX_COMPLETE: {
            // ui32MsgValue has the number of bytes transmitted
            if ((internalInfoPtr != NULL) && (internalInfoPtr->txDoneCallback != NULL)) {
                internalInfoPtr->txDoneCallback(ui32MsgValue);
            }
        } break;

        default:
#ifdef DEBUG
            while (1)
                ;
#else
            break;
#endif
    }
    return (0);
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
uint32_t
bsp_UsbBulk_rxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData)
{
    uint32_t                    retVal;
    bsp_UsbBulk_InternalInfo_t *internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t *)pvCBData;

    switch (ui32Event) {
        /* The host has connected. */
        case USB_EVENT_CONNECTED: {
            /* Flush our buffers. */
            USBBufferFlush(&bsp_UsbBulk_txBufferInfo);
            USBBufferFlush(&bsp_UsbBulk_rxBufferInfo);

            bsp_UsbBulk_connected = true;
            if (internalInfoPtr->connectionCallback != NULL) {
                internalInfoPtr->connectionCallback(bsp_UsbBulk_connected);
            }
            retVal = 0;
            break;
        }

        /* The host has disconnected. */
        case USB_EVENT_DISCONNECTED: {
            bsp_UsbBulk_connected = false;
            if (internalInfoPtr->connectionCallback != NULL) {
                internalInfoPtr->connectionCallback(bsp_UsbBulk_connected);
            }
            retVal = 0;
            break;
        }

        /* A new packet has been received. */
        case USB_EVENT_RX_AVAILABLE: {
            if (internalInfoPtr->dataAvailCallback != NULL) {
                internalInfoPtr->dataAvailCallback(USBBufferDataAvailable(
                    (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData)));
            }
            retVal = 0;
        } break;

        /* Ignore SUSPEND and RESUME for now. */
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            retVal = 0;
            break;

        default:
#ifdef DEBUG
            while (1)
                ;
#else
            break;
#endif
    }

    return (retVal);
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void *bsp_UsbBulk_init(void *compositeEntry)
{
    bsp_UsbBulk_InternalInfo.deviceInfoPtr      = &(bsp_UsbBulk_deviceInfo);
    bsp_UsbBulk_InternalInfo.dataAvailCallback  = NULL;
    bsp_UsbBulk_InternalInfo.connectionCallback = NULL;
    bsp_UsbBulk_InternalInfo.txDoneCallback     = NULL;

    /* Initialize the transmit and receive buffers. */
    USBBufferInit(&(bsp_UsbBulk_txBufferInfo));
    USBBufferInit(&(bsp_UsbBulk_rxBufferInfo));

    /* Initialize the device */
    return (USBDBulkCompositeInit(0, &(bsp_UsbBulk_deviceInfo), (tCompositeEntry *)compositeEntry));
}

/*============================================================================*/
bsp_UsbBulk_DataAvailableCallback_t bsp_UsbBulk_registerCallbackDataAvailable(
    int                                 fd,
    bsp_UsbBulk_DataAvailableCallback_t callback)
{
    bsp_UsbBulk_DataAvailableCallback_t oldCb;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    oldCb = ((bsp_UsbBulk_InternalInfo_t *)fd)->dataAvailCallback;
    ((bsp_UsbBulk_InternalInfo_t *)fd)->dataAvailCallback = callback;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return (oldCb);
}

/*============================================================================*/
bsp_UsbBulk_ConnectionCallback_t bsp_UsbBulk_registerCallbackConnection(
    int                              fd,
    bsp_UsbBulk_ConnectionCallback_t callback)
{
    bsp_UsbBulk_ConnectionCallback_t oldCb;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    oldCb = ((bsp_UsbBulk_InternalInfo_t *)fd)->connectionCallback;
    ((bsp_UsbBulk_InternalInfo_t *)fd)->connectionCallback = callback;
    if (callback != NULL) {
        callback(bsp_UsbBulk_connected);
    }
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return (oldCb);
}

/*============================================================================*/
bsp_UsbBulk_TxDoneCallback_t bsp_UsbBulk_registerCallbackTxDone(
    int                          fd,
    bsp_UsbBulk_TxDoneCallback_t callback)
{
    bsp_UsbBulk_TxDoneCallback_t oldCb;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    oldCb = ((bsp_UsbBulk_InternalInfo_t *)fd)->txDoneCallback;
    ((bsp_UsbBulk_InternalInfo_t *)fd)->txDoneCallback = callback;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return (oldCb);
}

/*============================================================================*/
int bsp_UsbBulk_open(void)
{
    return ((int)&(bsp_UsbBulk_InternalInfo));
}

/*============================================================================*/
int bsp_UsbBulk_close(int fd)
{
    return (0);
}

/*============================================================================*/
int bsp_UsbBulk_read(int fd, void *buffer, size_t count)
{
    bsp_UsbBulk_InternalInfo_t *internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t *)fd;
    return (USBBufferRead(
        (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvRxCBData), (uint8_t *)buffer, count));
}

/*============================================================================*/
int bsp_UsbBulk_write(int fd, const void *buffer, size_t count)
{
    bsp_UsbBulk_InternalInfo_t *internalInfoPtr;
    internalInfoPtr = (bsp_UsbBulk_InternalInfo_t *)fd;
    return (USBBufferWrite(
        (tUSBBuffer *)(internalInfoPtr->deviceInfoPtr->pvTxCBData), (uint8_t *)buffer, count));
}
