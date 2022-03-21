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
 * @file svc_Eh.h
 * @brief Contains the structures, definitions and functions common for all
 *        Event Handlers
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "svc_MsgFwk.h"
#include "svc_EhId.h"
#include "osapi.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef void (*svc_Eh_InitHandler_t)(void);
typedef void (*svc_Eh_MsgHandler_t)(svc_MsgFwk_Hdr_t *msgPtr);

/*============================================================================*/
typedef struct svc_Eh_Info_s {
    svc_EhId_t                eh;
    size_t                    bcastListLen;
    const svc_MsgFwk_MsgId_t *bcastList;
    svc_Eh_InitHandler_t      initHandler;
    svc_Eh_MsgHandler_t       msgHandler;
} svc_Eh_Info_t;

/*============================================================================*/
void svc_Eh_listRun(
    size_t                cnt,
    const svc_Eh_Info_t **infoPtrList,
    size_t                queueDepth,
    void *                queueMem);
