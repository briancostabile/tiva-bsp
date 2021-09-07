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
 * @file svc_LedEh.c
 * @brief Contains the Event handler for Service layer button messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Led.h"
#include "bsp_Mcu.h"
#include "svc_LedEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_NONE
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_LED)
/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_LEDEH_PATTERN_TIMER_ID  "%d_LEDEH"

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
typedef struct BSP_ATTR_PACKED svc_LedEh_LedInfo_s
{
    char              name[9];
    osapi_Timer_t     timer;
    bsp_Led_Color_t   color;
    uint8_t           cnt;
    svc_LedEh_SetPatternReq_t* patternPtr;
} svc_LedEh_LedInfo_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_LedEh_LedInfo_t svc_LedEh_ledInfoTable[BSP_PLATFORM_LED_NUM];

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_LedEh_buildAndSendSetColorCnf( svc_EhId_t   eh,
                                   bsp_Led_Id_t ledId )
{
    svc_LedEh_SetColorCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_LEDEH_SET_COLOR_CNF, sizeof(svc_LedEh_SetColorCnf_t) );
    cnfPtr->ledId = ledId;
    svc_MsgFwk_msgSend( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_LedEh_buildAndSendSetPatternCnf( svc_EhId_t   eh,
                                     bsp_Led_Id_t ledId )
{
    svc_LedEh_SetPatternCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_LEDEH_SET_PATTERN_CNF, sizeof(svc_LedEh_SetPatternCnf_t) );
    cnfPtr->ledId = ledId;
    svc_MsgFwk_msgSend( cnfPtr );
    return;
}

/*============================================================================*/
void
svc_LedEh_processPattern( svc_LedEh_LedInfo_t* ledInfoPtr )
{
    if( ledInfoPtr->cnt < ledInfoPtr->patternPtr->cnt )
    {
        svc_LedEh_PatternElement_t* elementPtr = &ledInfoPtr->patternPtr->patternTable[ ledInfoPtr->cnt ];

        ledInfoPtr->cnt++;
        ledInfoPtr->color = elementPtr->color;

        bsp_Led_setColor( ledInfoPtr->patternPtr->ledId, elementPtr->color );
        osapi_Timer_oneShotStart( ledInfoPtr->timer, elementPtr->msTime );
    }
    else if( ledInfoPtr->patternPtr != NULL )
    {
        svc_MsgFwk_msgRelease( ledInfoPtr->patternPtr );
        ledInfoPtr->patternPtr = NULL;
    }
    return;
}

/*============================================================================*/
void
svc_LedEh_msgHandlerColorReq( svc_LedEh_SetColorReq_t* reqPtr )
{
    SVC_LOG_INFO( "ColorReq:%ld"NL, reqPtr->ledId );
    if( reqPtr->ledId >= BSP_PLATFORM_LED_NUM ) { return; }

    bsp_Led_setColor( reqPtr->ledId, reqPtr->color  );

    if( reqPtr->hdr.eh != SVC_EHID_LED )
    {
        svc_LedEh_buildAndSendSetColorCnf( reqPtr->hdr.eh, reqPtr->ledId );
    }
    return;
}

/*============================================================================*/
void
svc_LedEh_msgHandlerPatternReq( svc_LedEh_SetPatternReq_t* reqPtr )
{
    SVC_LOG_INFO( "PatternReq:%ld"NL, reqPtr->ledId );
    if( reqPtr->ledId >= BSP_PLATFORM_LED_NUM ) { return; }
    svc_LedEh_LedInfo_t* ledInfoPtr = &svc_LedEh_ledInfoTable[ reqPtr->ledId ];

    if( ledInfoPtr->patternPtr != NULL )
    {
        svc_MsgFwk_msgRelease( ledInfoPtr->patternPtr );
    }

    svc_MsgFwk_msgZeroCopy( (svc_MsgFwk_Hdr_t*)reqPtr );
    ledInfoPtr->patternPtr = reqPtr;
    ledInfoPtr->cnt = 0;

    svc_LedEh_processPattern( ledInfoPtr );

    if( reqPtr->hdr.eh != SVC_EHID_LED )
    {
        svc_LedEh_buildAndSendSetPatternCnf( reqPtr->hdr.eh, reqPtr->ledId );
    }

    return;
}

/*============================================================================*/
void
svc_LedEh_msgHandler( svc_MsgFwk_Hdr_t* msgPtr )
{
    if( msgPtr->id == SVC_LEDEH_SET_COLOR_REQ )
    {
        svc_LedEh_msgHandlerColorReq( (svc_LedEh_SetColorReq_t*)msgPtr );
    }
    else if( msgPtr->id == SVC_LEDEH_SET_PATTERN_REQ )
    {
        svc_LedEh_msgHandlerPatternReq( (svc_LedEh_SetPatternReq_t*)msgPtr );
    }
    return;
}

/*============================================================================*/
void
svc_LedEh_timerCallback( osapi_Timer_t     timer,
                         osapi_TimerName_t name )
{
    int id = (name[0] - '0');
    svc_LedEh_processPattern( &svc_LedEh_ledInfoTable[id] );
    return;
}

/*============================================================================*/
static void
svc_LedEh_init( void )
{
    memset(svc_LedEh_ledInfoTable, 0 , sizeof(svc_LedEh_ledInfoTable));

    for( int i=0; i<DIM(svc_LedEh_ledInfoTable); i++ )
    {
        snprintf( svc_LedEh_ledInfoTable[i].name,
                  sizeof(svc_LedEh_ledInfoTable[i].name),
                  SVC_LEDEH_PATTERN_TIMER_ID, i );

        svc_LedEh_ledInfoTable[i].timer = osapi_Timer_oneShotCreate( svc_LedEh_ledInfoTable[i].name,
                                                                     svc_LedEh_timerCallback );
    }

    return;
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_LedEh_info =
{
    SVC_EHID_LED,
    0,    // bcastListLen
    NULL, // bcastList
    svc_LedEh_init,
    svc_LedEh_msgHandler
};

/*============================================================================*/
void
svc_LedEh_buildAndSendSetColorReq( svc_EhId_t      eh,
                                   bsp_Led_Id_t    ledId,
                                   bsp_Led_Color_t color )
{
    svc_LedEh_SetColorReq_t* reqPtr = svc_MsgFwk_msgAlloc( eh,
                                                           SVC_LEDEH_SET_COLOR_REQ,
                                                           sizeof(svc_LedEh_SetColorReq_t) );

    reqPtr->ledId = ledId;
    reqPtr->color = color;
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
void
svc_LedEh_buildAndSendSetPatternReq( svc_EhId_t                  eh,
                                     bsp_Led_Id_t                ledId,
                                     uint8_t                     cnt,
                                     svc_LedEh_PatternElement_t* patternArray )
{
    svc_LedEh_SetPatternReq_t* reqPtr = svc_MsgFwk_msgAlloc( eh,
                                                             SVC_LEDEH_SET_PATTERN_REQ,
                                                             sizeof(svc_LedEh_SetPatternReq_t) + (cnt * sizeof(svc_LedEh_PatternElement_t)) );

    reqPtr->ledId = ledId;
    reqPtr->cnt = cnt;
    for( int i=0; i<cnt; i++ )
    {
        memcpy( &reqPtr->patternTable[i],
                &patternArray[i],
                sizeof(svc_LedEh_PatternElement_t) );
    }
    svc_MsgFwk_msgSend( reqPtr );
    return;
}
#endif
