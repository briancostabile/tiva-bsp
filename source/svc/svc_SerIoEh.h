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
 * @file svc_SerIoEh.h
 * @brief Contains the message interface related to the Serial IO service
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Io.h"
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
// Event handler message IDs
#define SVC_SERIOEH_DATA_IND SVC_MSGFWK_MSG_ID_BUILD_IND( SVC_EHID_SERIO, 0 )


/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
// Event handler message structures
typedef struct BSP_ATTR_PACKED svc_SerIoEh_DataInd_s
{
    svc_MsgFwk_Hdr_t hdr;
} svc_SerIoEh_DataInd_t;
