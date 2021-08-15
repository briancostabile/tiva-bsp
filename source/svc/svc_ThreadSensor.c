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
 * @file svc_ThreadSensor.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_ThreadSensor.h"
#include "svc_TempEh.h"
#include "svc_HumidEh.h"
#include "svc_LightEh.h"
#include "svc_Eh.h"
#include "osapi.h"


#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"

#include <stdio.h>


/*==============================================================================
 *                                  Defines
 *============================================================================*/
#define SVC_THREADSENSOR_STACK_SIZE    2048
#define SVC_THREADSENSOR_STACK_SIZE_32 (SVC_THREADSENSOR_STACK_SIZE / 4)

#define SVC_THREADSENSOR_QUEUE_DEPTH 10

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// Total stack needed for the Peripheral thread
uint32_t svc_ThreadSensor_stack[SVC_THREADSENSOR_STACK_SIZE_32];
void*    svc_ThreadSensor_queue[SVC_THREADSENSOR_QUEUE_DEPTH];

/*============================================================================*/
static const svc_Eh_Info_t* svc_ThreadSensor_ehTable[] =
{
#if defined(SVC_EHID_TEMP)
    &svc_TempEh_info,
#endif
#if defined(SVC_EHID_HUMID)
    &svc_HumidEh_info,
#endif
#if defined(SVC_EHID_LIGHT)
    &svc_LightEh_info,
#endif
};

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_ThreadSensor_threadMain( osapi_ThreadArg_t arg )
{
    svc_Eh_listRun( DIM(svc_ThreadSensor_ehTable),
                    svc_ThreadSensor_ehTable,
                    SVC_THREADSENSOR_QUEUE_DEPTH,
                    svc_ThreadSensor_queue );
    return;
}

/*============================================================================*/
const osapi_ThreadInitInfo_t BSP_ATTR_USED BSP_ATTR_SECTION(".tinit") svc_ThreadSensor_threadInitInfo =
{
  .name        = "SENSOR",
  .handler     = svc_ThreadSensor_threadMain,
  .arg         = NULL,
  .priority    = 2,
  .stackSize32 = SVC_THREADSENSOR_STACK_SIZE_32,
  .stackPtr    = &svc_ThreadSensor_stack[0]
};
BSP_PRAGMA_DATA_REQUIRED(svc_ThreadSensor_threadInitInfo)
