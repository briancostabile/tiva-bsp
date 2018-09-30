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
 * @file osapi_FreeRTOS.c
 * @brief Contains necessary configuration and adaptation of FreeRTOS to the
 *        platform.
 */

#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Mcu.h"

#include "osapi.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*==============================================================================
 *                                  Defines
 *============================================================================*/
// Service, UI
#define OSAPI_THREAD_MAX_CNT 4
#define OSAPI_SEMAPHORE_MAX_CNT 4
#define OSAPI_MS_PER_TICK 1

/*==============================================================================
 *                                 Types
 *============================================================================*/

/*==============================================================================
 *                                 Globals
 *============================================================================*/
StaticTask_t osapi_threadControlBlockArray[OSAPI_THREAD_MAX_CNT + 2];
uint8_t osapi_threadControlBlockIdx;

StaticSemaphore_t osapi_semaphoreControlBlockArray[OSAPI_SEMAPHORE_MAX_CNT];
uint8_t osapi_semaphoreControlBlockIdx;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
static StaticTask_t* osapi_threadCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_threadControlBlockIdx < OSAPI_THREAD_MAX_CNT );
    StaticTask_t *tcbPtr = &osapi_threadControlBlockArray[ osapi_threadControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( tcbPtr );
}

static StaticSemaphore_t* osapi_semaphoreCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_semaphoreControlBlockIdx < OSAPI_SEMAPHORE_MAX_CNT );
    StaticSemaphore_t *semPtr = &osapi_semaphoreControlBlockArray[ osapi_semaphoreControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( semPtr );
}

/*============================================================================*/
void
osapi_Semaohire_Init( void )
{
    osapi_semaphoreControlBlockIdx = 0;
    memset( osapi_semaphoreControlBlockArray, 0, sizeof(osapi_semaphoreControlBlockArray) );
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
osapi_init( void )
{
    extern uint32_t _tinitStart;
    extern uint32_t _tinitEnd;

    osapi_Semaohire_Init();

    osapi_ThreadInitInfo_t* tinitPtr = (osapi_ThreadInitInfo_t*)&_tinitStart;

    while( tinitPtr < (osapi_ThreadInitInfo_t*)&_tinitEnd )
    {
        osapi_Thread_create( tinitPtr );
        tinitPtr++;
    }
    return;
}


/*============================================================================*/
void
osapi_Scheduler_run( void )
{
    vTaskStartScheduler();
    return;
}


/*============================================================================*/
void
osapi_Thread_sleep( uint32_t sleepTimeMs )
{
    uint32_t sleepTicks = (sleepTimeMs + (OSAPI_MS_PER_TICK - 1)) / OSAPI_MS_PER_TICK;
    vTaskDelay(sleepTicks);
}


/*============================================================================*/
void
osapi_Thread_create( osapi_ThreadInitInfo_t* tInitPtr )
{
    xTaskCreateStatic( tInitPtr->handler,
                       tInitPtr->name,
                       tInitPtr->stackSize32,
                       tInitPtr->arg,
                       tInitPtr->priority,
                       tInitPtr->stackPtr,
                       osapi_threadCtrlBlockAlloc() );
    return;
}


/*============================================================================*/
osapi_Semaphore_t
osapi_Semaphore_create( void )
{
    return( (osapi_Semaphore_t)xSemaphoreCreateBinaryStatic( osapi_semaphoreCtrlBlockAlloc() ) );
}


/*============================================================================*/
void
osapi_Semaphore_give( osapi_Semaphore_t sem )
{
    BaseType_t pxHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR( (SemaphoreHandle_t)sem, &pxHigherPriorityTaskWoken );
    return;
}


/*============================================================================*/
bool
osapi_Semaphore_take( osapi_Semaphore_t sem, osapi_Timeout_t timeout )
{
    return ( xSemaphoreTake( (SemaphoreHandle_t)sem, (TickType_t)timeout ) == pdPASS );
}


/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}


/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}


/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}


/*-----------------------------------------------------------*/
/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = osapi_threadCtrlBlockAlloc();

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


/*-----------------------------------------------------------*/
/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = osapi_threadCtrlBlockAlloc();

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
    static uint32_t osapi_timerTicks = 0;
    osapi_timerTicks++;
}


/*-----------------------------------------------------------*/
/* Catch asserts so the file and line number of the assert can be viewed. */
void vMainAssertCalled( const char *pcFileName, uint32_t ulLineNumber )
{
    volatile BaseType_t xSetToNonZeroToStepOutOfLoop = 0;

    taskENTER_CRITICAL();
    while( xSetToNonZeroToStepOutOfLoop == 0 )
    {
        /* Use the variables to prevent compiler warnings and in an attempt to
        ensure they can be viewed in the debugger.  If the variables get
        optimised away then set copy their values to file scope or globals then
        view the variables they are copied to. */
        ( void ) pcFileName;
        ( void ) ulLineNumber;
    }
}

void bsp_Interrupt_sysTickHandler(void)
{
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
    return;
}

void bsp_Interrupt_svCallHandler(void)
{
    extern void vPortSVCHandler(void);
    vPortSVCHandler();
    return;
}

void bsp_Interrupt_pendSvCallHandler(void)
{
    extern void xPortPendSVHandler(void);
    xPortPendSVHandler();
    return;
}

