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
 * @file svc_Periph.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_Periph.h"
#include "svc_ButtonEh.h"
#include "svc_TestEh.h"
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
#define SVC_PERIPH_STACK_SIZE    2048
#define SVC_PERIPH_STACK_SIZE_32 (SVC_PERIPH_STACK_SIZE / 4)

#define SVC_PERIPH_QUEUE_DEPTH 10

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// Total stack needed for the Peripheral thread
uint32_t svc_Periph_stack[SVC_PERIPH_STACK_SIZE_32];
void*    svc_Periph_queue[SVC_PERIPH_QUEUE_DEPTH];

/*============================================================================*/
static const svc_Eh_Info_t* svc_Periph_ehTable[] = 
{
    &svc_ButtonEh_info,
    &svc_TempEh_info,
    &svc_HumidEh_info,
    &svc_LightEh_info,
    &svc_TestEh_info
};

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_Periph_threadMain( osapi_ThreadArg_t arg )
{
    svc_Eh_listRun( DIM(svc_Periph_ehTable),
                    svc_Periph_ehTable,
                    SVC_PERIPH_QUEUE_DEPTH,
                    svc_Periph_queue );
    return;
}

/*============================================================================*/
const osapi_ThreadInitInfo_t BSP_ATTR_USED BSP_ATTR_SECTION(".tinit") svc_Periph_threadInitInfo =
{
  .name        = "Periph",
  .handler     = svc_Periph_threadMain,
  .arg         = NULL,
  .priority    = 3,
  .stackSize32 = SVC_PERIPH_STACK_SIZE_32,
  .stackPtr    = &svc_Periph_stack[0]
};
BSP_PRAGMA_DATA_REQUIRED(svc_Periph_threadInitInfo)
