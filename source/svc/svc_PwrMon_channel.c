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
 * @file svc_PwrMon.c
 * @brief Contains channel object and APIs
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "bsp_Assert.h"
#include "svc_EhId.h"
#include "svc_PwrMonEh.h"
#include "svc_PwrMon_channel.h"
#include "dev_PwrMon.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/

/*==============================================================================
 *                                Types
 *============================================================================*/
typedef struct svc_PwrMon_ChannelInfo_s
{
    dev_PwrMon_ChannelId_t chId;
    dev_PwrMon_ShuntVal_t  shuntVal;
    char                   name[SVC_PWRMONEH_CH_NAME_LEN+1];
    uint32_t               smplCnt;
    uint32_t               avgCnt;
    int64_t                sumV;
    int64_t                sumI;
} svc_PwrMon_ChannelInfo_t;

svc_PwrMonEh_ChBitmap_t svc_PwrMon_channelBitmap;

/*==============================================================================
 *                                Globals
 *============================================================================*/

svc_PwrMon_ChannelInfo_t svc_PwrMon_channelInfo[BSP_PLATFORM_PWRMON_NUM_CHANNELS];

/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_channelInit( void )
{
    memset( svc_PwrMon_channelInfo, 0, sizeof(svc_PwrMon_channelInfo) );
    svc_PwrMon_channelBitmap = 0x00000000;
    return;
}

/*============================================================================*/
void svc_PwrMon_channelConfig( dev_PwrMon_ChannelId_t chId,
                               dev_PwrMon_ShuntVal_t  shuntVal,
                               char*                  name )
{
    BSP_ASSERT( chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS );

    strncpy( svc_PwrMon_channelInfo[chId].name, name, SVC_PWRMONEH_CH_NAME_LEN );
    svc_PwrMon_channelInfo[chId].name[SVC_PWRMONEH_CH_NAME_LEN] = 0;

    svc_PwrMon_channelInfo[chId].chId     = chId;
    svc_PwrMon_channelInfo[chId].shuntVal = shuntVal;
    svc_PwrMon_channelInfo[chId].smplCnt  = 0;
    svc_PwrMon_channelInfo[chId].avgCnt   = 0;
    svc_PwrMon_channelInfo[chId].sumV     = 0;
    svc_PwrMon_channelInfo[chId].sumI     = 0;

    svc_PwrMon_channelBitmap |= (1 << chId);

    dev_PwrMon_channelConfigShunt( chId, shuntVal, NULL, NULL );

    return;
}

/*============================================================================*/
void svc_PwrMon_channelUpdate( dev_PwrMon_ChannelId_t chId,
                               dev_PwrMon_Data_t      voltage,
                               dev_PwrMon_Data_t      current )
{
    svc_PwrMon_channelInfo[chId].smplCnt++;
    svc_PwrMon_channelInfo[chId].avgCnt++;
    svc_PwrMon_channelInfo[chId].sumV += (int16_t)voltage;
    svc_PwrMon_channelInfo[chId].sumI += (int16_t)current;
    return;
}

/*============================================================================*/
void svc_PwrMon_channelAvgReset( dev_PwrMon_ChannelId_t chId )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    svc_PwrMon_channelInfo[chId].avgCnt = 0;
    svc_PwrMon_channelInfo[chId].sumV   = 0;
    svc_PwrMon_channelInfo[chId].sumI   = 0;
    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_channelAvgResetAll( void )
{
    uint8_t chId = 0;
    svc_PwrMonEh_ChBitmap_t bitmap = svc_PwrMon_channelBitmap;
    while( bitmap )
    {
        if( bitmap & 1 )
        {
            svc_PwrMon_channelAvgReset( chId );
        }
        bitmap >>= 1;
        chId++;
    }
    return;
}

/*============================================================================*/
void svc_PwrMon_channelAvgGet( dev_PwrMon_ChannelId_t chId,
                               dev_PwrMon_Data_t*     vPtr,
                               dev_PwrMon_Data_t*     iPtr )
{
    int32_t sumV;
    int32_t sumI;
    uint32_t cnt;

    BSP_MCU_CRITICAL_SECTION_ENTER();
    cnt  = svc_PwrMon_channelInfo[chId].avgCnt;
    sumV = svc_PwrMon_channelInfo[chId].sumV;
    sumI = svc_PwrMon_channelInfo[chId].sumI;
    BSP_MCU_CRITICAL_SECTION_EXIT();

    *vPtr = (sumV / cnt);
    *iPtr = (sumI / cnt);

    return;
}


/*============================================================================*/
void svc_PwrMon_channelAvgGetAll( svc_PwrMonEh_ChAvgInfo_t* dataBuffer )
{
    uint8_t chId = 0;
    uint8_t bufIdx = 0;
    svc_PwrMonEh_ChBitmap_t bitmap = svc_PwrMon_channelBitmap;
    while( bitmap )
    {
        if( bitmap & 1 )
        {
            // Use temporary variables because SmplData is not aligned
            dev_PwrMon_Data_t v;
            dev_PwrMon_Data_t i;
            svc_PwrMon_channelAvgGet( chId, &v, &i );
            dataBuffer[ bufIdx ].chId = chId;
            dataBuffer[ bufIdx ].avg.fmt0.v = v;
            dataBuffer[ bufIdx ].avg.fmt0.i = i;
            bufIdx++;
        }
        bitmap >>= 1;
        chId++;
    }

    return;
}

/*============================================================================*/
svc_PwrMonEh_ChBitmap_t svc_PwrMon_channelBitmapGet( void )
{
    return( svc_PwrMon_channelBitmap );
}

/*============================================================================*/
uint8_t svc_PwrMon_channelCntGet( void )
{
    uint8_t cnt = 0;
    svc_PwrMonEh_ChBitmap_t bitmap = svc_PwrMon_channelBitmap;
    while( bitmap )
    {
        cnt += (bitmap & 1);
        bitmap >>= 1;
    }
    return( cnt );
}

#endif
