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
 * @file svc_HumidEh.c
 * @brief Contains the Event handler for Service layer temperature/humidity messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Button.h"
#include "bsp_Mcu.h"
#include "svc_HumidEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"
#include "dev_Temp.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_NONE
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_HUMID)
/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_HUMIDEH_MEAS_POLLING_PERIOD_MS 500
#define SVC_HUMIDEH_MEAS_POLLING_TIMER_ID  "HUMIDEH"

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
osapi_Timer_t svc_HumidEh_timer;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

#if 0    // for testing
/*============================================================================*/
static void
svc_HumidEh_buildAndSendMeasHumidityInd( dev_Humid_MeasHumidity_t humidity )
{
    svc_HumidEh_MeasHumidityInd_t measInd;

    measInd.humidity = humidity;

    svc_MsgFwk_msgAllocAndBroadcast( SVC_HUMIDEH_MEAS_HUMIDITY_IND,
                                     sizeof(svc_HumidEh_MeasHumidityInd_t),
                                     SVC_MSGFWK_MSG_PAYLOAD_PTR(&measInd) );

    return;
}

/*============================================================================*/
static void
svc_HumidEh_measHandlerHumidity( dev_Humid_MeasHumidity_t humidity )
{
#if (SVC_LOG_LEVEL >= SVC_LOG_LEVEL_INFO)
    volatile int i=0;
    if( (i++ % 4) == 0 )
    {
        SVC_LOG_INFO( "humidity:%d.%d%%"NL, (humidity/64), ((humidity%64) * 100 / 64) );
    }
#endif
    svc_HumidEh_buildAndSendMeasHumidityInd( humidity );
    return;
}


/*============================================================================*/
static void
svc_HumidEh_buildAndSendMeasTemperatureInd( dev_Humid_MeasTemperature_t temperature )
{
    svc_HumidEh_MeasTemperatureInd_t measInd;

    measInd.temperature = temperature;

    svc_MsgFwk_msgAllocAndBroadcast( SVC_HUMIDEH_MEAS_TEMPERATURE_IND,
                                     sizeof(svc_HumidEh_MeasTemperatureInd_t),
                                     SVC_MSGFWK_MSG_PAYLOAD_PTR(&measInd) );

    return;
}

/*============================================================================*/
static void
svc_HumidEh_measHandlerTemperature( dev_Humid_MeasTemperature_t temperature )
{
#if (SVC_LOG_LEVEL >= SVC_LOG_LEVEL_INFO)
    volatile int i=0;
    if( (i++ % 4) == 0 )
    {
        SVC_LOG_INFO( "temperature:%d.%d"NL, (temperature/64), ((temperature%64) * 100 / 64) );
    }
#endif
    svc_HumidEh_buildAndSendMeasTemperatureInd( temperature );
    return;
}
#endif

/*============================================================================*/
static void svc_HumidEh_buildAndSendMeasComboInd(
    dev_Humid_MeasHumidity_t    humidity,
    dev_Humid_MeasTemperature_t temperature)
{
    svc_HumidEh_MeasComboInd_t measInd;

    measInd.humidity    = humidity;
    measInd.temperature = temperature;

    svc_MsgFwk_msgAllocAndBroadcast(
        SVC_HUMIDEH_MEAS_COMBO_IND,
        sizeof(svc_HumidEh_MeasComboInd_t),
        SVC_MSGFWK_MSG_PAYLOAD_PTR(&measInd));

    return;
}

/*============================================================================*/
static void svc_HumidEh_measHandlerCombo(
    dev_Humid_MeasHumidity_t    humidity,
    dev_Humid_MeasTemperature_t temperature)
{
#if (SVC_LOG_LEVEL >= SVC_LOG_LEVEL_INFO)
    volatile int i = 0;
    if ((i++ % 4) == 0) {
        SVC_LOG_INFO(
            "humidity:%d.%d%% temperature:%d.%d" NL,
            (humidity / 64),
            ((humidity % 64) * 100 / 64),
            (temperature / 64),
            ((temperature % 64) * 100 / 64));
    }
#endif
    svc_HumidEh_buildAndSendMeasComboInd(humidity, temperature);
    return;
}

/*============================================================================*/
void svc_HumidEh_timerCallback(osapi_Timer_t timer, osapi_TimerName_t name)
{
    dev_Humid_measTrigger(svc_HumidEh_measHandlerCombo);
    return;
}

/*============================================================================*/
static void svc_HumidEh_init(void)
{
    dev_Humid_init();
    svc_HumidEh_timer = osapi_Timer_periodicCreate(
        SVC_HUMIDEH_MEAS_POLLING_TIMER_ID,
        SVC_HUMIDEH_MEAS_POLLING_PERIOD_MS,
        svc_HumidEh_timerCallback);
    osapi_Timer_periodicStart(svc_HumidEh_timer);
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_HumidEh_info = {
    SVC_EHID_HUMID,
    0,       // bcastListLen
    NULL,    // bcastList
    svc_HumidEh_init,
    NULL    // msgHandler
};
#endif
