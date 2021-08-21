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
 * @file svc_LedEh.h
 * @brief Contains the message interface related to the button service
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Led.h"
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
// Event handler message IDs
#define SVC_LEDEH_SET_COLOR_REQ    SVC_MSGFWK_MSG_ID_BUILD_REQ( SVC_EHID_LED, 0 )
#define SVC_LEDEH_SET_COLOR_CNF    SVC_MSGFWK_MSG_ID_BUILD_CNF( SVC_EHID_LED, 1 )
#define SVC_LEDEH_SET_PATTERN_REQ  SVC_MSGFWK_MSG_ID_BUILD_REQ( SVC_EHID_LED, 2 )
#define SVC_LEDEH_SET_PATTERN_CNF  SVC_MSGFWK_MSG_ID_BUILD_CNF( SVC_EHID_LED, 3 )


/*==============================================================================
 *                                Types
 *============================================================================*/
#define SVC_LEDEH_CONTROL_DISABLE 0x00
#define SVC_LEDEH_CONTROL_ENABLE  0x01
typedef uint8_t svc_LedEh_Control_t;

/*============================================================================*/
// Event handler message structures
typedef struct BSP_ATTR_PACKED svc_LedEh_SetColorReq_s
{
    svc_MsgFwk_Hdr_t hdr;
    bsp_Led_Id_t     ledId;
    bsp_Led_Color_t  color;
} svc_LedEh_SetColorReq_t;

typedef struct BSP_ATTR_PACKED svc_LedEh_SetColorCnf_s
{
    svc_MsgFwk_Hdr_t hdr;
    bsp_Led_Id_t     ledId;
} svc_LedEh_SetColorCnf_t;


typedef struct BSP_ATTR_PACKED svc_LedEh_PatternElement_s
{
    bsp_Led_Color_t color;
    uint32_t        msTime;
} svc_LedEh_PatternElement_t;

typedef struct BSP_ATTR_PACKED svc_LedEh_SetPatternReq_s
{
    svc_MsgFwk_Hdr_t           hdr;
    bsp_Led_Id_t               ledId;
    uint8_t                    cnt;
    svc_LedEh_PatternElement_t patternTable[];
} svc_LedEh_SetPatternReq_t;

typedef struct BSP_ATTR_PACKED svc_LedEh_SetPatternCnf_s
{
    svc_MsgFwk_Hdr_t hdr;
    bsp_Led_Id_t     ledId;
} svc_LedEh_SetPatternCnf_t;


/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_LedEh_info;
/*============================================================================*/
void
svc_LedEh_buildAndSendSetColorReq( svc_EhId_t      eh,
                                   bsp_Led_Id_t    id,
                                   bsp_Led_Color_t color );

/*============================================================================*/
void
svc_LedEh_buildAndSendSetPatternReq( svc_EhId_t                  eh,
                                     bsp_Led_Id_t                id,
                                     uint8_t                     cnt,
                                     svc_LedEh_PatternElement_t* patternArray );
