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
 * @file svc_PwrMonEh.c
 * @brief Contains the Event handler for Service layer Power Monitor messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "bsp_UsbBulk.h"
#include "bsp_Led.h"
#include "dev_PwrMon.h"
#include "osapi.h"
#include "svc_MsgFwk.h"
#include "svc_PwrMonEh.h"
#include "svc_PwrMon_sampler.h"
#include "svc_PwrMon_channel.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
typedef enum svc_PwrMon_State_e {
    SVC_PWRMON_STATE_INIT        = 0,
    SVC_PWRMON_STATE_CONFIGURED  = 1,
    SVC_PWRMON_STATE_CALIBRATION = 2,
    SVC_PWRMON_STATE_SAMPLING    = 3
} svc_PwrMon_State_t;

/*============================================================================*/
#define SVC_PWRMON_STATE_SET( _state )                           \
{                                                                \
    svc_PwrMonEh_state = SVC_PWRMON_STATE_##_state;              \
    SVC_LOG_INFO( "[PwrMon] New State: %s"NL,                    \
                  svc_PwrMonEh_stateNames[svc_PwrMonEh_state] ); \
}

/*============================================================================*/
#define SVC_PWRMON_MSG_SND( _msgPtr )                                                  \
{                                                                                      \
    SVC_LOG_INFO( "[PwrMon] Sending: %s"NL,                                            \
                  svc_PwrMonEh_msgNames[SVC_MSGFWK_MSG_ID_NUM_GET(_msgPtr->hdr.id)] ); \
    svc_MsgFwk_msgSend( cnfPtr );                                                      \
}

/*==============================================================================
 *                                Types
 *============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
const svc_MsgFwk_MsgId_t svc_PwrMonEh_bcastMsgIds[] =
{
    SVC_PWRMONEH_DATA_IND
};

const char* svc_PwrMonEh_stateNames[] =
{
    "Init",
    "Configured",
    "Calibration",
    "Sampling"
};

const char* svc_PwrMonEh_msgNames[] = SVC_PWRMONEH_MSG_ID_NAMES_TABLE;

svc_PwrMonEh_SmplFmt_t svc_PwrMonEh_smplFmt;
svc_PwrMonEh_SamplerStats_t* svc_PwrMonEh_samplerStatsPtr;
svc_PwrMon_State_t svc_PwrMonEh_state;


/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendConfigCnf( svc_EhId_t            eh,
                                    svc_PwrMonEh_Status_t status )
{
    svc_PwrMonEh_ConfigCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_PWRMONEH_CONFIG_CNF, sizeof(svc_PwrMonEh_ConfigCnf_t) );
    cnfPtr->status = status;
    SVC_PWRMON_MSG_SND( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendStartCnf( svc_EhId_t            eh,
                                   svc_PwrMonEh_Status_t status )
{
    svc_PwrMonEh_StartCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_PWRMONEH_START_CNF, sizeof(svc_PwrMonEh_StartCnf_t) );
    cnfPtr->status = status;
    SVC_PWRMON_MSG_SND( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendStopCnf( svc_EhId_t            eh,
                                  svc_PwrMonEh_Status_t status )
{
    svc_PwrMonEh_StopCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_PWRMONEH_STOP_CNF, sizeof(svc_PwrMonEh_StopCnf_t) );
    cnfPtr->status = status;
    SVC_PWRMON_MSG_SND( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendStatsCnf( svc_EhId_t eh )
{
    svc_PwrMonEh_StatsCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_PWRMONEH_STATS_CNF, sizeof(svc_PwrMonEh_StatsCnf_t) );
    BSP_MCU_CRITICAL_SECTION_ENTER();
    memcpy( &cnfPtr->samplerStats, svc_PwrMonEh_samplerStatsPtr, sizeof(cnfPtr->samplerStats) );
    BSP_MCU_CRITICAL_SECTION_EXIT();
    SVC_PWRMON_MSG_SND( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_buildAndSendChAvgCnf( svc_EhId_t eh )
{
    uint8_t chCnt = svc_PwrMon_channelCntGet();
    size_t size = (sizeof(svc_PwrMonEh_ChAvgCnf_t) + (chCnt * sizeof(svc_PwrMonEh_ChAvgInfo_t)));
    SVC_LOG_INFO( "[PwrMon] svc_PwrMon_channelCntGet :%d size:%d"NL, chCnt, size );

    svc_PwrMonEh_ChAvgCnf_t* cnfPtr;
    cnfPtr = svc_MsgFwk_msgAlloc( eh, SVC_PWRMONEH_CH_AVG_CNF, size );
    cnfPtr->chCnt = chCnt;
    svc_PwrMon_channelAvgGetAll( cnfPtr->avgArray );
    SVC_PWRMON_MSG_SND( cnfPtr );
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_processConfigReq( svc_PwrMonEh_ConfigReq_t* configReqPtr )
{
    for( int i=0; i<configReqPtr->numCh; i++ )
    {
        svc_PwrMon_channelConfig( configReqPtr->chTable[i].chId,
                                  configReqPtr->chTable[i].shuntVal,
                                  configReqPtr->chTable[i].chName );
    }
    svc_PwrMonEh_smplFmt = configReqPtr->smplFmt;
    return;
}


/*========================
 *   Handlers
 *=======================*/
/*============================================================================*/
static void
svc_PwrMonEh_handlerInit( svc_MsgFwk_Hdr_t* msgPtr )
{
    switch( msgPtr->id )
    {
        case SVC_PWRMONEH_CONFIG_REQ:
        {
            svc_PwrMonEh_processConfigReq((svc_PwrMonEh_ConfigReq_t*)msgPtr);
            svc_PwrMonEh_buildAndSendConfigCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            SVC_PWRMON_STATE_SET( CONFIGURED );
        }
        break;
        case SVC_PWRMONEH_START_REQ:
        {
            svc_PwrMonEh_buildAndSendStartCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_FAILURE );
        }
        break;
        case SVC_PWRMONEH_STOP_REQ:
        {
            svc_PwrMonEh_buildAndSendStopCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_FAILURE );
        }
        break;

        default:
            SVC_LOG_INFO( "[PwrMon] Unhandled Message"NL );

    }
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_handlerConfigured( svc_MsgFwk_Hdr_t* msgPtr )
{
    switch( msgPtr->id )
    {
        case SVC_PWRMONEH_START_REQ:
        {
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_smplFmt );
            svc_PwrMonEh_buildAndSendStartCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            SVC_PWRMON_STATE_SET( SAMPLING );
        }
        break;

        case SVC_PWRMONEH_CONFIG_REQ:
        {
            svc_PwrMonEh_processConfigReq( (svc_PwrMonEh_ConfigReq_t*)msgPtr );
            svc_PwrMonEh_buildAndSendConfigCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            SVC_PWRMON_STATE_SET( CONFIGURED );
        }
        break;
        case SVC_PWRMONEH_STOP_REQ:
        {
            svc_PwrMonEh_buildAndSendStopCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
        }
        break;
        default:
            SVC_LOG_INFO( "[PwrMon] Unhandled Message"NL );

    }
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_handlerCalibration( svc_MsgFwk_Hdr_t* msgPtr )
{
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_handlerSampling( svc_MsgFwk_Hdr_t* msgPtr )
{
    switch( msgPtr->id )
    {
        case SVC_PWRMONEH_DATA_IND:
        {
            BSP_TRACE_PWRMONEH_DATA_IND_ENTER();
            svc_PwrMonEh_DataInd_t* dataPtr = (svc_PwrMonEh_DataInd_t*)msgPtr;
            for( int smplSet = 0; smplSet < dataPtr->numSmplSet; smplSet++ )
            {
                for( int ch = 0; ch < dataPtr->numCh; ch++ )
                {
                    switch( dataPtr->smplFmt )
                    {
                        case SVC_PWRMONEH_SMPL_FMT0_IV:
                        {
                            svc_PwrMon_channelUpdate( ch,
                                dataPtr->data[(smplSet * dataPtr->numCh) + ch].fmt0.v,
                                dataPtr->data[(smplSet * dataPtr->numCh) + ch].fmt0.i );
                        }
                    }
                }
            }

            bsp_Led_setColor( BSP_PLATFORM_LED_ID_STATUS,
                             (BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_G | BSP_LED_COLOR_MASK_B) *
                              ((svc_PwrMonEh_samplerStatsPtr->pktSndNum >> 6) & 0x01) );
            BSP_TRACE_PWRMONEH_DATA_IND_EXIT();
        }
        break;
        case SVC_PWRMONEH_CONFIG_REQ:
        {
            // Stop sampling, update config and restart sampling
            svc_PwrMon_samplerStop();
            svc_PwrMonEh_processConfigReq( (svc_PwrMonEh_ConfigReq_t*)msgPtr );
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_smplFmt );
            svc_PwrMonEh_buildAndSendConfigCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
        }
        break;
        case SVC_PWRMONEH_START_REQ:
        {
            // Stop sampling, and restart sampling
            svc_PwrMon_samplerStop();
            svc_PwrMonEh_buildAndSendStartCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_smplFmt );
        }
        break;
        case SVC_PWRMONEH_STOP_REQ:
        {
            // Stop sampling, and drop to Configured state
            svc_PwrMon_samplerStop();
            svc_PwrMonEh_buildAndSendStopCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            SVC_PWRMON_STATE_SET( CONFIGURED );
        }
        break;
        default:
            SVC_LOG_INFO( "[PwrMon] Unhandled Message"NL );
    }
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_handler( svc_MsgFwk_Hdr_t* msgPtr )
{
    if( msgPtr->id != SVC_PWRMONEH_DATA_IND )
    {
        SVC_LOG_INFO( "[PwrMon] Received %s:0x%04X in %s State"NL,
                    svc_PwrMonEh_msgNames[SVC_MSGFWK_MSG_ID_NUM_GET(msgPtr->id)],
                    msgPtr->id,
                    svc_PwrMonEh_stateNames[svc_PwrMonEh_state] );
    }

    // messages handled in all states
    switch( msgPtr->id )
    {
        case SVC_PWRMONEH_STATS_REQ:
        {
            svc_PwrMonEh_StatsReq_t* reqPtr = (svc_PwrMonEh_StatsReq_t*)msgPtr;
            if( reqPtr->reset )
            {
                svc_PwrMon_samplerStatsReset();
            }
            svc_PwrMonEh_buildAndSendStatsCnf( msgPtr->eh );
        }
        break;
        case SVC_PWRMONEH_CH_AVG_REQ:
        {
            svc_PwrMonEh_ChAvgReq_t* reqPtr = (svc_PwrMonEh_ChAvgReq_t*)msgPtr;
            if( reqPtr->reset )
            {
                svc_PwrMon_channelAvgResetAll();
            }
            svc_PwrMonEh_buildAndSendChAvgCnf( msgPtr->eh );
        }
        break;
        default:
        {
            switch( svc_PwrMonEh_state )
            {
                case SVC_PWRMON_STATE_INIT:
                    svc_PwrMonEh_handlerInit( msgPtr );
                break;
                case SVC_PWRMON_STATE_CONFIGURED:
                    svc_PwrMonEh_handlerConfigured( msgPtr );
                break;
                case SVC_PWRMON_STATE_CALIBRATION:
                    svc_PwrMonEh_handlerCalibration( msgPtr );
                break;
                case SVC_PWRMON_STATE_SAMPLING:
                    svc_PwrMonEh_handlerSampling( msgPtr );
                break;
                default:
                    break;
            }
        }
    }

    if( msgPtr->id != SVC_PWRMONEH_DATA_IND )
    {
        SVC_LOG_INFO( "[PwrMon] New State: %s"NL,
                    svc_PwrMonEh_stateNames[svc_PwrMonEh_state] );
    }

    return;
}


/*============================================================================*/
static void
svc_PwrMonEh_init( void )
{
    SVC_PWRMON_STATE_SET( INIT );
    dev_PwrMon_init();
    svc_PwrMon_channelInit();
    svc_PwrMon_samplerInit();

    svc_PwrMonEh_samplerStatsPtr = svc_PwrMon_samplerStatsPtr();
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_PwrMonEh_info =
{
    SVC_EHID_PWRMON,
    sizeof(svc_PwrMonEh_bcastMsgIds),
    svc_PwrMonEh_bcastMsgIds,
    svc_PwrMonEh_init,
    svc_PwrMonEh_handler  // msgHandler
};
#endif
