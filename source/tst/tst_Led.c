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
 * @file tst_Led.c
 * @brief Test functions to set LEDs
 */

#include "bsp_Types.h"
#include "tst.h"
#include "tst_Led.h"
#include "svc_LedEh.h"
#include "bsp_TimerGp.h"
#include "osapi.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#if defined(BSP_PLATFORM_ENABLE_DEV_PWRMON_INA226)
#include "dev_PwrMon.h"
#endif

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// String globals
TST_STR_CMD( TST_LED_TEST_STR_CMD, "test" );
TST_STR_HLP( TST_LED_TEST_STR_HLP, "Run a predefined test pattern on LED" );
TST_STR_CMD( TST_LED_TEST2_STR_CMD, "test2" );
TST_STR_HLP( TST_LED_TEST2_STR_HLP, "Run a test" );
TST_STR_CMD( TST_LED_SET_COLOR_STR_CMD, "set_color" );
TST_STR_HLP( TST_LED_SET_COLOR_STR_HLP, "Set color of selected LED" );

TST_STR_OUT( TST_LED_ERROR_STR,  "Error: Expecting %d parameters"NL );


/*==============================================================================
 *                            Private Functions
 *============================================================================*/
static const svc_LedEh_PatternElement_t tst_Led_testPattern[] =
{
    { BSP_LED_COLOR_MASK_R, 1000 },
    { BSP_LED_COLOR_MASK_G, 1000 },
    { BSP_LED_COLOR_MASK_B, 1000 },
    { BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_G, 1000 },
    { BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_B, 1000 },
    { BSP_LED_COLOR_MASK_G | BSP_LED_COLOR_MASK_B, 1000 },
    { BSP_LED_COLOR_MASK_R | BSP_LED_COLOR_MASK_G | BSP_LED_COLOR_MASK_B, 1000 },
    { 0, 1000 },
};

/*============================================================================*/
static tst_Status_t
tst_Led_test( int argc, char** argv )
{
    for( int i=0; i<BSP_PLATFORM_LED_NUM; i++ )
    {
        svc_LedEh_buildAndSendSetPatternReq( SVC_EHID_LED, i,
                                             DIM( tst_Led_testPattern ),
                                             (svc_LedEh_PatternElement_t*)tst_Led_testPattern );
    }
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Led_set_color( int argc, char** argv )
{
    uint8_t id;
    bsp_Led_Color_t color;

    if( argc < 2 )
    {
        printf( TST_LED_ERROR_STR, 2 );
        return( TST_STATUS_ERROR );
    }

    id = (uint32_t)strtol(argv[0], NULL, 10);
    color = (uint32_t)strtol(argv[1], NULL, 16);

    svc_LedEh_buildAndSendSetColorReq( SVC_EHID_LED, id, color );
    return( TST_STATUS_OK );
}

/*============================================================================*/

#if defined(SVC_EHID_PWRMON)
#include "svc_PwrMonEh.h"
/*============================================================================*/
static void
tst_Led_buildAndSendSetPwrMonConfigReq( void )
{
    svc_PwrMonEh_ConfigReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_TEST,
                                  SVC_PWRMONEH_CONFIG_REQ,
                                  sizeof(svc_PwrMonEh_ConfigReq_t) + (2 * sizeof(svc_PwrMonEh_ChEntry_t)) );
    reqPtr->smplFmt = SVC_PWRMONEH_SMPL_FMT_32BIT;
    reqPtr->numCh = 2;
    reqPtr->chTable[0].chId = 0;
    reqPtr->chTable[0].shuntVal = 10;
    strncpy(reqPtr->chTable[0].chName, "TEST_POWER_RAIL0", sizeof(reqPtr->chTable[0].chName) );
    reqPtr->chTable[1].chId = 1;
    reqPtr->chTable[1].shuntVal = 12;
    strncpy(reqPtr->chTable[0].chName, "TEST_POWER_RAIL1", sizeof(reqPtr->chTable[0].chName) );
    svc_MsgFwk_msgSend( reqPtr );
    return;
}


/*============================================================================*/
static void
tst_Led_buildAndSendSetPwrMonStartReq( void )
{
    svc_PwrMonEh_StartReq_t* reqPtr;
    reqPtr = svc_MsgFwk_msgAlloc( SVC_EHID_TEST,
                                  SVC_PWRMONEH_START_REQ,
                                  sizeof(svc_PwrMonEh_StartReq_t) );
    svc_MsgFwk_msgSend( reqPtr );
    return;
}


/*============================================================================*/
static tst_Status_t
tst_Led_test2( int argc, char** argv )
{
    tst_Led_buildAndSendSetPwrMonConfigReq();
    tst_Led_buildAndSendSetPwrMonStartReq();
    return( TST_STATUS_OK );
}
#else
static tst_Status_t
tst_Led_test2( int argc, char** argv )
{
    return( TST_STATUS_OK );
}
#endif
/*==============================================================================
 *                            Public Functions
 *============================================================================*/
// Helper macro to cleanup the table
#define TST_LED_CMD( _uname, _lname ) TST_HANDLER_ELEMENT( TST_LED_##_uname##_STR_CMD, \
                                                           TST_LED_##_uname##_STR_HLP, \
                                                           tst_Led_##_lname )

/*============================================================================*/
const tst_TableElement_t tst_Led_menu[] =
{
    TST_LED_CMD( TEST, test ),
    TST_LED_CMD( TEST2, test2 ),
    TST_LED_CMD( SET_COLOR, set_color ),
    TST_END_ELEMENT
};
