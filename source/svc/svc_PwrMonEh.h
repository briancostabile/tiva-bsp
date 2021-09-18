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
#define SVC_PWRMONEH_MSG_ID_DATA_IND    6
#define SVC_PWRMONEH_MSG_ID_STATS_REQ   7
#define SVC_PWRMONEH_MSG_ID_STATS_CNF   8
#define SVC_PWRMONEH_MSG_ID_CH_AVG_REQ  9
#define SVC_PWRMONEH_MSG_ID_CH_AVG_CNF  10


#define SVC_PWRMONEH_MSG_ID_NAMES_TABLE \
{                                       \
    "StartReq",                         \
    "StartCnf",                         \
    "StopReq",                          \
    "StopCnf",                          \
    "ConfigReq",                        \
    "ConfigCnf",                        \
    "DataInd",                          \
    "StatsReq",                         \
    "StatsCnf",                         \
    "ChAvgReq",                         \
    "ChAvgCnf",                         \
}

#define SVC_PWRMONEH_START_REQ      SCV_PWRMONEH_MSG_ID_BUILD( START, REQ )
#define SVC_PWRMONEH_START_CNF      SCV_PWRMONEH_MSG_ID_BUILD( START, CNF )
#define SVC_PWRMONEH_STOP_REQ       SCV_PWRMONEH_MSG_ID_BUILD( STOP, REQ )
#define SVC_PWRMONEH_STOP_CNF       SCV_PWRMONEH_MSG_ID_BUILD( STOP, CNF )
#define SVC_PWRMONEH_CONFIG_REQ     SCV_PWRMONEH_MSG_ID_BUILD( CONFIG, REQ )
#define SVC_PWRMONEH_CONFIG_CNF     SCV_PWRMONEH_MSG_ID_BUILD( CONFIG, CNF )
#define SVC_PWRMONEH_DATA_IND       SCV_PWRMONEH_MSG_ID_BUILD( DATA, IND )
#define SVC_PWRMONEH_STATS_REQ      SCV_PWRMONEH_MSG_ID_BUILD( STATS, REQ )
#define SVC_PWRMONEH_STATS_CNF      SCV_PWRMONEH_MSG_ID_BUILD( STATS, CNF )
#define SVC_PWRMONEH_CH_AVG_REQ     SCV_PWRMONEH_MSG_ID_BUILD( CH_AVG, REQ )
#define SVC_PWRMONEH_CH_AVG_CNF     SCV_PWRMONEH_MSG_ID_BUILD( CH_AVG, CNF )

/*==============================================================================
 *                                Types
 *============================================================================*/
#define SVC_PWRMONEH_STATUS_SUCCESS 0
#define SVC_PWRMONEH_STATUS_FAILURE 1
typedef uint8_t svc_PwrMonEh_Status_t;

#define SVC_PWRMONEH_SMPL_FMT0_IV  0
#define SVC_PWRMONEH_SMPL_FMT1_IOW 1
typedef uint8_t svc_PwrMonEh_SmplFmt_t;

typedef uint8_t svc_PwrMonEh_ChId_t;

/*============================================================================*/
typedef uint32_t svc_PwrMonEh_ChBitmap_t;


/*============================================================================*/
// Event handler message structures
//----------------------
// StartReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_StartReq_t;

//----------------------
// StartCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StartCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StartCnf_t;

//----------------------
// StopReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopReq_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_PwrMonEh_StopReq_t;

//----------------------
// StopCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StopCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_StopCnf_t;

//----------------------
// ConfigReq
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

//----------------------
// ConfigCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ConfigCnf_s
{
    svc_MsgFwk_Hdr_t      hdr;
    svc_PwrMonEh_Status_t status;
} svc_PwrMonEh_ConfigCnf_t;


//----------------------
// Data Indication
// Max channels per sample-time
#define SVC_PWRMONEH_DATA_IND_CHANNELS_MAX 10

// Samples Per Packet: 50 sample sets at 2500Hz = 20ms worth of samples
#define SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX 50

// Fmt0 is 16bit signed current and voltage
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SmplDataFmt0_s
{
    int16_t v;
    int16_t i;
} svc_PwrMonEh_SmplDataFmt0_t;

// Fmt0 is 16bit signed current and voltage
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SmplDataFmt1_s
{
    uint8_t iobitmap : 8;
    int32_t w        : 24;
} svc_PwrMonEh_SmplDataFmt1_t;

typedef union
{
    svc_PwrMonEh_SmplDataFmt0_t fmt0;
    svc_PwrMonEh_SmplDataFmt1_t fmt1;
} svc_PwrMonEh_SmplData_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_DataInd_s
{
    svc_MsgFwk_Hdr_t        hdr;
    uint16_t                numCh      : 6;
    uint16_t                numSmplSet : 6;
    svc_PwrMonEh_SmplFmt_t  smplFmt    : 4;
    uint16_t                seq;
    uint32_t                smplStartIdx;
    svc_PwrMonEh_ChBitmap_t chBitmap;
    svc_PwrMonEh_SmplData_t data[ SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX * SVC_PWRMONEH_DATA_IND_CHANNELS_MAX ];
} svc_PwrMonEh_DataInd_t;


//----------------------
// StatsReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StatsReq_s
{
    svc_MsgFwk_Hdr_t hdr;
    bool_t           reset;
} svc_PwrMonEh_StatsReq_t;

//----------------------
// StatsCnf
/*============================================================================*/
// Stats made available outside
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_SamplerStats_s
{
    uint64_t smplNum;
    uint32_t smplErrNum;
    uint32_t pktSndNum;
    uint32_t pktErrNum;
} svc_PwrMonEh_SamplerStats_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_StatsCnf_s
{
    svc_MsgFwk_Hdr_t            hdr;
    svc_PwrMonEh_SamplerStats_t samplerStats;
} svc_PwrMonEh_StatsCnf_t;


//----------------------
// ChAvgReq
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgReq_s
{
    svc_MsgFwk_Hdr_t hdr;
    bool_t           reset;
} svc_PwrMonEh_ChAvgReq_t;

//----------------------
// ChAvgCnf
typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgInfo_s
{
    uint8_t                 chId;
    svc_PwrMonEh_SmplData_t avg;
} svc_PwrMonEh_ChAvgInfo_t;

typedef struct BSP_ATTR_PACKED svc_PwrMonEh_ChAvgCnf_s
{
    svc_MsgFwk_Hdr_t         hdr;
    uint8_t                  chCnt;
    svc_PwrMonEh_ChAvgInfo_t avgArray[];
} svc_PwrMonEh_ChAvgCnf_t;


/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_PwrMonEh_info;
