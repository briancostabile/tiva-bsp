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
/*===========================================================================*/
/**
 * @file dev_PwrMon.c
 * @brief Contains Driver for the INA228 Power Monitor.
 */
#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Gpio.h"
#include "bsp_Pragma.h"
#include "bsp_I2c.h"
#include "dev_PwrMon.h"
#include "dev_PwrMon_ina.h"
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
#include "dev_PwrMon_ina226.h"
#endif
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)
#include "dev_PwrMon_ina228.h"
#endif
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA239)
#include "dev_PwrMon_ina239.h"
#endif

#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226) || \
    defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228) || \
    defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA239)

#define DEV_PWRMON_DEVICE_BUS_IS_SSI(_devInfoPtr) \
    ((_devInfoPtr)->devType == DEV_PWR_MON_DEVICE_TYPE_INA239)
#define DEV_PWRMON_DEVICE_BUS_IS_I2C(_devInfoPtr) !(DEV_PWRMON_DEVICE_BUS_IS_SSI(_devInfoPtr))

// Macros to build header for SPI based INA devices
#define DEV_PWRMON_INA239_CMD_WRITE 0x00
#define DEV_PWRMON_INA239_CMD_READ  0x01

#define DEV_PWRMON_INA239_CMD_HDR(addr, cmd) \
    ((((addr)&0x3F) << 2) | ((DEV_PWRMON_INA239_CMD_##cmd) & 0x03))

// How long after activating the calibration short before taking a measurement
#define DEV_PWRMON_CAL_SHORT_DELAY_MS 50

/*=============================================================================
 *                                   Types
 *===========================================================================*/

typedef struct dev_PwrMon_ChannelInfo_s {
    dev_PwrMon_DevId_t vShunt;
    dev_PwrMon_DevId_t vBus;
    bsp_Gpio_PortId_t  calPort;
    bsp_Gpio_BitMask_t calMask;
} dev_PwrMon_ChannelInfo_t;

/*=============================================================================
 *                                   Globals
 *===========================================================================*/
dev_PwrMon_DeviceCtx_t        dev_PwrMon_deviceCtx[BSP_PLATFORM_PWRMON_NUM_DEVICES];
const dev_PwrMon_DeviceInfo_t dev_PwrMon_deviceInfo[] =
    BSP_PLATFORM_PWRMON_DEVICE_TABLE(dev_PwrMon_deviceCtx);
const dev_PwrMon_ChannelInfo_t dev_PwrMon_channelInfo[] = BSP_PLATFORM_PWRMON_CHANNEL_MAP_TABLE;

/*===========================================================================*/
// Order must match the enumeration for dev_PwrMon_DeviceType_t
dev_PwrMon_DeviceApi_t *dev_PwrMon_deviceApiTable[] = {
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
    &dev_PwrMon_ina226_deviceApi,
#else
    NULL,
#endif
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA228)
    &dev_PwrMon_ina228_deviceApi,
#else
    NULL,
#endif
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA239)
    &dev_PwrMon_ina239_deviceApi
#else
    NULL
#endif
};

/*=============================================================================
 *                              Local Functions
 *===========================================================================*/
/*===========================================================================*/
// Wrapper callback for all I2C transactions
static void dev_PwrMon_i2cTransCallback(bsp_I2c_Status_t status, void *usrData)
{
    dev_PwrMon_DeviceCtx_t *ctx = (dev_PwrMon_DeviceCtx_t *)usrData;
    ctx->active                 = false;
    if (ctx->callback != NULL) {
        ctx->callback(ctx->cbData);
    }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the I2C transaction structure and queue it
static void dev_PwrMon_i2cTransQueue(const dev_PwrMon_DeviceInfo_t *devPtr)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;
    ctx->trans.i2c.speed        = devPtr->bus.i2c.speed;
    ctx->trans.i2c.addr         = devPtr->bus.i2c.addr;
    ctx->trans.i2c.callback     = dev_PwrMon_i2cTransCallback;
    ctx->trans.i2c.usrData      = devPtr->ctx;
    ctx->active                 = true;
    bsp_I2c_masterTransQueue(devPtr->bus.i2c.id, &ctx->trans.i2c);
    return;
}

/*===========================================================================*/
// Wrapper to write to the config register on the INA226/228.
static void dev_PwrMon_i2cRegWrite(
    const dev_PwrMon_DeviceInfo_t *devPtr,
    uint8_t                        regId,
    dev_PwrMon_WriteData_t         regValue)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;

    ctx->wBuffer[0] = regId;
    ctx->wBuffer[1] = ((regValue >> 8) & 0xFF);
    ctx->wBuffer[2] = ((regValue >> 0) & 0xFF);

    ctx->prevRegId = regId;

    ctx->trans.i2c.type     = BSP_I2C_TRANS_TYPE_WRITE;
    ctx->trans.i2c.wLen     = (2 + 1);    // All INA writable regs are 2 bytes, plus 1 byte address
    ctx->trans.i2c.wBuffer  = ctx->wBuffer;
    ctx->trans.i2c.rLen     = 0;
    ctx->trans.i2c.rBuffer  = NULL;
    ctx->trans.i2c.rReverse = false;
    dev_PwrMon_i2cTransQueue(devPtr);

    // Spin for completion if no callback
    int32_t timeout = 10000;
    while ((ctx->callback == NULL) && (ctx->active == true) && (--timeout > 0))
        ;
}

/*===========================================================================*/
// Wrapper to read data from the INA226/228.
static void
dev_PwrMon_i2cRegRead(const dev_PwrMon_DeviceInfo_t *devPtr, uint8_t regId, uint8_t regLen)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;

    ctx->trans.i2c.rLen     = regLen;
    ctx->trans.i2c.rBuffer  = ctx->rPtr;
    ctx->trans.i2c.rReverse = true;
    if (regId != ctx->prevRegId) {
        ctx->wBuffer[0]        = regId;
        ctx->trans.i2c.type    = BSP_I2C_TRANS_TYPE_WRITE_READ;
        ctx->trans.i2c.wLen    = 1;
        ctx->trans.i2c.wBuffer = ctx->wBuffer;
    }
    else {
        ctx->trans.i2c.type    = BSP_I2C_TRANS_TYPE_READ;
        ctx->trans.i2c.wLen    = 0;
        ctx->trans.i2c.wBuffer = NULL;
    }
    ctx->prevRegId = regId;
    dev_PwrMon_i2cTransQueue(devPtr);

    // Spin for completion if no callback
    int32_t timeout = 10000;
    while ((ctx->callback == NULL) && (ctx->active == true) && (--timeout > 0))
        ;
}

/*===========================================================================*/
// Wrapper callback for all SSI transactions
static void dev_PwrMon_SsiTransCallback(void *usrData)
{
    dev_PwrMon_DeviceCtx_t *ctx = (dev_PwrMon_DeviceCtx_t *)usrData;
    ctx->active                 = false;
    if (ctx->callback != NULL) {
        ctx->callback(ctx->cbData);
    }
    return;
}

/*===========================================================================*/
// Wrapper function to setup the SSI transaction structure and queue it
static void dev_PwrMon_SsiTransQueue(const dev_PwrMon_DeviceInfo_t *devPtr)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;
    ctx->trans.ssi.callback     = dev_PwrMon_SsiTransCallback;
    ctx->trans.ssi.usrData      = devPtr->ctx;
    ctx->active                 = true;
    bsp_Ssi_masterTransQueue(devPtr->bus.ssi.id, &ctx->trans.ssi);
    return;
}

/*===========================================================================*/
// Wrapper to write to the config register on the Ina239 ADC.
static void dev_PwrMon_ssiRegWrite(
    const dev_PwrMon_DeviceInfo_t *devPtr,
    uint8_t                        regId,
    dev_PwrMon_WriteData_t         regValue)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;

    ctx->wBuffer[0] = DEV_PWRMON_INA239_CMD_HDR(regId, WRITE);
    ctx->wBuffer[1] = ((regValue >> 8) & 0xFF);
    ctx->wBuffer[2] = (regValue & 0xFF);

    ctx->trans.ssi.speed    = devPtr->bus.ssi.speedWrite;
    ctx->trans.ssi.mode     = BSP_SSI_TRANS_MODE_3;
    ctx->trans.ssi.type     = BSP_SSI_TRANS_TYPE_WRITE;
    ctx->trans.ssi.wLen     = (2 + 1);    // All INA writable regs are 2 bytes, plus 1 byte hdr;
    ctx->trans.ssi.csPort   = devPtr->bus.ssi.csPort;
    ctx->trans.ssi.csMask   = devPtr->bus.ssi.csMask;
    ctx->trans.ssi.csMaskEn = 0;
    ctx->trans.ssi.wBuffer  = ctx->wBuffer;
    ctx->trans.ssi.rLen     = 0;
    ctx->trans.ssi.rBuffer  = NULL;
    ctx->trans.ssi.rReverse = false;
    dev_PwrMon_SsiTransQueue(devPtr);

    // Spin for completion if no callback
    while ((ctx->callback == NULL) && (ctx->active == true))
        ;
    return;
}

/*===========================================================================*/
// Wrapper to read data from the Ina239 DAC.
static void
dev_PwrMon_ssiRegRead(const dev_PwrMon_DeviceInfo_t *devPtr, uint8_t regId, uint8_t regLen)
{
    dev_PwrMon_DeviceCtx_t *ctx = devPtr->ctx;

    ctx->wBuffer[0] = DEV_PWRMON_INA239_CMD_HDR(regId, READ);
    ctx->wBuffer[1] = 0;
    ctx->wBuffer[2] = 0;
    ctx->wBuffer[3] = 0;

    ctx->trans.ssi.speed    = devPtr->bus.ssi.speedRead;
    ctx->trans.ssi.mode     = BSP_SSI_TRANS_MODE_3;
    ctx->trans.ssi.type     = BSP_SSI_TRANS_TYPE_WRITE_READ;
    ctx->trans.ssi.wLen     = regLen + 1;
    ctx->trans.ssi.csPort   = devPtr->bus.ssi.csPort;
    ctx->trans.ssi.csMask   = devPtr->bus.ssi.csMask;
    ctx->trans.ssi.csMaskEn = 0;
    ctx->trans.ssi.wBuffer  = ctx->wBuffer;
    ctx->trans.ssi.rLen     = regLen;
    ctx->trans.ssi.rBuffer  = ctx->rPtr;
    ctx->trans.ssi.rReverse = true;
    dev_PwrMon_SsiTransQueue(devPtr);

    // Spin for completion if no callback
    while ((ctx->callback == NULL) && (ctx->active == true))
        ;
    return;
}

/*===========================================================================*/
static void dev_PwrMon_calInit(dev_PwrMon_ChannelId_t channelId)
{
    const dev_PwrMon_ChannelInfo_t *channelPtr = &dev_PwrMon_channelInfo[channelId];
    if (channelPtr->calMask != 0) {
        bsp_Gpio_configOutput(channelPtr->calPort, channelPtr->calMask, false, BSP_GPIO_DRIVE_2MA);
        bsp_Gpio_write(channelPtr->calPort, channelPtr->calMask, channelPtr->calMask);
    }
    return;
}

/*===========================================================================*/
void dev_PwrMon_calEnable(dev_PwrMon_ChannelId_t channelId)
{
    const dev_PwrMon_ChannelInfo_t *channelPtr = &dev_PwrMon_channelInfo[channelId];
    if (channelPtr->calMask != 0) {
        bsp_Gpio_write(channelPtr->calPort, channelPtr->calMask, 0);
    }
    return;
}

/*===========================================================================*/
void dev_PwrMon_calDisable(dev_PwrMon_ChannelId_t channelId)
{
    const dev_PwrMon_ChannelInfo_t *channelPtr = &dev_PwrMon_channelInfo[channelId];
    if (channelPtr->calMask != 0) {
        bsp_Gpio_write(channelPtr->calPort, channelPtr->calMask, channelPtr->calMask);
    }
    return;
}

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void dev_PwrMon_commonRead(
    const dev_PwrMon_DeviceInfo_t *devPtr,
    uint8_t                        regId,
    uint8_t                        regLen,
    void *                         dataPtr,
    dev_PwrMon_Callback_t          callback,
    void *                         cbData)
{
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    devPtr->ctx->rLen     = regLen;
    devPtr->ctx->rPtr     = dataPtr;
    if (DEV_PWRMON_DEVICE_BUS_IS_I2C(devPtr)) {
        dev_PwrMon_i2cRegRead(devPtr, regId, regLen);
    }
    else {
        dev_PwrMon_ssiRegRead(devPtr, regId, regLen);
    }
    return;
}

/*===========================================================================*/
void dev_PwrMon_commonWrite(
    const dev_PwrMon_DeviceInfo_t *devPtr,
    uint8_t                        regId,
    dev_PwrMon_WriteData_t         data,
    dev_PwrMon_Callback_t          callback,
    void *                         cbData)
{
    devPtr->ctx->callback = callback;
    devPtr->ctx->cbData   = cbData;
    if (DEV_PWRMON_DEVICE_BUS_IS_I2C(devPtr)) {
        dev_PwrMon_i2cRegWrite(devPtr, regId, data);
    }
    else {
        dev_PwrMon_ssiRegWrite(devPtr, regId, data);
    }
    return;
}

/*===========================================================================*/
// Go device by device initializing the contexts and devices. Then initiailze
// the calibration and configure devices to be off
void dev_PwrMon_init(void)
{
    const dev_PwrMon_ChannelInfo_t *chInfoPtr;
    const dev_PwrMon_DeviceInfo_t * devInfoPtr;
    const dev_PwrMon_DeviceApi_t *  devApiPtr;

    // Initialize each i2c bus and each device on the bus
    uint16_t i2cBusInitMask = 0x0000;
    uint16_t ssiBusInitMask = 0x0000;
    for (uint8_t i = 0; i < BSP_PLATFORM_PWRMON_NUM_DEVICES; i++) {
        dev_PwrMon_deviceCtx[i].prevRegId = DEV_PWRMON_REG_INVALID;
        dev_PwrMon_deviceCtx[i].callback  = NULL;

        devInfoPtr = &dev_PwrMon_deviceInfo[i];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
        if (DEV_PWRMON_DEVICE_BUS_IS_I2C(devInfoPtr)) {
            // Only init busses that haven't already been initialized
            if ((i2cBusInitMask & (1 << devInfoPtr->bus.i2c.id)) == 0) {
                i2cBusInitMask |= (1 << devInfoPtr->bus.i2c.id);
                bsp_I2c_masterControl(devInfoPtr->bus.i2c.id, BSP_I2C_CONTROL_ENABLE_HS);
            }
        }
        else {
            // Only init busses that haven't already been initialized
            if ((ssiBusInitMask & (1 << devInfoPtr->bus.ssi.id)) == 0) {
                ssiBusInitMask |= (1 << devInfoPtr->bus.ssi.id);
                bsp_Ssi_masterControl(devInfoPtr->bus.ssi.id, BSP_SSI_CONTROL_ENABLE);
            }

            // Configure the Chip Select
            bsp_Gpio_configOutput(
                devInfoPtr->bus.ssi.csPort, devInfoPtr->bus.ssi.csMask, FALSE, BSP_GPIO_DRIVE_2MA);

            // Disable selection
            bsp_Gpio_write(
                devInfoPtr->bus.ssi.csPort, devInfoPtr->bus.ssi.csMask, devInfoPtr->bus.ssi.csMask);
        }
        devApiPtr->init(&dev_PwrMon_deviceInfo[i]);
    }

    // For each channel, configure the default settings
    for (uint8_t i = 0; i < DIM(dev_PwrMon_channelInfo); i++) {
        chInfoPtr  = &dev_PwrMon_channelInfo[i];
        devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];

        if (devInfoPtr->mode == DEV_PWRMON_DEVICE_MODE_MASK_BOTH) {
            devApiPtr->config(
                &dev_PwrMon_deviceInfo[chInfoPtr->vShunt],
                DEV_PWRMON_DEVICE_MODE_MASK_BOTH,
                NULL,
                NULL);
        }
        else {
            devApiPtr->config(
                &dev_PwrMon_deviceInfo[chInfoPtr->vShunt],
                DEV_PWRMON_DEVICE_MODE_MASK_SHUNT,
                NULL,
                NULL);

            devApiPtr->config(
                &dev_PwrMon_deviceInfo[chInfoPtr->vBus],
                DEV_PWRMON_DEVICE_MODE_MASK_BUS,
                NULL,
                NULL);
        }
        dev_PwrMon_calInit(i);
    }

    return;
}

/*===========================================================================*/
void dev_PwrMon_channelOffsetCal(
    dev_PwrMon_ChannelId_t channelId,
    dev_PwrMon_Data_t *    vBusPtr,
    dev_PwrMon_Data_t *    vShuntPtr)
{
    const dev_PwrMon_ChannelInfo_t *chInfoPtr = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t * devInfoPtr;
    const dev_PwrMon_DeviceApi_t *  devApiPtr;

    if (chInfoPtr->calMask != 0) {
        dev_PwrMon_calEnable(channelId);
        bsp_Clk_delayMs(DEV_PWRMON_CAL_SHORT_DELAY_MS * 2);

        devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
        *vShuntPtr = devApiPtr->vShunt.cal(&dev_PwrMon_deviceInfo[chInfoPtr->vShunt]);

        devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vBus];
        devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
        *vBusPtr   = devApiPtr->vBus.cal(&dev_PwrMon_deviceInfo[chInfoPtr->vBus]);

        dev_PwrMon_calDisable(channelId);
    }
    return;
}

#define DEV_PWR_MON_DEVICE_API_FROM_CH(_ch, _vId) \
    dev_PwrMon_deviceApiTable[dev_PwrMon_deviceInfo[dev_PwrMon_channelInfo[_ch]._vId].devType]

/*===========================================================================*/
void dev_PwrMon_channelBusVoltageRead(
    dev_PwrMon_ChannelId_t channelId,
    uint8_t *              dataPtr,
    dev_PwrMon_Callback_t  callback,
    void *                 cbData)
{
    const dev_PwrMon_ChannelInfo_t *chInfoPtr  = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t * devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vBus];
    const dev_PwrMon_DeviceApi_t *  devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    devApiPtr->vBus.read(&dev_PwrMon_deviceInfo[chInfoPtr->vBus], dataPtr, callback, cbData);
    return;
}

/*===========================================================================*/
void dev_PwrMon_channelShuntVoltageRead(
    dev_PwrMon_ChannelId_t channelId,
    uint8_t *              dataPtr,
    dev_PwrMon_Callback_t  callback,
    void *                 cbData)
{
    const dev_PwrMon_ChannelInfo_t *chInfoPtr  = &dev_PwrMon_channelInfo[channelId];
    const dev_PwrMon_DeviceInfo_t * devInfoPtr = &dev_PwrMon_deviceInfo[chInfoPtr->vShunt];
    const dev_PwrMon_DeviceApi_t *  devApiPtr  = dev_PwrMon_deviceApiTable[devInfoPtr->devType];
    devApiPtr->vShunt.read(&dev_PwrMon_deviceInfo[chInfoPtr->vShunt], dataPtr, callback, cbData);
    return;
}

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vBusConvert(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val)
{
    const dev_PwrMon_DeviceApi_t *devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vBus);
    return (devApiPtr->vBus.convert(val));
}

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vShuntConvert(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val)
{
    const dev_PwrMon_DeviceApi_t *devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vShunt);
    return (devApiPtr->vShunt.convert(val));
}

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vBusFormat(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val)
{
    const dev_PwrMon_DeviceApi_t *devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vBus);
    return (devApiPtr->vBus.format(val));
}

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vShuntFormat(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val)
{
    const dev_PwrMon_DeviceApi_t *devApiPtr = DEV_PWR_MON_DEVICE_API_FROM_CH(channelId, vShunt);
    return (devApiPtr->vShunt.format(val));
}

#endif
