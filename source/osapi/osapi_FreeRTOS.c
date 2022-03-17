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
 * @file osapi_FreeRTOS.c
 * @brief Contains necessary configuration and adaptation of FreeRTOS to the
 *        platform.
 */

#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Mcu.h"

#include "osapi.h"
#include "osapi_Product.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// Add 2 thread control blocks for idle and timer
StaticTask_t osapi_threadControlBlockArray[OSAPI_THREAD_MAX_CNT + 2];
uint8_t osapi_threadControlBlockIdx;

StaticSemaphore_t osapi_semaphoreControlBlockArray[OSAPI_SEMAPHORE_MAX_CNT];
uint8_t osapi_queueControlBlockIdx;

StaticQueue_t osapi_queueControlBlockArray[OSAPI_QUEUE_MAX_CNT];
uint8_t osapi_semaphoreControlBlockIdx;

StaticTimer_t osapi_timerControlBlockArray[OSAPI_TIMER_MAX_CNT];
uint8_t osapi_timerControlBlockIdx;

const osapi_MemoryPoolInfo_t osapi_memoryPoolTable[] =
{
    OSAPI_MEMORY_POOL_ELEMENTS
};

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
static StaticTask_t*
osapi_threadCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_threadControlBlockIdx < OSAPI_THREAD_MAX_CNT );
    StaticTask_t *tcbPtr = &osapi_threadControlBlockArray[ osapi_threadControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( tcbPtr );
}

static StaticSemaphore_t*
osapi_semaphoreCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_semaphoreControlBlockIdx < OSAPI_SEMAPHORE_MAX_CNT );
    StaticSemaphore_t *semPtr = &osapi_semaphoreControlBlockArray[ osapi_semaphoreControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( semPtr );
}

static StaticQueue_t*
osapi_queueCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_queueControlBlockIdx < OSAPI_QUEUE_MAX_CNT );
    StaticQueue_t *queuePtr = &osapi_queueControlBlockArray[ osapi_queueControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( queuePtr );
}

static StaticTimer_t*
osapi_timerCtrlBlockAlloc(void)
{
    BSP_MCU_INT_DISABLE();
    BSP_ASSERT( osapi_timerControlBlockIdx < OSAPI_TIMER_MAX_CNT );
    StaticTimer_t *timerPtr = &osapi_timerControlBlockArray[ osapi_timerControlBlockIdx++ ];
    BSP_MCU_INT_ENABLE();
    return( timerPtr );
}

/*============================================================================*/
static void
osapi_Thread_init( void )
{
    extern uint32_t _tinitStart;
    extern uint32_t _tinitEnd;

    osapi_ThreadInitInfo_t* tinitPtr = (osapi_ThreadInitInfo_t*)&_tinitStart;

    while( tinitPtr < (osapi_ThreadInitInfo_t*)&_tinitEnd )
    {
        osapi_Thread_create( tinitPtr );
        tinitPtr++;
    }
}

/*============================================================================*/
static void
osapi_Semaphore_init( void )
{
    osapi_semaphoreControlBlockIdx = 0;
    memset( osapi_semaphoreControlBlockArray, 0, sizeof(osapi_semaphoreControlBlockArray) );
}

/*============================================================================*/
static void
osapi_Memory_initPool( const osapi_MemoryPoolInfo_t* poolInfoPtr )
{
    uint8_t*  poolPtr    = ((uint8_t*)(poolInfoPtr->pool));
    uint8_t*  poolEndPtr = (poolPtr + poolInfoPtr->poolSize);
    uint32_t* hdrPtr     = (uint32_t*)poolInfoPtr->hdrPtrPtr;

    while( poolPtr < poolEndPtr )
    {
        *hdrPtr  = (uint32_t)poolPtr;
        hdrPtr   = (uint32_t*)*hdrPtr;
        poolPtr += (poolInfoPtr->blockSize + OSAPI_MEMORY_HDR_SIZE_8);
    }

    // Overwrite the last hdrPtr to Null-terminate the list
    *hdrPtr = (uint32_t)NULL;

    return;
}

/*============================================================================*/
static void
osapi_Memory_init( void )
{
    for( uint8_t i=0; i<DIM(osapi_memoryPoolTable); i++ )
    {
        osapi_Memory_initPool( &(osapi_memoryPoolTable[i]) );
    }
    return;
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
osapi_init( void )
{

    osapi_Semaphore_init();
    osapi_Memory_init();
    osapi_Thread_init();

    return;
}

/*============================================================================*/
void
osapi_Scheduler_run( void )
{
    vTaskStartScheduler();
    return;
}

/********************
 * Thread Functions
 */
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


/********************
 * Semaphore Functions
 */
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
    if( bsp_Interrupt_activeId() != 0 )
    {
        BaseType_t pxHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR( (SemaphoreHandle_t)sem, &pxHigherPriorityTaskWoken );
    }
    else
    {
        xSemaphoreGive( (SemaphoreHandle_t)sem );
    }
    return;
}

/*============================================================================*/
bool
osapi_Semaphore_take( osapi_Semaphore_t sem, osapi_Timeout_t timeout )
{
    return ( xSemaphoreTake( (SemaphoreHandle_t)sem, (TickType_t)timeout ) == pdPASS );
}


/********************
 * Memory Functions
 */
/*============================================================================*/
void*
osapi_Memory_alloc( size_t size )
{
    void* retPtr = NULL;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    for( uint8_t i=0; (retPtr == NULL) && (i<DIM(osapi_memoryPoolTable)); i++ )
    {
        if( (osapi_memoryPoolTable[i].blockSize >= size) && (*osapi_memoryPoolTable[i].hdrPtrPtr != NULL) )
        {
            void* blockPtr = *osapi_memoryPoolTable[i].hdrPtrPtr;

            // The user-block is just past the header
            retPtr = ((uint32_t*)blockPtr + OSAPI_MEMORY_HDR_SIZE_32);

            // Unlink the block at the head of the free-list and return a pointer to the user-area
            *osapi_memoryPoolTable[i].hdrPtrPtr = (uint32_t*)**osapi_memoryPoolTable[i].hdrPtrPtr;

            // Save the list head in small area before the user-area
            *(uint32_t*)blockPtr = (uint32_t)(osapi_memoryPoolTable[i].hdrPtrPtr);
        }
    }
    BSP_MCU_CRITICAL_SECTION_EXIT();
    BSP_ASSERT( retPtr != NULL );
    return( retPtr );
}


/*============================================================================*/
void
osapi_Memory_free( void* freePtr )
{
    if( freePtr )
    {
        void* blockPtr       = ((uint32_t*)freePtr - OSAPI_MEMORY_HDR_SIZE_32);
        uint32_t** hdrPtrPtr = (uint32_t**)*((uint32_t*)blockPtr);

        BSP_MCU_CRITICAL_SECTION_ENTER();
        *(uint32_t*)blockPtr = (uint32_t)*hdrPtrPtr;
        *hdrPtrPtr = blockPtr;
        BSP_MCU_CRITICAL_SECTION_EXIT();
    }
    return;
}


/********************
 * Queue Functions
 */
/*============================================================================*/
osapi_Queue_t
osapi_Queue_create( size_t count,
                    size_t elementSize,
                    void*  buf )
{
    return( xQueueCreateStatic( (UBaseType_t)count,
                                (UBaseType_t)elementSize,
                                (uint8_t*)buf,
                                osapi_queueCtrlBlockAlloc() ) );
}

/*============================================================================*/
bool
osapi_Queue_dequeue( osapi_Queue_t   queue,
                     void*           dataPtr,
                     osapi_Timeout_t timeout )
{
    BaseType_t ret;

    ret = xQueueReceive( (QueueHandle_t)queue,
                         dataPtr,
                         (TickType_t)timeout );

    return( ret == pdPASS );
}

/*============================================================================*/
bool
osapi_Queue_enqueue( osapi_Queue_t queue,
                     void*         buf )
{
    BaseType_t ret;

    if( bsp_Interrupt_activeId() != 0 )
    {
        BaseType_t pxHigherPriorityTaskWoken;
        ret = xQueueSendToBackFromISR( (QueueHandle_t)queue,
                                       buf,
                                       &pxHigherPriorityTaskWoken );
    }
    else
    {
        ret = xQueueSendToBack( (QueueHandle_t)queue,
                                buf,
                                OSAPI_TIMEOUT_NO_WAIT );
    }
    return( ret == pdTRUE );
}


/********************
 * Timer Functions
 */
/*============================================================================*/
static void
osapi_Timer_callbackWrapper( osapi_Timer_t handle )
{
    osapi_TimerName_t     id       = (osapi_TimerName_t)pcTimerGetTimerName( (TimerHandle_t)handle );
    osapi_TimerCallback_t callback = (osapi_TimerCallback_t)pvTimerGetTimerID( (TimerHandle_t)handle );

    if( callback )
    {
        callback( handle, id );
    }
    return;
}

/*============================================================================*/
osapi_Timer_t
osapi_Timer_create( osapi_TimerName_t     id,
                    osapi_Timeout_t       timeout,
                    osapi_TimerType_t     type,
                    osapi_TimerCallback_t callback )
{
    return( (osapi_Timer_t)xTimerCreateStatic( (const char*)id,
                                               pdMS_TO_TICKS(timeout),
                                               (type == OSAPI_TIMER_TYPE_ONE_SHOT) ? pdFALSE : pdTRUE,
                                               callback,
                                               (TimerCallbackFunction_t)osapi_Timer_callbackWrapper,
                                               osapi_timerCtrlBlockAlloc() ) );
}

/*============================================================================*/
osapi_Timer_t
osapi_Timer_periodicCreate( osapi_TimerName_t     id,
                            osapi_Timeout_t       timeout,
                            osapi_TimerCallback_t callback )
{
    return( osapi_Timer_create( id, timeout, OSAPI_TIMER_TYPE_PERIODIC, callback ) );
}

/*============================================================================*/
osapi_Timer_t
osapi_Timer_oneShotCreate( osapi_TimerName_t     id,
                           osapi_TimerCallback_t callback )
{
    return( osapi_Timer_create( id, 1000, OSAPI_TIMER_TYPE_ONE_SHOT, callback ) );
}

/*============================================================================*/
bool
osapi_Timer_oneShotStart( osapi_Timer_t   timer,
                          osapi_Timeout_t timeout )
{
    BaseType_t ret;

    if( bsp_Interrupt_activeId() == 0 )
    {
        ret = xTimerChangePeriod( (TimerHandle_t)timer, pdMS_TO_TICKS(timeout), (TickType_t)OSAPI_TIMEOUT_NO_WAIT );
    }
    else
    {
        BaseType_t pxHigherPriorityTaskWoken;
        ret = xTimerChangePeriodFromISR( (TimerHandle_t)timer, pdMS_TO_TICKS(timeout), &pxHigherPriorityTaskWoken );
    }

    return( ret == pdPASS );
}

/*============================================================================*/
bool
osapi_Timer_periodicStart( osapi_Timer_t timer )
{
    BaseType_t ret;

    if( bsp_Interrupt_activeId() == 0 )
    {
        ret = xTimerStart( (TimerHandle_t)timer, (TickType_t)OSAPI_TIMEOUT_NO_WAIT );
    }
    else
    {
        BaseType_t pxHigherPriorityTaskWoken;
        ret = xTimerStartFromISR( (TimerHandle_t)timer, &pxHigherPriorityTaskWoken );
    }

    return( ret == pdPASS );
}

/*============================================================================*/
bool
osapi_Timer_stop( osapi_Timer_t timer )
{
   BaseType_t ret;

    if( bsp_Interrupt_activeId() == 0 )
    {
        ret = xTimerStop( (TimerHandle_t)timer, (TickType_t)OSAPI_TIMEOUT_NO_WAIT );
    }
    else
    {
        BaseType_t pxHigherPriorityTaskWoken;
        ret = xTimerStopFromISR( (TimerHandle_t)timer, &pxHigherPriorityTaskWoken );
    }

    return( ret == pdPASS );
}


/********************
 * Free RTOS hooks
 */

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
    bsp_Reset_systemReset( BSP_RESET_SWREASON_OS_MALLOC );
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
    bsp_Reset_systemReset( BSP_RESET_SWREASON_OS_STACK );
}


/*-----------------------------------------------------------*/
/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */

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
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    memset( uxTimerTaskStack, 0xA5, sizeof(uxTimerTaskStack) );

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
    taskENTER_CRITICAL();
    bsp_Assert_reset( pcFileName, ulLineNumber );
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

