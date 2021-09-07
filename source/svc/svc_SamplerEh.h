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
 * @file svc_PwrMonEh.h
 * @brief Contains the message interface related to the Samplerity service
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "dev_PwrMon.h"
#include "svc_PwrMon_channel.h"
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
#define SVC_PWRMONEH_CH_NAME_LEN 40

#define SCV_PWRMONEH_MSG_ID_BUILD(_name, _type)                              \
    SVC_MSGFWK_MSG_ID_BUILD_##_type( SVC_EHID_PWRMON,                        \
                                     SVC_PWRMONEH_MSG_ID_##_name ##_ ##_type )

/*============================================================================*/
// Event handler message IDs
#define SVC_PWRMONEH_MSG_ID_START_REQ   0
#define SVC_PWRMONEH_MSG_ID_START_CNF   1
#define SVC_PWRMONEH_MSG_ID_STOP_REQ    2
#define SVC_PWRMONEH_MSG_ID_STOP_CNF    3
#define SVC_PWRMONEH_MSG_ID_CONFIG_REQ  4
#define SVC_PWRMONEH_MSG_ID_CONFIG_CNF  5

#define SVC_PWRMONEH_MSG_ID_PKT_READY_IND  6


#define SVC_PWRMONEH_MSG_ID_NAMES_TABLE \
{                                       \
    "StartReq",                         \
    "StartCnf",                         \
    "StopReq",                          \
    "StopCnf",                          \
    "ConfigReq",                        \
    "ConfigCnf",                        \
    "PktReadyInd"                       \
}

#define SVC_PWRMONEH_START_REQ     SCV_PWRMONEH_MSG_ID_BUILD( START, REQ )
#define SVC_PWRMONEH_START_CNF     SCV_PWRMONEH_MSG_ID_BUILD( START, CNF )
#define SVC_PWRMONEH_STOP_REQ      SCV_PWRMONEH_MSG_ID_BUILD( STOP, REQ )
#define SVC_PWRMONEH_STOP_CNF      SCV_PWRMONEH_MSG_ID_BUILD( STOP, CNF )
#define SVC_PWRMONEH_CONFIG_REQ    SCV_PWRMONEH_MSG_ID_BUILD( CONFIG, REQ )
#define SVC_PWRMONEH_CONFIG_CNF    SCV_PWRMONEH_MSG_ID_BUILD( CONFIG, CNF )
#define SVC_PWRMONEH_PKT_READY_IND SCV_PWRMONEH_MSG_ID_BUILD( PKT_READY, IND )

/*==============================================================================
 *                                Types
 *============================================================================*/
#define SVC_PWRMONEH_STATUS_SUCCESS 0
#define SVC_PWRMONEH_STATUS_FAILURE 1
typedef uint8_t svc_PwrMonEh_Status_t;

#define SVC_PWRMONEH_SMPL_FMT_24BIT 0
#define SVC_PWRMONEH_SMPL_FMT_32BIT 1
typedef uint8_t svc_PwrMonEh_SmplFmt_t;

typedef uint8_t svc_PwrMonEh_ChId_t;

/*============================================================================*/
// Event handler message structures
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_StartReq_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StartCnf_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_StopReq_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StopCnf_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChEntry_s
{
    dev_PwrMon_ChannelId_t chId;
    dev_PwrMon_ShuntVal_t  shuntVal;
    char                   chName[SVC_PWRMONEH_CH_NAME_LEN+1];
} svc_PwrMonEh_ChEntry_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ConfigReq_s
{
    svc_MsgFwk_Hdr_t       hdr;
    svc_PwrMonEh_SmplFmt_t smplFmt;
    uint8_t                numCh;
    svc_PwrMonEh_ChEntry_t chTable[];
} svc_PwrMonEh_ConfigReq_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ConfigCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_ConfigCnf_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_PktReadyInd_s
{
    svc_MsgFwk_Hdr_t hdr;
    void*            pktPtr;
    size_t           pktLen;
    void*            callback;
    void*            cbData;
} svc_PwrMonEh_PktReadyInd_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_PwrMonEh_info;
