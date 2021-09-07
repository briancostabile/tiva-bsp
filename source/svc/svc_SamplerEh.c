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
#include "svc_SamplerEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"
#include "dev_PwrMon.h"
#include "svc_PwrMon_channel.h"
#include "svc_PwrMon_sampler.h"

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
#define SVC_PWRMON_STATE_SET( _state )                          \
{                                                               \
    svc_PwrMonEh_state = SVC_PWRMON_STATE_##_state;               \
    SVC_LOG_INFO( "[PwrMon] New State: %s"NL,                   \
                  svc_PwrMonEh_stateNames[svc_PwrMonEh_state] );    \
}

/*============================================================================*/
#define SVC_PWRMON_MSG_SND( _msgPtr )                                                \
{                                                                                    \
    SVC_LOG_INFO( "[PwrMon] Sending: %s"NL,                                          \
                  svc_PwrMonEh_msgNames[SVC_MSGFWK_MSG_ID_NUM_GET(_msgPtr->hdr.id)] ); \
    svc_MsgFwk_msgSend( cnfPtr );                                                    \
}

/*==============================================================================
 *                                Types
 *============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_State_t svc_PwrMonEh_state;

const char* svc_PwrMonEh_stateNames[] =
{
    "Init",
    "Configured",
    "Calibration",
    "Sampling"
};

const char* svc_PwrMonEh_msgNames[] = SVC_PWRMONEH_MSG_ID_NAMES_TABLE;

int svc_PwrMonEh_usbFd;
bool_t svc_PwrMonEh_usbConnected;

svc_PwrMon_SamplerSampleSize_t svc_PwrMonEh_sampleSize;

int32_t                     svc_PwrMonEh_pktLenRemain;
dev_PwrMon_SamplerPktSent_t svc_PwrMonEh_pktSentCallback;
void*                       svc_PwrMonEh_pktSentCallbackData;

svc_PwrMon_SamplerStats_t* svc_PwrMonEh_samplerStatsPtr;
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
svc_PwrMonEh_processConfigReq( svc_PwrMonEh_ConfigReq_t* configReqPtr )
{
    for( int i=0; i<configReqPtr->numCh; i++ )
    {
        svc_PwrMon_channelConfig( configReqPtr->chTable[i].chId,
                                  configReqPtr->chTable[i].shuntVal,
                                  configReqPtr->chTable[i].chName );
    }

    svc_PwrMonEh_sampleSize = (configReqPtr->smplFmt == SVC_PWRMONEH_SMPL_FMT_24BIT) ?
        SVC_PWRMON_SAMPLER_SAMPLE_SIZE_24BIT : SVC_PWRMON_SAMPLER_SAMPLE_SIZE_32BIT;

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
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_sampleSize );
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
        case SVC_PWRMONEH_PKT_READY_IND:
        {
            svc_PwrMonEh_PktReadyInd_t* pktReadyIndPtr = (svc_PwrMonEh_PktReadyInd_t*)msgPtr;
            svc_PwrMonEh_pktLenRemain        = pktReadyIndPtr->pktLen;
            svc_PwrMonEh_pktSentCallback     = pktReadyIndPtr->callback;
            svc_PwrMonEh_pktSentCallbackData = pktReadyIndPtr->cbData;

            svc_PwrMon_SamplerPacket_t* pktPtr = pktReadyIndPtr->pktPtr;
            for( int smplSet = 0; smplSet < pktPtr->hdr.numSmplSet; smplSet++ )
            {
                for( int ch = 0; ch < pktPtr->hdr.numCh; ch++ )
                {
                    svc_PwrMon_channelUpdate( ch,
                        pktPtr->data[(smplSet * pktPtr->hdr.numCh) + ch].v,
                        pktPtr->data[(smplSet * pktPtr->hdr.numCh) + ch].i );
                }
            }

            bsp_UsbBulk_write( svc_PwrMonEh_usbFd,
                               (uint8_t*)pktReadyIndPtr->pktPtr,
                               pktReadyIndPtr->pktLen );

            bsp_Led_setColor( BSP_PLATFORM_LED_ID_STATUS,
                             (BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_G | BSP_LED_COLOR_MASK_B) *
                              ((svc_PwrMonEh_samplerStatsPtr->pktSndNum >> 6) & 0x01) );
        }
        break;
        case SVC_PWRMONEH_CONFIG_REQ:
        {
            // Stop sampling, update config and restart sampling
            svc_PwrMon_samplerStop();
            svc_PwrMonEh_processConfigReq( (svc_PwrMonEh_ConfigReq_t*)msgPtr );
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_sampleSize );
            svc_PwrMonEh_buildAndSendConfigCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
        }
        break;
        case SVC_PWRMONEH_START_REQ:
        {
            // Stop sampling, and restart sampling
            svc_PwrMon_samplerStop();
            svc_PwrMonEh_buildAndSendStartCnf( msgPtr->eh, SVC_PWRMONEH_STATUS_SUCCESS );
            svc_PwrMon_samplerStart( svc_PwrMon_channelBitmapGet(), svc_PwrMonEh_sampleSize );
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
    if( msgPtr->id != SVC_PWRMONEH_PKT_READY_IND )
    {
        SVC_LOG_INFO( "[PwrMon] Received %s:0x%04X in %s State"NL,
                    svc_PwrMonEh_msgNames[SVC_MSGFWK_MSG_ID_NUM_GET(msgPtr->id)],
                    msgPtr->id,
                    svc_PwrMonEh_stateNames[svc_PwrMonEh_state] );
    }

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

    if( msgPtr->id != SVC_PWRMONEH_PKT_READY_IND )
    {
        SVC_LOG_INFO( "[PwrMon] New State: %s"NL,
                    svc_PwrMonEh_stateNames[svc_PwrMonEh_state] );
    }

    return;
}


/*========================
 *   Callbacks
 *=======================*/
/*============================================================================*/
static void
svc_PwrMonEh_usbConnectionCallback( bool connected )
{
    SVC_LOG_INFO( "[PwrMon]USB Connected: %d"NL, connected );
    svc_PwrMonEh_usbConnected = connected;
    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_usbTxDoneCallback( size_t cnt )
{
    svc_PwrMonEh_pktLenRemain -= cnt;
    if( svc_PwrMonEh_pktLenRemain <= 0 )
    {
        svc_PwrMonEh_pktSentCallback( svc_PwrMonEh_pktSentCallbackData );
    }
    return;
}

/*============================================================================*/
static void
svc_PwrMon_pktReady( void*                       pktPtr,
                     size_t                      pktLen,
                     dev_PwrMon_SamplerPktSent_t callback,
                     void*                       cbData )
{
    if( svc_PwrMonEh_usbConnected )
    {
        svc_PwrMonEh_PktReadyInd_t* indPtr = svc_MsgFwk_msgAlloc( SVC_EHID_PWRMON,
                                                                  SVC_PWRMONEH_PKT_READY_IND,
                                                                  sizeof(svc_PwrMonEh_PktReadyInd_t) );
        indPtr->pktPtr   = pktPtr;
        indPtr->pktLen   = pktLen;
        indPtr->callback = callback;
        indPtr->cbData   = cbData;
        svc_MsgFwk_msgSend( indPtr );
    }

    return;
}

/*============================================================================*/
static void
svc_PwrMonEh_init( void )
{
    SVC_PWRMON_STATE_SET( INIT );
    dev_PwrMon_init();
    svc_PwrMonEh_usbFd = bsp_UsbBulk_open();
    bsp_UsbBulk_registerCallbackConnection( svc_PwrMonEh_usbFd, svc_PwrMonEh_usbConnectionCallback );
    bsp_UsbBulk_registerCallbackTxDone( svc_PwrMonEh_usbFd, svc_PwrMonEh_usbTxDoneCallback );

    svc_PwrMonEh_pktLenRemain        = 0;
    svc_PwrMonEh_pktSentCallback     = NULL;
    svc_PwrMonEh_pktSentCallbackData = NULL;

    svc_PwrMon_channelInit();
    svc_PwrMon_samplerInit( svc_PwrMon_pktReady );

    svc_PwrMonEh_samplerStatsPtr = svc_PwrMon_samplerStatsPtr();
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_PwrMonEh_info =
{
    SVC_EHID_PWRMON,
    0,    // bcastListLen
    NULL, // bcastList
    svc_PwrMonEh_init,
    svc_PwrMonEh_handler  // msgHandler
};
#endif
