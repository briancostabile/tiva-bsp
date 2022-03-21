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
 * @file svc_CmdEh_cmds.h
 * @brief
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_Build.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                                  Defines
 *============================================================================*/

#define SVC_CMDEH_CMD_ID_VERSION   ((svc_CmdEh_CmdId_t)0x0001)
#define SVC_CMDEH_CMD_ID_EHSTATS   ((svc_CmdEh_CmdId_t)0x0002)
#define SVC_CMDEH_CMD_ID_EHMSG     ((svc_CmdEh_CmdId_t)0x0003)
#define SVC_CMDEH_CMD_ID_BCAST_REG ((svc_CmdEh_CmdId_t)0x0004)

typedef uint16_t svc_CmdEh_CmdId_t;

/*============================================================================*/
typedef void (*svc_CmdEh_binarySendCallback_t)(uint8_t *buf, uint16_t len);

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdHdr_s {
    uint16_t          len;    // Length does not include itself
    svc_CmdEh_CmdId_t cmdId;
} svc_CmdEh_CmdHdr_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdVersionReq_s {
    svc_CmdEh_CmdHdr_t hdr;
} svc_CmdEh_CmdVersionReq_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdVersionCnf_s {
    svc_CmdEh_CmdHdr_t  hdr;
    bsp_Build_Version_t verHw;
    bsp_Build_Version_t verFw;
} svc_CmdEh_CmdVersionCnf_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdEhStatsReq_s {
    svc_CmdEh_CmdHdr_t hdr;
} svc_CmdEh_CmdEhStatsReq_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdEhStatsCnf_s {
    svc_CmdEh_CmdHdr_t hdr;
    svc_MsgFwk_Stats_t stats;
} svc_CmdEh_CmdEhStatsCnf_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdBcastRegReq_s {
    svc_CmdEh_CmdHdr_t hdr;
    svc_EhId_t         eh;
    svc_MsgFwk_MsgId_t msgId;
} svc_CmdEh_CmdBcastRegReq_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdBcastRegCnf_s {
    svc_CmdEh_CmdHdr_t hdr;
} svc_CmdEh_CmdBcastRegCnf_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_CmdEh_CmdEhMsg_s {
    svc_CmdEh_CmdHdr_t hdr;
    uint8_t            data[];
} svc_CmdEh_CmdEhMsg_t;

/*==============================================================================
 *                              Functions
 *============================================================================*/
/*============================================================================*/
int32_t svc_CmdEh_handlerBinary(svc_CmdEh_CmdHdr_t *cmdPtr, svc_CmdEh_binarySendCallback_t cb);
