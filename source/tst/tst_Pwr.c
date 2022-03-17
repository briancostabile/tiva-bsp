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
 * @file tst_Pwr.c
 * @brief Test functions for Power Monitor
 */

#include "bsp_Types.h"
#include "tst.h"
#include "tst_Sys.h"
#include "osapi.h"
#include "svc_MsgFwk.h"
#include "svc_PwrMon_channel.h"
#include "svc_PwrMon_sampler.h"
#include "svc_PwrMonEh.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#if defined(SVC_EHID_PWRMON)
#include "bsp_Ssi.h"

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// String globals
TST_STR_CMD( TST_PWR_CAL_STR_CMD,         "cal" );
TST_STR_HLP( TST_PWR_CAL_STR_HLP,         "Calibrate Power Monitor" );
TST_STR_CMD( TST_PWR_CONFIG_STR_CMD,      "config" );
TST_STR_HLP( TST_PWR_CONFIG_STR_HLP,      "Config Power Monitor" );
TST_STR_CMD( TST_PWR_START_STR_CMD,       "start" );
TST_STR_HLP( TST_PWR_START_STR_HLP,       "Start Power Monitor" );
TST_STR_CMD( TST_PWR_STOP_STR_CMD,        "stop" );
TST_STR_HLP( TST_PWR_STOP_STR_HLP,        "Stop Power Monitor" );
TST_STR_CMD( TST_PWR_STATS_STR_CMD,       "stats" );
TST_STR_HLP( TST_PWR_STATS_STR_HLP,       "Get Stats" );
TST_STR_CMD( TST_PWR_STATS_RESET_STR_CMD, "statsReset" );
TST_STR_HLP( TST_PWR_STATS_RESET_STR_HLP, "Reset Stats" );
TST_STR_CMD( TST_PWR_SHORT_EN_STR_CMD,    "shortEn" );
TST_STR_HLP( TST_PWR_SHORT_EN_STR_HLP,    "Enable Calibration Short" );
TST_STR_CMD( TST_PWR_SHORT_DIS_STR_CMD,   "shortDis" );
TST_STR_HLP( TST_PWR_SHORT_DIS_STR_HLP,   "Enable Calibration Short" );
TST_STR_CMD( TST_PWR_VSET_STR_CMD,        "vset" );
TST_STR_HLP( TST_PWR_VSET_STR_HLP,        "Set Voltage Supply Voltage" );
TST_STR_CMD( TST_PWR_OEN_STR_CMD,         "oen" );
TST_STR_HLP( TST_PWR_OEN_STR_HLP,         "Enable Voltage Supply Output" );
TST_STR_CMD( TST_PWR_ODIS_STR_CMD,        "odis" );
TST_STR_HLP( TST_PWR_ODIS_STR_HLP,        "Disable Voltage Supply Output" );

TST_STR_OUT( TST_PWR_ERROR_STR,  "Error: Expecting %d parameters"NL );

/*==============================================================================
 *                            Private Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendStopReq( void )
{
    svc_PwrMonEh_StopReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON, SVC_PWRMONEH_STOP_REQ, sizeof(svc_PwrMonEh_StopReq_t) );
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendStartReq( uint8_t fmt )
{
    svc_PwrMonEh_StartReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON, SVC_PWRMONEH_START_REQ, sizeof(svc_PwrMonEh_StartReq_t) );
    reqPtr->smplCnt = 0;
    reqPtr->smplFmt = fmt;
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendCalReq( void )
{
    svc_PwrMonEh_CalReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON, SVC_PWRMONEH_CAL_REQ, sizeof(svc_PwrMonEh_CalReq_t) );
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrSupEh_buildAndSendVoltageSetReq( svc_PwrSupEh_VoltageMv_t mv )
{
    svc_PwrSupEh_VoltageSetReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON, SVC_PWRSUPEH_VOLTAGE_SET_REQ, sizeof(svc_PwrSupEh_VoltageSetReq_t) );
    reqPtr->mv = mv;
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrSupEh_buildAndSendOeSetReq( dev_PwrSupEh_OutputControl_t control )
{
    svc_PwrSupEh_OeSetReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON, SVC_PWRSUPEH_OE_SET_REQ, sizeof(svc_PwrSupEh_OeSetReq_t) );
    reqPtr->control = control;
    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static const char* tst_ch_name0 = "TEST_CHANNEL_0";
static const char* tst_ch_name1 = "TEST_CHANNEL_9";
static void
svc_PwrMonEh_buildAndSendConfigReq( void )
{
    svc_PwrMonEh_ConfigReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON,
                                  SVC_PWRMONEH_CONFIG_REQ,
                                  (sizeof(svc_PwrMonEh_ConfigReq_t) + (2 * sizeof(svc_PwrMonEh_ChEntry_t))) );

    reqPtr->numCh               = 2;
    reqPtr->chTable[0].chId     = 0;
    reqPtr->chTable[0].shuntVal = 10;
    strncpy( reqPtr->chTable[0].chName, tst_ch_name0, sizeof(reqPtr->chTable[0].chName) );
    reqPtr->chTable[1].chId     = 9;
    reqPtr->chTable[1].shuntVal = 100;
    strncpy( reqPtr->chTable[1].chName, tst_ch_name1, sizeof(reqPtr->chTable[1].chName) );

    svc_MsgFwk_msgSend( reqPtr );
    return;
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_config( int argc, char** argv )
{
    svc_PwrMonEh_buildAndSendConfigReq();
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_start( int argc, char** argv )
{
    uint8_t fmt;
    printf( "Power Monitor Starting"NL );
    if( argc < 1 )
    {
        printf( TST_PWR_ERROR_STR, 1 );
        return( TST_STATUS_ERROR );
    }

    fmt = (uint32_t)strtol(argv[0], NULL, 10);
    svc_PwrMonEh_buildAndSendStartReq( fmt );
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_stop( int argc, char** argv )
{
    printf( "Power Monitor Stopping"NL );
    svc_PwrMonEh_buildAndSendStopReq();
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_shortEn( int argc, char** argv )
{
    uint8_t chId;

    if( argc < 1 )
    {
        printf( TST_PWR_ERROR_STR, 1 );
        return( TST_STATUS_ERROR );
    }

    chId = (uint32_t)strtol(argv[0], NULL, 10);

    printf( "Power Monitor Shortting Channel %d"NL, chId );

    void dev_PwrMon_calEnable( dev_PwrMon_ChannelId_t channelId );
    dev_PwrMon_calEnable( chId );

    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_shortDis( int argc, char** argv )
{
    uint8_t chId;

    if( argc < 1 )
    {
        printf( TST_PWR_ERROR_STR, 1 );
        return( TST_STATUS_ERROR );
    }

    chId = (uint32_t)strtol(argv[0], NULL, 10);

    printf( "Power Monitor Un-Shorting Channel %d"NL, chId );

    void dev_PwrMon_calDisable( dev_PwrMon_ChannelId_t channelId );
    dev_PwrMon_calDisable( chId );

    return( TST_STATUS_OK );
}

/*============================================================================*/
svc_PwrMonEh_ChAvgInfo_t svc_PwrMon_channelAvgInfo[BSP_PLATFORM_PWRMON_NUM_CHANNELS];
static tst_Status_t
tst_Pwr_stats( int argc, char** argv )
{
    svc_PwrMonEh_ChannelStats_t* chStatsPtr = svc_PwrMon_channelStatsPtr();
    svc_PwrMonEh_SamplerStats_t* smplrStatsPtr = svc_PwrMon_samplerStatsPtr();

    printf( "Power Monitor Stats"NL );
    printf( "Channel:"NL );
    printf( "\tpktSndNum: %ld"NL, chStatsPtr->pktSndNum );
    printf( "\tpktErrNum: %ld"NL, chStatsPtr->pktErrNum );
    printf( "Sampler:"NL );
    printf( "\tsmplNum: %ld"NL, (uint32_t)smplrStatsPtr->smplNum );
    printf( "\tsmplErrNum: %ld"NL, smplrStatsPtr->smplErrNum );

    printf( "Channel Averages"NL );
    uint8_t numCh = svc_PwrMon_channelAvgGetAll(&svc_PwrMon_channelAvgInfo[0]);
    for( uint8_t i=0; i<numCh; i++ )
    {
        const char* name = svc_PwrMon_channelName( svc_PwrMon_channelAvgInfo[i].chId );
        printf( "\tCH%d: vBus: %ld mV   vShunt: %ld uV   iShunt: %ld uA  %s"NL,
                svc_PwrMon_channelAvgInfo[i].chId,
                svc_PwrMon_channelAvgInfo[i].mvBusAvg,
                svc_PwrMon_channelAvgInfo[i].uvShuntAvg,
                svc_PwrMon_channelAvgInfo[i].uaShuntAvg,
                name );
    }

    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_statsReset( int argc, char** argv )
{
    printf( "Power Monitor Stats Reset"NL );
    svc_PwrMon_channelAvgResetAll();
    return( TST_STATUS_OK );
}


/*============================================================================*/
static tst_Status_t
tst_Pwr_cal( int argc, char** argv )
{
    printf( "Power Monitor Calibration"NL );
    svc_PwrMonEh_buildAndSendCalReq();
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_vset( int argc, char** argv )
{
    if( argc < 1 )
    {
        printf( TST_PWR_ERROR_STR, 1 );
        return( TST_STATUS_ERROR );
    }

    uint16_t mv = (uint32_t)strtol(argv[0], NULL, 10);
    svc_PwrSupEh_buildAndSendVoltageSetReq(mv);
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_oen( int argc, char** argv )
{
    svc_PwrSupEh_buildAndSendOeSetReq(DEV_PWRSUPEH_OUTPUT_CONTROL_ENABLE);
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Pwr_odis( int argc, char** argv )
{
    svc_PwrSupEh_buildAndSendOeSetReq(DEV_PWRSUPEH_OUTPUT_CONTROL_DISABLE);
    return( TST_STATUS_OK );
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
// Helper macro to cleanup the table
#define TST_PWR_CMD( _uname, _lname ) TST_HANDLER_ELEMENT( TST_PWR_##_uname##_STR_CMD, \
                                                           TST_PWR_##_uname##_STR_HLP, \
                                                           tst_Pwr_##_lname )

/*============================================================================*/
const tst_TableElement_t tst_Pwr_menu[] =
{
    TST_PWR_CMD( CAL,         cal ),
    TST_PWR_CMD( CONFIG,      config ),
    TST_PWR_CMD( START,       start ),
    TST_PWR_CMD( STOP,        stop ),
    TST_PWR_CMD( STATS,       stats ),
    TST_PWR_CMD( STATS_RESET, statsReset ),
    TST_PWR_CMD( SHORT_EN,    shortEn ),
    TST_PWR_CMD( SHORT_DIS,   shortDis ),
    TST_PWR_CMD( VSET,        vset ),
    TST_PWR_CMD( OEN,         oen ),
    TST_PWR_CMD( ODIS,        odis ),
    TST_END_ELEMENT
};
#endif