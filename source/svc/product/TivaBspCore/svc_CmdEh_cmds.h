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

#include "bsp_Types.h"
#include "bsp_Pragma.h"

/*==============================================================================
 *                                  Defines
 *============================================================================*/

#define SVC_CMDEH_CMD_ID_VERSION_REQ ((svc_CmdEh_CmdId_t)0)
#define SVC_CMDEH_CMD_ID_VERSION_CNF ((svc_CmdEh_CmdId_t)1)

typedef uint8_t svc_CmdEh_CmdId_t;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
typedef struct PACKED svc_CmdEh_CmdVersionReq_s {
    svc_CmdEh_CmdId_t cmdId;
} svc_CmdEh_CmdVersionReq_t;

/*============================================================================*/
typedef struct PACKED svc_CmdEh_CmdVersionCnf_s {
    svc_CmdEh_CmdId_t cmdId;
    
} svc_CmdEh_CmdVersionCnf_t;
