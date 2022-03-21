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
 * @file dev_PwrMon.h
 * @brief Contains Macros, defines, and prototypes for the Power Monitor ADCs.
 */
#pragma once

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
typedef int32_t dev_PwrMon_Data_t;
typedef uint8_t dev_PwrMon_ChannelId_t;
typedef void (*dev_PwrMon_Callback_t)(void *cbData);

/*==============================================================================
 *                               Prototypes
 *============================================================================*/
/*===========================================================================*/
void dev_PwrMon_init(void);

/*===========================================================================*/
void dev_PwrMon_channelOffsetCal(
    dev_PwrMon_ChannelId_t channelId,
    dev_PwrMon_Data_t *    vBusPtr,
    dev_PwrMon_Data_t *    vShuntPtr);

/*===========================================================================*/
void dev_PwrMon_channelBusVoltageRead(
    dev_PwrMon_ChannelId_t channelId,
    uint8_t *              dataPtr,
    dev_PwrMon_Callback_t  callback,
    void *                 cbData);

/*===========================================================================*/
void dev_PwrMon_channelShuntVoltageRead(
    dev_PwrMon_ChannelId_t channelId,
    uint8_t *              dataPtr,
    dev_PwrMon_Callback_t  callback,
    void *                 cbData);

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vBusConvert(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val);

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vShuntConvert(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val);

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vBusFormat(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val);

/*===========================================================================*/
dev_PwrMon_Data_t dev_PwrMon_vShuntFormat(dev_PwrMon_ChannelId_t channelId, dev_PwrMon_Data_t val);
