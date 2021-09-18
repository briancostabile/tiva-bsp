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
 * @file svc_PwrMon_channel.h
 * @brief Contains API for the Power Monitor channel object
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "dev_PwrMon.h"
#include "svc_PwrMonEh.h"

/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_PwrMon_channelInit( void );

/*============================================================================*/
void
svc_PwrMon_channelConfig( dev_PwrMon_ChannelId_t chId,
                          dev_PwrMon_ShuntVal_t  shuntVal,
                          char*                  name );

/*============================================================================*/
void
svc_PwrMon_channelUpdate( dev_PwrMon_ChannelId_t chId,
                          dev_PwrMon_Data_t      voltage,
                          dev_PwrMon_Data_t      current );

/*============================================================================*/
void
svc_PwrMon_channelAvgReset( dev_PwrMon_ChannelId_t chId );

/*============================================================================*/
void
svc_PwrMon_channelAvgResetAll( void );

/*============================================================================*/
void
svc_PwrMon_channelAvgGet( dev_PwrMon_ChannelId_t chId,
                          dev_PwrMon_Data_t*     vPtr,
                          dev_PwrMon_Data_t*     iPtr );

/*============================================================================*/
void
svc_PwrMon_channelAvgGetAll( svc_PwrMonEh_ChAvgInfo_t* dataBuffer );

/*============================================================================*/
svc_PwrMonEh_ChBitmap_t
svc_PwrMon_channelBitmapGet( void );

/*============================================================================*/
uint8_t
svc_PwrMon_channelCntGet( void );
