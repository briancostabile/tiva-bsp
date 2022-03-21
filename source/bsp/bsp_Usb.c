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
 * @file bsp_Usb.c
 * @brief Contains Functions for setting up the USB front end
 */

#include "bsp_Platform.h"
#include "bsp_Types.h"
#include "bsp_Usb.h"
#include "bsp_UsbBulk.h"
#include "bsp_UsbCdc.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Assert.h"
#include "bsp_Interrupt.h"
#include "bsp_Io.h"
#include "bsp_Mfg.h"

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
#if defined(BSP_PLATFORM_USB_ENABLED) && \
    (defined(BSP_PLATFORM_USB_CDC) || defined(BSP_PLATFORM_USB_BULK))

/* Dynamically (at compile-time) compute the number of devices and the size of the scratch memory */
#if defined(BSP_PLATFORM_USB_CDC)
#define BSP_USB_COMPOSITE_NUM_CDC        1
#define BSP_USB_DESCRIPTOR_DATA_SIZE_CDC COMPOSITE_DCDC_SIZE
#else
#define BSP_USB_COMPOSITE_NUM_CDC        0
#define BSP_USB_DESCRIPTOR_DATA_SIZE_CDC 0
#endif

#if defined(BSP_PLATFORM_USB_BULK)
#define BSP_USB_COMPOSITE_NUM_BULK        1
#define BSP_USB_DESCRIPTOR_DATA_SIZE_BULK COMPOSITE_DBULK_SIZE
#else
#define BSP_USB_COMPOSITE_NUM_BULK        0
#define BSP_USB_DESCRIPTOR_DATA_SIZE_BULK 0
#endif

/* Add up the compiled in sizes */
#define BSP_USB_COMPOSITE_NUM (BSP_USB_COMPOSITE_NUM_CDC + BSP_USB_COMPOSITE_NUM_BULK)

#define BSP_USB_DESCRIPTOR_DATA_SIZE \
    (BSP_USB_DESCRIPTOR_DATA_SIZE_CDC + BSP_USB_DESCRIPTOR_DATA_SIZE_BULK)

/*==============================================================================
 *                                Globals
 *============================================================================*/
/* clang-format off */
// Descriptor overlay for DFU
uint8_t bsp_Usb_dfuDescriptorOverlay[] =
{
    0xbe, 0x1c,    // TI VID
    0xff, 0x00,    // Tiva DFU PID
    0x20, 0x01,    // USB version 1.2
    USB_CONF_ATTR_BUS_PWR,  // Self powered using bus power
    (250 / 2),              // Max power in increments of 2mA

    // Start of string descriptor.
    2 + (1 * 2),
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US),

    // Texas Instruments Incorporated
    2 + (30 * 2),
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
    ' ', 0,
    'I', 0,
    'n', 0,
    'c', 0,
    'o', 0,
    'r', 0,
    'p', 0,
    'o', 0,
    'r', 0,
    'a', 0,
    't', 0,
    'e', 0,
    'd', 0,

    // Tiva Device Firmware Update
    2 + (27 * 2),
    USB_DTYPE_STRING,
    'T', 0,
    'i', 0,
    'v', 0,
    'a', 0,
    ' ', 0,
    'D', 0,
    'e', 0,
    'v', 0,
    'i', 0,
    'c', 0,
    'e', 0,
    ' ', 0,
    'F', 0,
    'i', 0,
    'r', 0,
    'm', 0,
    'w', 0,
    'a', 0,
    'r', 0,
    'e', 0,
    ' ', 0,
    'U', 0,
    'p', 0,
    'd', 0,
    'a', 0,
    't', 0,
    'e', 0,

    // 000000000000
    2 + (12 * 2),
    USB_DTYPE_STRING,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0
};

/*============================================================================*/
/**
 * The languages supported by this device.
 */
const uint8_t bsp_Usb_langDescriptor[] = {4, USB_DTYPE_STRING, USBShort(USB_LANG_EN_US)};

/*============================================================================*/
/**
 * The manufacturer string.
 */
const uint8_t bsp_Usb_manufacturerString[] =
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
const uint8_t bsp_Usb_productString[] =
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
uint8_t bsp_Usb_serialNumberString[] =
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
const uint8_t bsp_Usb_dataInterfaceString[] =
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
const uint8_t bsp_Usb_configString[] =
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
const uint8_t *const bsp_Usb_stringDescriptors[] = {
    bsp_Usb_langDescriptor,
    bsp_Usb_manufacturerString,
    bsp_Usb_productString,
    bsp_Usb_serialNumberString,
    bsp_Usb_dataInterfaceString,
    bsp_Usb_configString};
#define BSP_USB_STR_DESCRIPTOR_NUM DIM(bsp_Usb_stringDescriptors)

//****************************************************************************
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//****************************************************************************
uint8_t bsp_Usb_deviceDescriptorData[BSP_USB_DESCRIPTOR_DATA_SIZE];

//****************************************************************************
// Allocate the Device Data for the top level composite device class.
//****************************************************************************
tCompositeEntry      bsp_Usb_compositeEntries[BSP_USB_COMPOSITE_NUM];
tUSBDCompositeDevice bsp_Usb_deviceInfo = {
#if defined(BSP_PLATOFRM_USB_DEVICE_VID)
    BSP_PLATOFRM_USB_DEVICE_VID,
#else
    USB_VID_TI_1CBE,           // TM4C VID.
#endif
#if defined(BSP_PLATOFRM_USB_DEVICE_PID)
    BSP_PLATOFRM_USB_DEVICE_PID,
#else
    USB_PID_COMP_SERIAL,       // TM4C PID for composite serial device.
#endif
#if defined(BSP_PLATOFRM_USB_MAX_POWER)
    BSP_PLATOFRM_USB_MAX_POWER,
#else
    250,                       // This is in 2mA increments so 500mA.
#endif
#if defined(BSP_PLATOFRM_USB_CONF_ATTR)
    BSP_PLATOFRM_USB_CONF_ATTR,
#else
    USB_CONF_ATTR_SELF_PWR,    // Bus powered device.
#endif
    0,                            // There is no need for a default composite event handler
    bsp_Usb_stringDescriptors,    // The string table.
    BSP_USB_STR_DESCRIPTOR_NUM,
    BSP_USB_COMPOSITE_NUM,    // The Composite device array.
    bsp_Usb_compositeEntries};

#endif

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Usb_init(void)
{
#if defined(BSP_PLATFORM_USB_ENABLED)
    /* Configure USB pins as input no pull analog */
    bsp_Gpio_configInput(
        BSP_GPIO_PORT_ID_USB_DP,
        BSP_GPIO_BIT_MASK_USB_DP,
        false,    // openDrain
        BSP_GPIO_PULL_NONE);
    bsp_Gpio_configAnalog(BSP_GPIO_PORT_ID_USB_DP, BSP_GPIO_BIT_MASK_USB_DP);

    bsp_Gpio_configInput(
        BSP_GPIO_PORT_ID_USB_DM,
        BSP_GPIO_BIT_MASK_USB_DM,
        false,    // openDrain
        BSP_GPIO_PULL_NONE);
    bsp_Gpio_configAnalog(BSP_GPIO_PORT_ID_USB_DM, BSP_GPIO_BIT_MASK_USB_DM);

#if defined(BSP_GPIO_PORT_ID_USB_VB)
    bsp_Gpio_configInput(
        BSP_GPIO_PORT_ID_USB_VB,
        BSP_GPIO_BIT_MASK_USB_VB,
        false,    // openDrain
        BSP_GPIO_PULL_NONE);
    bsp_Gpio_configAnalog(BSP_GPIO_PORT_ID_USB_VB, BSP_GPIO_BIT_MASK_USB_VB);
#endif

    /* Set the USB stack mode to Device mode with or without VBUS monitoring. */
    USBStackModeSet(0, eUSBModeForceDevice, NULL);

    /* Tell the USB library the CPU clock and the PLL frequency. */
#if defined(BSP_PLATFORM_PROCESSOR_TM4C129)
    uint32_t feature = bsp_Clk_sysClkGet();
    USBDCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &feature);
    feature = bsp_Clk_vcoFreqGet();
    USBDCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &feature);
#endif

    /* Initialize composite devices */
    uint8_t devIdx = 0;
#if defined(BSP_PLATFORM_USB_CDC)
    bsp_Usb_deviceInfo.psDevices[devIdx].pvInstance =
        bsp_UsbCdc_init(&bsp_Usb_compositeEntries[devIdx]);
    devIdx++;
#endif
#if defined(BSP_PLATFORM_USB_BULK)
    bsp_Usb_deviceInfo.psDevices[devIdx].pvInstance =
        bsp_UsbBulk_init(&bsp_Usb_compositeEntries[devIdx]);
    devIdx++;
#endif

    /* Copy in productId and serialNumber from Mfg portion of Flash
     * The product Id is the first 8 digits of the USB Serial Number
     * The bottom 4 digits from the Mfg Serial number make up the
     * remaining part of the USB serial number
     */
    uint32_t productId = bsp_Mfg_getProductId();
    uint32_t serialNum = bsp_Mfg_getSerialNumber();

    // Simple unrolled loop
    const char lookup[]            = "0123456789ABCDEF";
    bsp_Usb_serialNumberString[2]  = lookup[((productId >> 28) & 0x0F)];
    bsp_Usb_serialNumberString[4]  = lookup[((productId >> 24) & 0x0F)];
    bsp_Usb_serialNumberString[6]  = lookup[((productId >> 20) & 0x0F)];
    bsp_Usb_serialNumberString[8]  = lookup[((productId >> 16) & 0x0F)];
    bsp_Usb_serialNumberString[10] = lookup[((productId >> 12) & 0x0F)];
    bsp_Usb_serialNumberString[12] = lookup[((productId >> 8) & 0x0F)];
    bsp_Usb_serialNumberString[14] = lookup[((productId >> 4) & 0x0F)];
    bsp_Usb_serialNumberString[16] = lookup[((productId >> 0) & 0x0F)];
    bsp_Usb_serialNumberString[18] = lookup[((serialNum >> 12) & 0x0F)];
    bsp_Usb_serialNumberString[20] = lookup[((serialNum >> 8) & 0x0F)];
    bsp_Usb_serialNumberString[22] = lookup[((serialNum >> 4) & 0x0F)];
    bsp_Usb_serialNumberString[24] = lookup[((serialNum >> 0) & 0x0F)];

    /* Initialize the device */
    USBDCompositeInit(
        0, &(bsp_Usb_deviceInfo), BSP_USB_DESCRIPTOR_DATA_SIZE, bsp_Usb_deviceDescriptorData);
#endif
    return;
}

/*============================================================================*/
void bsp_Usb_bootloader(void)
{
    MAP_USBDevDisconnect(USB0_BASE);
    ROM_UpdateUSB((uint8_t *)&bsp_Usb_dfuDescriptorOverlay);
}

/*============================================================================*/
void bsp_Usb_interruptHandler(void)
{
#if defined(BSP_PLATFORM_USB_ENABLED)
    extern void USB0DeviceIntHandler(void);
    BSP_TRACE_INT_ENTER();
    BSP_TRACE_USBIO_INT_ENTER();
    USB0DeviceIntHandler();
    BSP_TRACE_USBIO_INT_EXIT();
    BSP_TRACE_INT_EXIT();
#endif
    return;
}
