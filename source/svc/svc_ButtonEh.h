/**
 * Copyright 2017 Brian Costabile
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
 * @file svc_ButtonEh.h
 * @brief Contains the message interface related to the button service
 */
#ifndef SVC_BUTTON_EH_H
#define SVC_BUTTON_EH_H

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Button.h"
#include <stdint.h>
#include "svc_Eh.h"
#include "svc_MsgFwk.h"

/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
// Event handler message IDs
#define SVC_BUTTONEH_PRESS_IND       SVC_MSGFWK_MSG_ID_BUILD_IND( SVC_EHID_BUTTON, 0 )
#define SVC_BUTTONEH_LONG_PRESS_IND  SVC_MSGFWK_MSG_ID_BUILD_IND( SVC_EHID_BUTTON, 1 )
#define SVC_BUTTONEH_RELEASE_IND     SVC_MSGFWK_MSG_ID_BUILD_IND( SVC_EHID_BUTTON, 2 )


/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
// Event handler message structures
typedef struct BSP_ATTR_PACKED svc_ButtonEh_HwPressInd_s
{
    svc_MsgFwk_Hdr_t hdr;
    bsp_Button_Id_t  id;
} svc_ButtonEh_PressInd_t;

typedef svc_ButtonEh_PressInd_t svc_ButtonEh_LongPressInd_t;

typedef svc_ButtonEh_PressInd_t svc_ButtonEh_ReleaseInd_t;


/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const svc_Eh_Info_t svc_ButtonEh_info;

#endif