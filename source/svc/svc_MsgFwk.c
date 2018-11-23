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
 * @file svc_MsgFwk.c
 * @brief Contains a message passing framework module
 */
#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"
#include "svc_MsgFwk.h"
#include "svc_EhId.h"
#include "svc_Eh.h"
#include "osapi.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"
#include <stdio.h>

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef struct BSP_ATTR_PACKED svc_MsgFwk_BcastElement_s
{
    svc_MsgFwk_MsgId_t id;
    svc_EhId_t         eh;
} svc_MsgFwk_BcastElement_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
void* svc_MsgFwk_queueTable[SVC_EHID_NUM_EHIDS];

svc_MsgFwk_BcastElement_t svc_MsgFwk_bcastTable[10];
uint8_t svc_MsgFwk_bcastTableIdx = 0;

/*============================================================================*/
void
svc_MsgFwk_msgEnqueue( osapi_Queue_t queue, void* msgPtr )
{
    // Increment reference count before sending
    BSP_MCU_CRITICAL_SECTION_ENTER();
    ((svc_MsgFwk_Hdr_t*)msgPtr)->cnt++;
    BSP_MCU_CRITICAL_SECTION_EXIT();

    osapi_Queue_enqueue( queue, &msgPtr );
}

/*============================================================================*/
svc_MsgFwk_Hdr_t*
svc_MsgFwk_msgAllocCopy( svc_EhId_t          eh,
                         svc_MsgFwk_MsgId_t  id,
                         svc_MsgFwk_MsgLen_t len,
                         void*               data )
{
    svc_MsgFwk_Hdr_t* msgPtr = svc_MsgFwk_msgAlloc( eh, id, len );
    void* dataPtr = (msgPtr + 1);

    if( len > sizeof(svc_MsgFwk_Hdr_t) )
    {
        memcpy( dataPtr, data, (len - sizeof(svc_MsgFwk_Hdr_t)) );
    }
    return( msgPtr );
}


/*==============================================================================
 *                               Functions
 *============================================================================*/
/*============================================================================*/
void*
svc_MsgFwk_msgAlloc( svc_EhId_t          eh,
                     svc_MsgFwk_MsgId_t  id,
                     svc_MsgFwk_MsgLen_t len )
{
    BSP_ASSERT( len >= sizeof(svc_MsgFwk_Hdr_t) );
    svc_MsgFwk_Hdr_t* msgPtr = osapi_Memory_alloc( len );
    if( msgPtr != NULL )
    {
        msgPtr->eh = eh;
        msgPtr->id = id;
        msgPtr->len = len;
    }
    BSP_ASSERT( msgPtr );
    return msgPtr;
}

/*============================================================================*/
void
svc_MsgFwk_msgSend( void* msgPtr )
{
    svc_EhId_t dstEh = (SVC_MSGFWK_MSG_ID_TYPE_GET( ((svc_MsgFwk_Hdr_t*)msgPtr)->id ) == SVC_MSGFWK_MSG_TYPE_CNF_IND) ?
                                                        ((svc_MsgFwk_Hdr_t*)msgPtr)->eh :
                                                        SVC_MSGFWK_MSG_ID_EH_GET( ((svc_MsgFwk_Hdr_t*)msgPtr)->id );

    svc_MsgFwk_msgEnqueue( svc_MsgFwk_queueTable[dstEh], msgPtr );
}

/*============================================================================*/
void
svc_MsgFwk_msgAllocAndSend( svc_EhId_t          eh,
                            svc_MsgFwk_MsgId_t  id,
                            svc_MsgFwk_MsgLen_t len,
                            void*               data )
{
    svc_MsgFwk_msgSend( svc_MsgFwk_msgAllocCopy( eh, id, len, data ) );
}

/*============================================================================*/
void
svc_MsgFwk_msgBroadcast( void* msgPtr )
{
    if( msgPtr != NULL )
    {
        BSP_MCU_CRITICAL_SECTION_ENTER();
        ((svc_MsgFwk_Hdr_t*)msgPtr)->cnt++;
        BSP_MCU_CRITICAL_SECTION_EXIT();

        for( int idx=0; idx<DIM(svc_MsgFwk_bcastTable); idx++ )
        {
            if( svc_MsgFwk_bcastTable[idx].id == ((svc_MsgFwk_Hdr_t*)msgPtr)->id )
            {
                svc_MsgFwk_msgEnqueue( svc_MsgFwk_queueTable[svc_MsgFwk_bcastTable[idx].eh], msgPtr );
            }
        }

        // If message wasn't sent anywhere then the memory will be freed here
        svc_MsgFwk_msgRelease( msgPtr );
    }
}

/*============================================================================*/
void
svc_MsgFwk_msgAllocAndBroadcast( svc_MsgFwk_MsgId_t  id,
                                 svc_MsgFwk_MsgLen_t len,
                                 void*               data )
{
    svc_MsgFwk_msgBroadcast( svc_MsgFwk_msgAllocCopy( SVC_EHID_BROADCAST, id, len, data ) );
}

/*============================================================================*/
void
svc_MsgFwk_msgZeroCopy( void* msgPtr )
{
    // Increment reference count before sending
    BSP_MCU_CRITICAL_SECTION_ENTER();
    ((svc_MsgFwk_Hdr_t*)msgPtr)->cnt++;
    BSP_MCU_CRITICAL_SECTION_EXIT();
}

/*============================================================================*/
void
svc_MsgFwk_msgRelease( void* msgPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    ((svc_MsgFwk_Hdr_t*)msgPtr)->cnt--;
    BSP_MCU_CRITICAL_SECTION_EXIT();

    if( ((svc_MsgFwk_Hdr_t*)msgPtr)->cnt == 0 )
    {
        osapi_Memory_free( msgPtr );
    }
}

/*============================================================================*/
void
svc_MsgFwk_registerMsg( svc_EhId_t         eh,
                        svc_MsgFwk_MsgId_t id )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    bool saveMapping = true;
    
    // Make sure message isn't already registered to go to underlying queue
    for( size_t i=0; i<svc_MsgFwk_bcastTableIdx; i++ )
    {
        if( (id == svc_MsgFwk_bcastTable[i].id) &&
            (svc_MsgFwk_queueTable[eh] == svc_MsgFwk_queueTable[svc_MsgFwk_bcastTable[i].eh]) )
        {
            saveMapping = false;
        }
    }

    if( saveMapping == true )
    {
        svc_MsgFwk_bcastTable[svc_MsgFwk_bcastTableIdx].id = id;
        svc_MsgFwk_bcastTable[svc_MsgFwk_bcastTableIdx].eh = eh;
        svc_MsgFwk_bcastTableIdx++;
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
}

/*============================================================================*/
void
svc_MsgFwk_registerEh( svc_EhId_t    eh,
                       osapi_Queue_t queue )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    svc_MsgFwk_queueTable[eh] = queue;
    BSP_MCU_CRITICAL_SECTION_EXIT();
}
