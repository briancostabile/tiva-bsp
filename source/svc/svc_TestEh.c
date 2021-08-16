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
 * @file svc_TestEh.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_TestEh.h"
#include "svc_ButtonEh.h"
#include "svc_Eh.h"
#include "osapi.h"


#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"


#include <stdio.h>
/*==============================================================================
 *                                  Defines
 *============================================================================*/


/*==============================================================================
 *                                 Globals
 *============================================================================*/
/*============================================================================*/
const svc_MsgFwk_MsgId_t svc_TestEh_bcastMsgIds[] =
{
    SVC_BUTTONEH_PRESS_IND,
    SVC_BUTTONEH_RELEASE_IND,
    SVC_BUTTONEH_LONG_PRESS_IND
};

/*==============================================================================
 *                              Local Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_TestEh_msgHandler( svc_MsgFwk_Hdr_t* msgPtr )
{
    switch( msgPtr->id )
    {
        case SVC_BUTTONEH_PRESS_IND:
        {
            printf("0x%04X pressed"NL, msgPtr->id);
        }
        break;

        case SVC_BUTTONEH_RELEASE_IND:
        {
            printf("0x%04X released"NL, msgPtr->id);
        }
        break;

        case SVC_BUTTONEH_LONG_PRESS_IND:
        {
            printf("0x%04X long pressed"NL, msgPtr->id);
        }
        break;
    }
    return;
}

/*============================================================================*/
static void
svc_TestEh_init( void )
{
    return;
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_TestEh_info =
{
    SVC_EHID_TEST,
    DIM(svc_TestEh_bcastMsgIds),
    svc_TestEh_bcastMsgIds,
    svc_TestEh_init,
    svc_TestEh_msgHandler
};
