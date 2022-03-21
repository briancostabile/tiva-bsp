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
 * @file svc_MsgFwk.h
 * @brief Contains a message passing framework API to abstract the concept of
 *        an event handler from the underlying thread
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_EhId.h"
#include <stdint.h>
#include "osapi.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
#define SVC_MSGFWK_MSG_TYPE_REQ_RSP 0
#define SVC_MSGFWK_MSG_TYPE_CNF_IND 1

#define SVC_MSGFWK_MSG_ID_BUILD(_e, _t, _n) (((_e) << 8) | ((_t) << 7) | (_n))

#define SVC_MSGFWK_MSG_ID_BUILD_REQ(_eh, _num) \
    SVC_MSGFWK_MSG_ID_BUILD((_eh), SVC_MSGFWK_MSG_TYPE_REQ_RSP, (_num))
#define SVC_MSGFWK_MSG_ID_BUILD_RSP(_eh, _num) \
    SVC_MSGFWK_MSG_ID_BUILD((_eh), SVC_MSGFWK_MSG_TYPE_REQ_RSP, (_num))
#define SVC_MSGFWK_MSG_ID_BUILD_CNF(_eh, _num) \
    SVC_MSGFWK_MSG_ID_BUILD((_eh), SVC_MSGFWK_MSG_TYPE_CNF_IND, (_num))
#define SVC_MSGFWK_MSG_ID_BUILD_IND(_eh, _num) \
    SVC_MSGFWK_MSG_ID_BUILD((_eh), SVC_MSGFWK_MSG_TYPE_CNF_IND, (_num))

#define SVC_MSGFWK_MSG_ID_EH_GET(_id)   (((_id)&0xFF00) >> 8)
#define SVC_MSGFWK_MSG_ID_TYPE_GET(_id) (((_id)&0x0080) >> 7)
#define SVC_MSGFWK_MSG_ID_NUM_GET(_id)  (((_id)&0x007F) >> 0)

#define SVC_MSGFWK_MSG_PAYLOAD_PTR(_hdr) (uint8_t *)(((svc_MsgFwk_Hdr_t *)(_hdr)) + 1)
#define SVC_MSGFWK_MSG_SYSTEM_PTR(_hdr) \
    (uint8_t *)(((uint8_t *)(_hdr)) - sizeof(svc_MsgFwk_SysData_t))

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef uint8_t  svc_MsgFwk_MsgNum_t;
typedef uint16_t svc_MsgFwk_MsgLen_t;
typedef uint8_t  svc_MsgFwk_MsgType_t;
typedef uint8_t  svc_MsgFwk_RefCnt_t;
typedef uint8_t  svc_MsgFwk_Alloc_t;
typedef uint16_t svc_MsgFwk_MsgId_t;
typedef uint32_t svc_MsgFwk_SysData_t;

typedef struct BSP_ATTR_PACKED svc_MsgFwk_Hdr_s {
    svc_MsgFwk_MsgId_t  id;
    svc_EhId_t          eh;
    svc_MsgFwk_Alloc_t  alloc : 1;
    svc_MsgFwk_RefCnt_t cnt : 7;
    svc_MsgFwk_MsgLen_t len;
} svc_MsgFwk_Hdr_t;

typedef struct BSP_ATTR_PACKED svc_MsgFwk_Stats_s {
    uint32_t numAlloc;
    uint32_t numFree;
    uint32_t numSend;
    uint32_t numBcast;
    uint8_t  bcastMapTotal;
    uint8_t  bcastMapAvail;
    uint8_t  numEhId;
} svc_MsgFwk_Stats_t;

typedef void (*svc_MsgFwk_LoggerCallback_t)(svc_MsgFwk_Hdr_t *msgPtr);

/*============================================================================*/
void *svc_MsgFwk_msgAlloc(svc_EhId_t eh, svc_MsgFwk_MsgId_t id, svc_MsgFwk_MsgLen_t len);

/*============================================================================*/
void svc_MsgFwk_msgSend(void *data);

/*============================================================================*/
void svc_MsgFwk_msgAllocAndSend(
    svc_EhId_t          eh,
    svc_MsgFwk_MsgId_t  id,
    svc_MsgFwk_MsgLen_t len,
    void *              data);

/*============================================================================*/
void svc_MsgFwk_msgBroadcast(void *msgPtr);

/*============================================================================*/
void svc_MsgFwk_msgAllocAndBroadcast(svc_MsgFwk_MsgId_t id, svc_MsgFwk_MsgLen_t len, void *data);

/*============================================================================*/
void svc_MsgFwk_msgZeroCopy(void *msgPtr);

/*============================================================================*/
void svc_MsgFwk_msgRelease(void *msgPtr);

/*============================================================================*/
void svc_MsgFwk_registerMsg(svc_EhId_t eh, svc_MsgFwk_MsgId_t id);

/*============================================================================*/
void svc_MsgFwk_registerEh(svc_EhId_t eh, osapi_Queue_t queue);

/*============================================================================*/
void svc_MsgFwk_registerProxyEh(svc_EhId_t eh);

/*============================================================================*/
svc_EhId_t svc_MsgFwk_getProxyEh(void);

/*============================================================================*/
void svc_MsgFwk_registerLogger(svc_MsgFwk_LoggerCallback_t cb);

/*============================================================================*/
svc_MsgFwk_Stats_t *svc_MsgFwk_getStats(void);
