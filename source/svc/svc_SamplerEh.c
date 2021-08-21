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
 * @file svc_SamplerEh.c
 * @brief Contains the Event handler for Service layer Sampler messages
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "svc_SamplerEh.h"
#include "svc_MsgFwk.h"
#include "osapi.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_NONE
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_SAMPLER)
/*==============================================================================
 *                                Defines
 *============================================================================*/

/*==============================================================================
 *                                Types
 *============================================================================*/

/*==============================================================================
 *                                Globals
 *============================================================================*/

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
static void
svc_SamplerEh_msgHandler( svc_MsgFwk_Hdr_t* msgPtr )
{
    return;
}

/*============================================================================*/
static void
svc_SamplerEh_init( void )
{

}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_HumidEh_info =
{
    SVC_EHID_SAMPLER,
    0,    // bcastListLen
    NULL, // bcastList
    svc_SamplerEh_init,
    svc_SamplerEh_msgHandler  // msgHandler
};
#endif
