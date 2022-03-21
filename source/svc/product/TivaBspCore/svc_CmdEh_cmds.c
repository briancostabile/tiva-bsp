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
 * @file svc_CmdEh_cmds.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_CmdEh.h"
#include "svc_CmdEh_cmds.h"
#include "bsp_Build.h"

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
int32_t svc_CmdEh_handlerBinary(svc_CmdEh_CmdHdr_t *cmdPtr, svc_CmdEh_binarySendCallback_t cb)
{
    switch (cmdPtr->cmdId) {
        case SVC_CMDEH_CMD_ID_VERSION: {
            svc_CmdEh_CmdVersionCnf_t cnf;
            cnf.hdr.len   = sizeof(cnf) - sizeof(uint16_t);
            cnf.hdr.cmdId = SVC_CMDEH_CMD_ID_VERSION;
            cnf.verHw     = bsp_Build_versionHw;
            cnf.verFw     = bsp_Build_versionFw;
            cb((uint8_t *)&cnf, sizeof(cnf));
        } break;

        case SVC_CMDEH_CMD_ID_EHSTATS: {
            svc_CmdEh_CmdEhStatsCnf_t cnf;
            cnf.hdr.len                  = sizeof(cnf) - sizeof(uint16_t);
            cnf.hdr.cmdId                = SVC_CMDEH_CMD_ID_EHSTATS;
            svc_MsgFwk_Stats_t *statsPtr = svc_MsgFwk_getStats();
            memcpy(&cnf.stats, statsPtr, sizeof(svc_MsgFwk_Stats_t));
            cb((uint8_t *)&cnf, sizeof(cnf));
        } break;

        case SVC_CMDEH_CMD_ID_BCAST_REG: {
            svc_CmdEh_CmdBcastRegCnf_t cnf;
            cnf.hdr.len   = sizeof(cnf) - sizeof(uint16_t);
            cnf.hdr.cmdId = SVC_CMDEH_CMD_ID_BCAST_REG;
            svc_MsgFwk_registerMsg(
                ((svc_CmdEh_CmdBcastRegReq_t *)cmdPtr)->eh,
                ((svc_CmdEh_CmdBcastRegReq_t *)cmdPtr)->msgId);
            cb((uint8_t *)&cnf, sizeof(cnf));
        } break;

        case SVC_CMDEH_CMD_ID_EHMSG: {
            svc_MsgFwk_Hdr_t *msgPtr;
            svc_MsgFwk_Hdr_t *rcvMsgPtr;
            uint16_t          rcvMsgLen;

            rcvMsgPtr = (svc_MsgFwk_Hdr_t *)(((svc_CmdEh_CmdEhMsg_t *)cmdPtr)->data);
            rcvMsgLen = (cmdPtr->len - sizeof(uint16_t));
            msgPtr    = svc_MsgFwk_msgAlloc(rcvMsgPtr->eh, rcvMsgPtr->id, rcvMsgLen);

            // Don't copy over the newly created header
            uint8_t *dstPtr = ((uint8_t *)msgPtr + sizeof(svc_MsgFwk_Hdr_t));
            uint8_t *srcPtr = ((uint8_t *)rcvMsgPtr + sizeof(svc_MsgFwk_Hdr_t));
            rcvMsgLen -= sizeof(svc_MsgFwk_Hdr_t);
            memcpy(dstPtr, srcPtr, rcvMsgLen);

            svc_MsgFwk_msgSend(msgPtr);
        } break;
    }
    return (0);
}
