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
 * @file svc_LightEh.c
 * @brief Contains the Event handler for Service layer temperature/humidity messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Button.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "svc_LightEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"
#include "dev_Light.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_NONE
#endif
#include "svc_Log.h"

#include "bsp_Trace.h"

#if defined(SVC_EHID_LIGHT)
/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_LIGHTEH_MEAS_POLLING_PERIOD_MS 250
#define SVC_LIGHTEH_MEAS_POLLING_TIMER_ID  0


/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
osapi_Timer_t svc_LightEh_timer;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_LightEh_buildAndSendMeasInd( dev_Light_MeasLight_t light )
{
    svc_LightEh_MeasInd_t measInd;

    measInd.light = light;

    svc_MsgFwk_msgAllocAndBroadcast( SVC_LIGHTEH_MEAS_ALS_IND,
                                     sizeof(svc_LightEh_MeasInd_t),
                                     SVC_MSGFWK_MSG_PAYLOAD_PTR(&measInd) );

    return;
}

/*============================================================================*/
static void
svc_LightEh_measHandlerAls( dev_Light_MeasLight_t light )
{
#if (SVC_LOG_LEVEL >= SVC_LOG_LEVEL_INFO)
    volatile int i=0;
    if( (i++ % 4) == 0 )
    {
        SVC_LOG_INFO( "als:%d.%d"NL, (light/256), (((light%256) * 100) / 256) );
    }
#endif
    svc_LightEh_buildAndSendMeasInd( light );
}

/*============================================================================*/
static void
svc_LightEh_measHandlerIr( dev_Light_MeasLight_t light )
{
#if (SVC_LOG_LEVEL >= SVC_LOG_LEVEL_INFO)
    volatile int i=0;
    if( (i++ % 4) == 0 )
    {
        SVC_LOG_INFO( "ir :%d.%d"NL, (light/256), (((light%256) * 100) / 256) );
    }
#endif
    svc_LightEh_buildAndSendMeasInd( light );
}

/*============================================================================*/
void
svc_LightEh_timerCallback( osapi_Timer_t   timer,
                           osapi_TimerId_t id )
{
    static uint8_t i = 0;

    // Flip between measureing Als and Ir
    if( i++ & 0x01 )
    {
        dev_Light_measTriggerAls( svc_LightEh_measHandlerAls );
    }
    else
    {
        dev_Light_measTriggerIr( svc_LightEh_measHandlerIr );
    }
    return;
}


/*============================================================================*/
static void
svc_LightEh_init( void )
{
    dev_Light_init();
    svc_LightEh_timer = osapi_Timer_create( SVC_LIGHTEH_MEAS_POLLING_TIMER_ID,
                                            SVC_LIGHTEH_MEAS_POLLING_PERIOD_MS,
                                            OSAPI_TIMER_TYPE_PERIODIC,
                                            svc_LightEh_timerCallback );
    osapi_Timer_start( svc_LightEh_timer );
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_LightEh_info =
{
    SVC_EHID_LIGHT,
    0,    // bcastListLen
    NULL, // bcastList
    svc_LightEh_init,
    NULL  // msgHandler
};
#endif