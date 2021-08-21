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
 * @file svc_Eh.c
 * @brief Contains the utility functions for event handlers
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "svc_EhId.h"
#include "svc_Eh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/

/*==============================================================================
 *                              Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_Eh_init( const svc_Eh_Info_t* infoPtr,
             osapi_Queue_t        eventQueue )
{
    // Call initialization function
    if( infoPtr->initHandler != NULL )
    {
        infoPtr->initHandler();
    }

    // Register Event Handler to Queue mapping
    svc_MsgFwk_registerEh( infoPtr->eh, eventQueue );

    // Register for broadcast messages
    for( size_t bcastIdx=0; bcastIdx<infoPtr->bcastListLen; bcastIdx++ )
    {
        svc_MsgFwk_registerMsg( infoPtr->eh, infoPtr->bcastList[bcastIdx] );
    }

    return;
}

/*============================================================================*/
static void
svc_Eh_listInit( size_t                cnt,
                 const svc_Eh_Info_t** infoPtrList,
                 osapi_Queue_t         eventQueue )
{
    for( size_t i=0; i<cnt; i++ )
    {
        svc_Eh_init( infoPtrList[i], eventQueue );
    }
    return;
}

/*============================================================================*/
static void
svc_Eh_msgHandler( const svc_Eh_Info_t* infoPtr,
                   svc_MsgFwk_Hdr_t*    msgPtr )
{
    // Call message handler function
    if( (infoPtr->msgHandler != NULL) && (msgPtr != NULL) )
    {
        svc_EhId_t dstEh = (SVC_MSGFWK_MSG_ID_TYPE_GET( msgPtr->id ) == SVC_MSGFWK_MSG_TYPE_CNF_IND) ?
                               msgPtr->eh :
                               SVC_MSGFWK_MSG_ID_EH_GET(msgPtr->id);

        if( (dstEh == SVC_EHID_BROADCAST) || (dstEh == infoPtr->eh) )
        {
            infoPtr->msgHandler( msgPtr );
        }
    }

    return;
}

/*============================================================================*/
static void
svc_Eh_listMsgHandler( size_t                 cnt,
                       const svc_Eh_Info_t**  infoPtrList,
                       svc_MsgFwk_Hdr_t*      msgPtr )
{
    // Broadcast messages go to all event handlers in the list
    for( size_t i=0; i<cnt; i++ )
    {
        svc_Eh_msgHandler( infoPtrList[i], msgPtr );
    }

    // Release the original copy passed into this function
    svc_MsgFwk_msgRelease( msgPtr );

    return;
}

/*==============================================================================
 *                                Functions
 *============================================================================*/
/*============================================================================*/
void
svc_Eh_listRun( size_t                cnt,
                const svc_Eh_Info_t** infoPtrList,
                size_t                queueDepth,
                void*                 queueMem )
{
    osapi_Queue_t eventQueue = NULL;

    // Create Queue
    if( (queueMem != NULL) && (queueDepth != 0) )
    {
        eventQueue = osapi_Queue_create( queueDepth,
                                         sizeof(svc_MsgFwk_Hdr_t*),
                                         queueMem );
    }

    svc_Eh_listInit( cnt, infoPtrList, eventQueue );

    for(;;)
    {
        svc_MsgFwk_Hdr_t* msgPtr;
        osapi_Queue_dequeue( eventQueue, &msgPtr, OSAPI_TIMEOUT_WAIT_FOREVER );
        svc_Eh_listMsgHandler( cnt, infoPtrList, msgPtr );
    }
}
