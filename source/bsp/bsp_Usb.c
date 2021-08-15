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
 * @file bsp_Usb.c
 * @brief Contains Functions for setting up the USB front end
 */

#include "bsp_Types.h"
#include "bsp_Usb.h"
#include "bsp_UsbBulk.h"
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
#include "usblib/device/usbdbulk.h"
#include "usblib/device/usbdcdc.h"

#include <stdio.h>

/* USB_ENABLED must be defined AND one of the Device Types must also be defined */
#if defined(BSP_PLATFORM_USB_ENABLED) && (defined(BSP_PLATFORM_USB_CDC) || defined(BSP_PLATFORM_USB_BULK))

/* Dynamically (at compile-time) compute the number of devices and the size of the scratch memory */
#if defined(BSP_PLATFORM_USB_CDC)
#define BSP_USB_COMPOSITE_NUM_CDC 1
#define BSP_USB_DESCRIPTOR_DATA_SIZE_CDC COMPOSITE_DCDC_SIZE
#else
#define BSP_USB_COMPOSITE_NUM_CDC 0
#define BSP_USB_DESCRIPTOR_DATA_SIZE_CDC 0
#endif

#if defined(BSP_PLATFORM_USB_BULK)
#define BSP_USB_COMPOSITE_NUM_BULK 1
#define BSP_USB_DESCRIPTOR_DATA_SIZE_BULK COMPOSITE_DBULK_SIZE
#else
#define BSP_USB_COMPOSITE_NUM_BULK 0
#define BSP_USB_DESCRIPTOR_DATA_SIZE_BULK 0
#endif

/* Add up the compiled in sizes */
#define BSP_USB_COMPOSITE_NUM        (BSP_USB_COMPOSITE_NUM_CDC + \
                                      BSP_USB_COMPOSITE_NUM_BULK)

#define BSP_USB_DESCRIPTOR_DATA_SIZE (BSP_USB_DESCRIPTOR_DATA_SIZE_CDC + \
                                      BSP_USB_DESCRIPTOR_DATA_SIZE_BULK)

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_Usb_langDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};


/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_Usb_manufacturerString[] =
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
const uint8_t bsp_Usb_productString[] =
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
const uint8_t bsp_Usb_serialNumberString[] =
{
    2 + (8 * 2),
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};


/*============================================================================*/
/**
 * The data interface description string.
 */
const uint8_t bsp_Usb_dataInterfaceString[] =
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
const uint8_t bsp_Usb_configString[] =
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
const uint8_t* const bsp_Usb_stringDescriptors[] =
{
    bsp_Usb_langDescriptor,
    bsp_Usb_manufacturerString,
    bsp_Usb_productString,
    bsp_Usb_serialNumberString,
    bsp_Usb_dataInterfaceString,
    bsp_Usb_configString
};
#define BSP_USB_STR_DESCRIPTOR_NUM DIM(bsp_Usb_stringDescriptors)

//****************************************************************************
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//****************************************************************************
uint8_t bsp_Usb_deviceDescriptorData[BSP_USB_DESCRIPTOR_DATA_SIZE];

//****************************************************************************
// Allocate the Device Data for the top level composite device class.
//****************************************************************************
tCompositeEntry bsp_Usb_compositeEntries[BSP_USB_COMPOSITE_NUM];
tUSBDCompositeDevice bsp_Usb_deviceInfo =
{
    USB_VID_TI_1CBE,           // TM4C VID.
    USB_PID_COMP_SERIAL,       // TM4C PID for composite serial device.
    250,                       // This is in 2mA increments so 500mA.
    USB_CONF_ATTR_BUS_PWR,     // Bus powered device.
    0,                         // There is no need for a default composite event handler
    bsp_Usb_stringDescriptors, // The string table.
    BSP_USB_STR_DESCRIPTOR_NUM,
    BSP_USB_COMPOSITE_NUM,     // The Composite device array.
    bsp_Usb_compositeEntries
};

#endif

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Usb_init( void )
{
#if defined(BSP_PLATFORM_USB_ENABLED)
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

    /* Initialize composite devices */
    uint8_t devIdx = 0;
#if defined(BSP_PLATFORM_USB_CDC)
    bsp_Usb_deviceInfo.psDevices[devIdx].pvInstance = bsp_UsbCdc_init( &bsp_Usb_compositeEntries[devIdx] );
    devIdx++;
#endif
#if defined(BSP_PLATFORM_USB_BULK)
    bsp_Usb_deviceInfo.psDevices[devIdx].pvInstance = bsp_UsbBulk_init( &bsp_Usb_compositeEntries[devIdx] );
    devIdx++;
#endif

    /* Initialize the device */
    USBDCompositeInit( 0, &(bsp_Usb_deviceInfo),
                       BSP_USB_DESCRIPTOR_DATA_SIZE,
                       bsp_Usb_deviceDescriptorData );
#endif
    return;
}


/*============================================================================*/
void
bsp_Usb_interruptHandler( void )
{
#if defined(BSP_PLATFORM_USB_ENABLED)
    extern void USB0DeviceIntHandler(void);
    BSP_TRACE_USBIO_INT_ENTER();
    USB0DeviceIntHandler();
    BSP_TRACE_USBIO_INT_EXIT();
#endif
    return;
}


