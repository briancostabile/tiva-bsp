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
 * @file svc_ThreadUi.c
 * @brief
 */
#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "svc_ThreadUi.h"
#include "svc_CmdEh.h"
#include "svc_TestEh.h"
#include "svc_ButtonEh.h"
#include "svc_Eh.h"
#include "osapi.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"


/*==============================================================================
 *                                  Defines
 *============================================================================*/
#define SVC_THREADUI_STACK_SIZE    2048
#define SVC_THREADUI_STACK_SIZE_32 (SVC_THREADUI_STACK_SIZE / 4)

#define SVC_THREADUI_QUEUE_DEPTH 32

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// Total stack needed for the UI thread
uint32_t svc_ThreadUi_stack[SVC_THREADUI_STACK_SIZE_32];
void*    svc_ThreadUi_queue[SVC_THREADUI_QUEUE_DEPTH];


/*============================================================================*/
static const svc_Eh_Info_t* svc_ThreadUi_ehTable[] =
{
    &svc_CmdEh_info,
#if defined(SVC_EHID_TEST)
    &svc_TestEh_info,
#endif
#if defined(SVC_EHID_BUTTON)
    &svc_ButtonEh_info,
#endif
};

/*============================================================================*/
static void
svc_ThreadUi_threadMain( osapi_ThreadArg_t arg )
{
    svc_Eh_listRun( DIM(svc_ThreadUi_ehTable),
                    svc_ThreadUi_ehTable,
                    SVC_THREADUI_QUEUE_DEPTH,
                    svc_ThreadUi_queue );
}

/*============================================================================*/
const osapi_ThreadInitInfo_t BSP_ATTR_USED BSP_ATTR_SECTION(".tinit") svc_ThreadUi_threadInitInfo =
{
  .name        = "UI",
  .handler     = svc_ThreadUi_threadMain,
  .arg         = NULL,
  .priority    = 3,
  .stackSize32 = SVC_THREADUI_STACK_SIZE_32,
  .stackPtr    = &svc_ThreadUi_stack[0]
};
BSP_PRAGMA_DATA_REQUIRED(svc_ThreadUi_threadInitInfo)
