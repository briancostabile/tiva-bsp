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
 * @file svc_TempEh.c
 * @brief Contains the Event handler for Service layer temperature/humidity messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Button.h"
#include "bsp_Mcu.h"
#include "svc_TempEh.h"
#include "svc_MsgFwk.h"
#include "svc_Log.h"
#include "osapi.h"
#include "dev_Temp.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_TEMPEH_MEAS_POLLING_PERIOD_MS 500
#define SVC_TEMPEH_MEAS_POLLING_TIMER_ID  0

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
osapi_Timer_t svc_TempEh_timer;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_TempEh_buildAndSendMeasInd( dev_Temp_MeasHumid_t humidity,
                                dev_Temp_MeasTemp_t  temperature )
{
    svc_TempEh_MeasInd_t measInd;

    measInd.humidity = humidity;
    measInd.temperature = temperature;

    svc_MsgFwk_msgAllocAndBroadcast( SVC_TEMPEH_MEAS_IND,
                                     sizeof(svc_TempEh_MeasInd_t),
                                     SVC_MSGFWK_MSG_PAYLOAD_PTR(&measInd) );

    return;
}

/*============================================================================*/
static void
svc_TempEh_measHandler( dev_Temp_MeasHumid_t humidity,
                        dev_Temp_MeasTemp_t  temperature )
{
#if 0
    volatile int i=0;
    if( (i++ % 4) == 0 )
    {
        printf( "humidity:%d.%d%% temperature:%d.%d"NL, (humidity/64), ((humidity%64) * 100 / 64), (temperature/64), ((temperature%64) * 100 / 64) );
    }
#endif
    svc_TempEh_buildAndSendMeasInd( humidity, temperature );
    return;
}


/*============================================================================*/
void
svc_TempEh_timerCallback( osapi_Timer_t   timer,
                          osapi_TimerId_t id )
{
    dev_Temp_measTrigger( svc_TempEh_measHandler );
    return;
}

/*============================================================================*/
static void
svc_TempEh_init( void )
{
    dev_Temp_init(svc_TempEh_measHandler);
    svc_TempEh_timer = osapi_Timer_create( SVC_TEMPEH_MEAS_POLLING_TIMER_ID,
                                           SVC_TEMPEH_MEAS_POLLING_PERIOD_MS,
                                           OSAPI_TIMER_TYPE_PERIODIC,
                                           svc_TempEh_timerCallback );
    osapi_Timer_start( svc_TempEh_timer );
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_TempEh_info =
{
    SVC_EHID_TEMP,
    0,    // bcastListLen
    NULL, // bcastList
    svc_TempEh_init,
    NULL  // msgHandler
};
