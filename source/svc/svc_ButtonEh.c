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
 * @file svc_ButtonEh.c
 * @brief Contains the Event handler for Service layer button messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Button.h"
#include "bsp_Mcu.h"
#include "svc_ButtonEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"

#if defined(SVC_EHID_BUTTON)
/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_BUTTONEH_BUTTON_NAME              "%d_BUTTONEH"
#define SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS 100
#define SVC_BUTTONEH_BUTTON_LONG_PRESS_MS     2000
#define SVC_BUTTONEH_BUTTON_LONG_PRESS_POLL_COUNT   (SVC_BUTTONEH_BUTTON_LONG_PRESS_MS / SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS)

#define SVC_BUTTONEH_BUTTON_STATE_SET( _id, _state )                                \
{                                                                                   \
    svc_ButtonEh_buttonInfoTable[(_id)].state = SVC_BUTTONEH_BUTTON_STATE_##_state; \
}

#define SVC_BUTTONEH_BUTTON_STATE_GET( _id ) svc_ButtonEh_buttonInfoTable[(_id)].state

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
typedef enum {
    SVC_BUTTONEH_BUTTON_STATE_UNPRESSED  = 0,
    SVC_BUTTONEH_BUTTON_STATE_DEBOUNCING = 1,
    SVC_BUTTONEH_BUTTON_STATE_PRESS      = 2,
    SVC_BUTTONEH_BUTTON_STATE_LONG_PRESS = 3
} svc_ButtonEh_ButtonState_t;

typedef struct BSP_ATTR_PACKED svc_ButtonEh_ButtonInfo_s
{
    char                       name[12];
    int8_t                     count : 8;
    svc_ButtonEh_ButtonState_t state : 8;
    osapi_Timer_t              timer;
} svc_ButtonEh_ButtonInfo_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_ButtonEh_ButtonInfo_t svc_ButtonEh_buttonInfoTable[BSP_PLATFORM_IO_BUTTON_NUM];

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_ButtonEh_buildAndSendPressInd( bsp_Button_Id_t id )
{
    svc_MsgFwk_msgAllocAndBroadcast( SVC_BUTTONEH_PRESS_IND,
                                     sizeof(svc_ButtonEh_PressInd_t),
                                     &id );

    return;
}

/*============================================================================*/
static void
svc_ButtonEh_buildAndSendReleaseInd( bsp_Button_Id_t id )
{
    svc_MsgFwk_msgAllocAndBroadcast( SVC_BUTTONEH_RELEASE_IND,
                                     sizeof(svc_ButtonEh_ReleaseInd_t),
                                     &id );

    return;
}

/*============================================================================*/
static void
svc_ButtonEh_buildAndSendLongPressInd( bsp_Button_Id_t id )
{
    svc_MsgFwk_msgAllocAndBroadcast( SVC_BUTTONEH_LONG_PRESS_IND,
                                     sizeof(svc_ButtonEh_LongPressInd_t),
                                     &id );

    return;
}

/*============================================================================*/
void
svc_ButtonEh_timerCallback( osapi_Timer_t     timer,
                            osapi_TimerName_t name )
{
    // check state of button and possibly continue the timer. Timer Id is Button Id
    BSP_MCU_CRITICAL_SECTION_ENTER();

    int id = (name[0] - '0');

    switch( SVC_BUTTONEH_BUTTON_STATE_GET( id ) )
    {
        case SVC_BUTTONEH_BUTTON_STATE_PRESS:
        {
            if( bsp_Button_state( id ) != BSP_BUTTON_STATE_PRESSED )
            {
                SVC_BUTTONEH_BUTTON_STATE_SET( id, UNPRESSED );
                bsp_Button_control( id, BSP_BUTTON_CONTROL_ENABLE );
                svc_ButtonEh_buildAndSendReleaseInd( id );
            }
            else
            {
                svc_ButtonEh_buttonInfoTable[ id ].count--;
                if( svc_ButtonEh_buttonInfoTable[ id ].count <= 0 )
                {
                    SVC_BUTTONEH_BUTTON_STATE_SET( id, LONG_PRESS );
                    svc_ButtonEh_buildAndSendLongPressInd( id );
                }
                osapi_Timer_oneShotStart( svc_ButtonEh_buttonInfoTable[id].timer,
                                          SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS );
            }
        }
        break;

        case SVC_BUTTONEH_BUTTON_STATE_LONG_PRESS:
        {
            if( bsp_Button_state( id ) != BSP_BUTTON_STATE_PRESSED )
            {
                SVC_BUTTONEH_BUTTON_STATE_SET( id, UNPRESSED );
                bsp_Button_control( id, BSP_BUTTON_CONTROL_ENABLE );
                svc_ButtonEh_buildAndSendReleaseInd( id );
            }
            else
            {
                osapi_Timer_oneShotStart( svc_ButtonEh_buttonInfoTable[id].timer,
                                          SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS );
            }
        }
        break;

        case SVC_BUTTONEH_BUTTON_STATE_UNPRESSED:
        case SVC_BUTTONEH_BUTTON_STATE_DEBOUNCING:
        default:
        {
            if( bsp_Button_state( id ) == BSP_BUTTON_STATE_PRESSED )
            {
                SVC_BUTTONEH_BUTTON_STATE_SET( id, PRESS );
                svc_ButtonEh_buttonInfoTable[ id ].count = (SVC_BUTTONEH_BUTTON_LONG_PRESS_POLL_COUNT - 1);
                osapi_Timer_oneShotStart( svc_ButtonEh_buttonInfoTable[id].timer,
                                          SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS );
                svc_ButtonEh_buildAndSendPressInd( id );
            }
            else
            {
                SVC_BUTTONEH_BUTTON_STATE_SET( id, UNPRESSED );
                bsp_Button_control( id, BSP_BUTTON_CONTROL_ENABLE );
            }
        }
        break;

    }
    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
static void
svc_ButtonEh_buttonHandler( bsp_Button_Id_t id )
{
    // Disable interrupt and start polling with timer
    bsp_Button_control( id, BSP_BUTTON_CONTROL_DISABLE );

    BSP_MCU_CRITICAL_SECTION_ENTER();
    SVC_BUTTONEH_BUTTON_STATE_SET( id, DEBOUNCING );
    BSP_MCU_CRITICAL_SECTION_EXIT();

    osapi_Timer_oneShotStart( svc_ButtonEh_buttonInfoTable[id].timer,
                              SVC_BUTTONEH_BUTTON_POLLING_PERIOD_MS );
    return;
}

/*============================================================================*/
static void
svc_ButtonEh_init( void )
{
    memset( svc_ButtonEh_buttonInfoTable, 0, sizeof(svc_ButtonEh_buttonInfoTable) );

    for( uint8_t i=0; i<BSP_PLATFORM_IO_BUTTON_NUM; i++ )
    {
        bsp_Button_registerHandler( i, svc_ButtonEh_buttonHandler );
        bsp_Button_control( i, BSP_BUTTON_CONTROL_ENABLE );
        snprintf( svc_ButtonEh_buttonInfoTable[i].name,
                  sizeof(svc_ButtonEh_buttonInfoTable[i].name),
                  SVC_BUTTONEH_BUTTON_NAME, i );
        svc_ButtonEh_buttonInfoTable[i].timer = osapi_Timer_oneShotCreate( svc_ButtonEh_buttonInfoTable[i].name,
                                                                           svc_ButtonEh_timerCallback );
    }
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_ButtonEh_info =
{
    SVC_EHID_BUTTON,
    0,    // bcastListLen
    NULL, // bcastList
    svc_ButtonEh_init,
    NULL  // msgHandler
};
#endif
