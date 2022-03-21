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
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
#define SVC_PWRMONEH_CH_NAME_LEN 40

#define SCV_PWRMONEH_MSG_ID_BUILD(_name, _type) \
    SVC_MSGFWK_MSG_ID_BUILD_##_type(SVC_EHID_PWRMON, SVC_PWRMONEH_MSG_ID_##_name##_##_type)

/*============================================================================*/
// Event handler message IDs
#define SVC_PWRMONEH_MSG_ID_START_REQ  0
#define SVC_PWRMONEH_MSG_ID_START_CNF  1
#define SVC_PWRMONEH_MSG_ID_STOP_REQ   2
#define SVC_PWRMONEH_MSG_ID_STOP_CNF   3
#define SVC_PWRMONEH_MSG_ID_STOP_IND   4
#define SVC_PWRMONEH_MSG_ID_CONFIG_REQ 5
#define SVC_PWRMONEH_MSG_ID_CONFIG_CNF 6
#define SVC_PWRMONEH_MSG_ID_DATA_IND   7
#define SVC_PWRMONEH_MSG_ID_STATS_REQ  8
#define SVC_PWRMONEH_MSG_ID_STATS_CNF  9
#define SVC_PWRMONEH_MSG_ID_CH_AVG_REQ 10
#define SVC_PWRMONEH_MSG_ID_CH_AVG_CNF 11
#define SVC_PWRMONEH_MSG_ID_SAMPLE_IND 12
#define SVC_PWRMONEH_MSG_ID_CAL_REQ    13
#define SVC_PWRMONEH_MSG_ID_CAL_CNF    14

/* clang-format off */ 
#define SVC_PWRMONEH_MSG_ID_NAMES_TABLE \
{                                       \
    "StartReq",                         \
    "StartCnf",                         \
    "StopReq",                          \
    "StopCnf",                          \
    "StopInd",                          \
    "ConfigReq",                        \
    "ConfigCnf",                        \
    "DataInd",                          \
    "StatsReq",                         \
    "StatsCnf",                         \
    "ChAvgReq",                         \
    "ChAvgCnf",                         \
    "SampleInd",                        \
    "CalReq",                           \
    "CalCnf",                           \
}
/* clang-format on */

#define SVC_PWRMONEH_START_REQ  SCV_PWRMONEH_MSG_ID_BUILD(START, REQ)
#define SVC_PWRMONEH_START_CNF  SCV_PWRMONEH_MSG_ID_BUILD(START, CNF)
#define SVC_PWRMONEH_STOP_REQ   SCV_PWRMONEH_MSG_ID_BUILD(STOP, REQ)
#define SVC_PWRMONEH_STOP_CNF   SCV_PWRMONEH_MSG_ID_BUILD(STOP, CNF)
#define SVC_PWRMONEH_STOP_IND   SCV_PWRMONEH_MSG_ID_BUILD(STOP, IND)
#define SVC_PWRMONEH_CONFIG_REQ SCV_PWRMONEH_MSG_ID_BUILD(CONFIG, REQ)
#define SVC_PWRMONEH_CONFIG_CNF SCV_PWRMONEH_MSG_ID_BUILD(CONFIG, CNF)
#define SVC_PWRMONEH_DATA_IND   SCV_PWRMONEH_MSG_ID_BUILD(DATA, IND)
#define SVC_PWRMONEH_STATS_REQ  SCV_PWRMONEH_MSG_ID_BUILD(STATS, REQ)
#define SVC_PWRMONEH_STATS_CNF  SCV_PWRMONEH_MSG_ID_BUILD(STATS, CNF)
#define SVC_PWRMONEH_CH_AVG_REQ SCV_PWRMONEH_MSG_ID_BUILD(CH_AVG, REQ)
#define SVC_PWRMONEH_CH_AVG_CNF SCV_PWRMONEH_MSG_ID_BUILD(CH_AVG, CNF)
#define SVC_PWRMONEH_SAMPLE_IND SCV_PWRMONEH_MSG_ID_BUILD(SAMPLE, IND)
#define SVC_PWRMONEH_CAL_REQ    SCV_PWRMONEH_MSG_ID_BUILD(CAL, REQ)
#define SVC_PWRMONEH_CAL_CNF    SCV_PWRMONEH_MSG_ID_BUILD(CAL, CNF)

/*==============================================================================
 *                                Types
 *============================================================================*/
#define SVC_PWRMONEH_STATUS_SUCCESS 0
#define SVC_PWRMONEH_STATUS_FAILURE 1
typedef uint8_t svc_PwrMonEh_Status_t;

#define SVC_PWRMONEH_SMPL_NO_STREAM 0
#define SVC_PWRMONEH_SMPL_FMT1_VV   1
#define SVC_PWRMONEH_SMPL_FMT2_P    2
typedef uint8_t svc_PwrMonEh_SmplFmt_t;

typedef uint8_t svc_PwrMonEh_ChId_t;

/*============================================================================*/
typedef uint16_t svc_PwrMonEh_ChBitmap_t;

/*============================================================================*/
typedef uint16_t svc_PwrMonEh_IoBitmap_t;

/*============================================================================*/
typedef uint32_t svc_PwrMonEh_ShuntVal_t;

/*============================================================================*/
// Event handler message structures
//----------------------
// StartReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartReq_s {
    svc_MsgFwk_Hdr_t       hdr;
    uint32_t               smplCnt;
    svc_PwrMonEh_SmplFmt_t smplFmt;
} svc_PwrMonEh_StartReq_t;

//----------------------
// StartCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartCnf_s {
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StartCnf_t;

//----------------------
// StopReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopReq_s {
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_StopReq_t;

//----------------------
// StopCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopCnf_s {
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StopCnf_t;

//----------------------
// StopInd
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopInd_s {
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StopInd_t;

//----------------------
// CalReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_CalReq_s {
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_CalReq_t;

//----------------------
// CalCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_CalCnf_s {
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_CalCnf_t;

//----------------------
// ConfigReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChEntry_s {
    dev_PwrMon_ChannelId_t  chId;
    svc_PwrMonEh_ShuntVal_t shuntVal;
    char                    chName[SVC_PWRMONEH_CH_NAME_LEN + 1];
} svc_PwrMonEh_ChEntry_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ConfigReq_s {
    svc_MsgFwk_Hdr_t       hdr;
    uint8_t                numCh;
    svc_PwrMonEh_ChEntry_t chTable[];
} svc_PwrMonEh_ConfigReq_t;

//----------------------
// ConfigCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ConfigCnf_s {
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_ConfigCnf_t;

//----------------------
// Data Indication
// Max channels per sample-time
#define SVC_PWRMONEH_DATA_IND_CHANNELS_MAX 11

// Samples Per Packet: 50 sample sets at 2500Hz = 20ms worth of samples
#define SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX 50

// Fmt0 is 16bit signed bus and shunt voltage
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SmplDataFmt0_s {
    int16_t vBus;
    int16_t vShunt;
} svc_PwrMonEh_SmplDataFmt0_t;

// Fmt1 is 32bit signed power in uW
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SmplDataFmt1_s {
    int32_t power;
} svc_PwrMonEh_SmplDataFmt1_t;

// Hdr sample is the IO reading bitmap and valid-channel bitmap
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SmplDataHdr_s {
    svc_PwrMonEh_IoBitmap_t ioBitmap;
    svc_PwrMonEh_ChBitmap_t chBitmap;
} svc_PwrMonEh_SmplDataHdr_t;

typedef union {
    svc_PwrMonEh_SmplDataHdr_t  hdr;
    svc_PwrMonEh_SmplDataFmt0_t fmt0;
    svc_PwrMonEh_SmplDataFmt1_t fmt1;
} svc_PwrMonEh_SmplData_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_DataInd_s {
    svc_MsgFwk_Hdr_t        hdr;
    uint16_t                numCh : 6;
    uint16_t                numSmplSet : 6;
    svc_PwrMonEh_SmplFmt_t  smplFmt : 4;
    uint16_t                seq;
    svc_PwrMonEh_ChBitmap_t chBitmap;
    // For performance reasons, keep data on 32-bit boundary
    uint32_t smplStartIdx;
    svc_PwrMonEh_SmplData_t
        data[SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX * (SVC_PWRMONEH_DATA_IND_CHANNELS_MAX + 1)];
} svc_PwrMonEh_DataInd_t;

// Sample Ind built and processed for every sample-time. It contains raw voltage
// readings from devices without any calibration applied. For now, this message
// is not "sent", it's processed through a direct callback
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SampleData_s {
    int32_t vBus;
    int32_t vShunt;
} svc_PwrMonEh_SampleData_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SampleInd_s {
    svc_MsgFwk_Hdr_t          hdr;
    uint16_t                  numCh;
    uint32_t                  seq;
    svc_PwrMonEh_IoBitmap_t   ioBitmap;
    svc_PwrMonEh_ChBitmap_t   chBitmap;
    svc_PwrMonEh_SampleData_t data[SVC_PWRMONEH_DATA_IND_CHANNELS_MAX];
} svc_PwrMonEh_SampleInd_t;

//----------------------
// StatsReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StatsReq_s {
    svc_MsgFwk_Hdr_t hdr;
    bool_t           reset;
} svc_PwrMonEh_StatsReq_t;

//----------------------
// StatsCnf
/*============================================================================*/
// Stats made available outside
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChannelStats_s {
    uint32_t pktSndNum;
    uint32_t pktErrNum;
} svc_PwrMonEh_ChannelStats_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SamplerStats_s {
    uint64_t smplNum;
    uint32_t smplErrNum;
} svc_PwrMonEh_SamplerStats_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StatsCnf_s {
    svc_MsgFwk_Hdr_t            hdr;
    svc_PwrMonEh_SamplerStats_t samplerStats;
    svc_PwrMonEh_ChannelStats_t channelStats;
} svc_PwrMonEh_StatsCnf_t;

//----------------------
// ChAvgReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgReq_s {
    svc_MsgFwk_Hdr_t hdr;
    bool_t           reset;
} svc_PwrMonEh_ChAvgReq_t;

//----------------------
// ChAvgCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgInfo_s {
    uint8_t chId;
    int32_t mvBusAvg;
    int32_t uvShuntAvg;
    int32_t uaShuntAvg;
    int32_t uwAvg;
} svc_PwrMonEh_ChAvgInfo_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgCnf_s {
    svc_MsgFwk_Hdr_t         hdr;
    uint8_t                  chCnt;
    svc_PwrMonEh_ChAvgInfo_t avgArray[];
} svc_PwrMonEh_ChAvgCnf_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_PwrMonEh_info;

/*==============================================================================
 *                                Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMonEh_buildAndSendStopInd(svc_PwrMonEh_Status_t status);
