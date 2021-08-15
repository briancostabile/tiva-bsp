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
 * @file osapi.h
 * @brief Contains types and defines for the OS service
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include <stdbool.h>

/*==============================================================================
 *                                Defines
 *============================================================================*/


/*==============================================================================
 *                                 Types
 *============================================================================*/
typedef int32_t osapi_Timeout_t;
#define OSAPI_TIMEOUT_NO_WAIT      0
#define OSAPI_TIMEOUT_WAIT_FOREVER 0xFFFFFFFFUL

/*============================================================================*/
typedef uint8_t osapi_ThreadPriority_t;

/*============================================================================*/
typedef uint32_t osapi_ThreadStackSize_t;

/*============================================================================*/
typedef void* osapi_ThreadStackPtr_t;

/*============================================================================*/
typedef void* osapi_ThreadArg_t;

/*============================================================================*/
typedef char* osapi_ThreadName_t;

/*============================================================================*/
typedef void (*osapi_ThreadHandler_t)( osapi_ThreadArg_t arg );

/*============================================================================*/
typedef void* osapi_Semaphore_t;

/*============================================================================*/
typedef struct osapi_ThreadInitInfo_s {
    osapi_ThreadName_t      name;
    osapi_ThreadHandler_t   handler;
    osapi_ThreadArg_t       arg;
    osapi_ThreadPriority_t  priority;
    osapi_ThreadStackSize_t stackSize32;
    osapi_ThreadStackPtr_t  stackPtr;
}osapi_ThreadInitInfo_t;

/*============================================================================*/
typedef struct osapi_MemoryPoolInfo_s {
    size_t    blockSize;
    size_t    poolSize;
    void*     pool;
    uint32_t** hdrPtrPtr;
}osapi_MemoryPoolInfo_t;

/*============================================================================*/
typedef void* osapi_Queue_t;


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
osapi_init( void );

/*============================================================================*/
void
osapi_Scheduler_run( void );


/********************
 * Thread Functions
 */
/*============================================================================*/
void
osapi_Thread_create( osapi_ThreadInitInfo_t* tInitPtr );

/*============================================================================*/
void
osapi_Thread_sleep( uint32_t sleepTimeMs );


/********************
 * Semaphore Functions
 */
/*============================================================================*/
osapi_Semaphore_t
osapi_Semaphore_create( void );

/*============================================================================*/
bool
osapi_Semaphore_take( osapi_Semaphore_t sem,
                      osapi_Timeout_t   timeout );

/*============================================================================*/
void
osapi_Semaphore_give( osapi_Semaphore_t sem );


/********************
 * Memory Functions
 */
/*============================================================================*/
void*
osapi_Memory_alloc( size_t size );

/*============================================================================*/
void
osapi_Memory_free( void* freePtr );


/********************
 * Queue Functions
 */
/*============================================================================*/
osapi_Queue_t
osapi_Queue_create( size_t count,
                    size_t elementSize,
                    void*  buf );

/*============================================================================*/
bool
osapi_Queue_dequeue( osapi_Queue_t   queue,
                     void*           buf,
                     osapi_Timeout_t timeout);

/*============================================================================*/
bool
osapi_Queue_enqueue( osapi_Queue_t queue,
                     void*         buf );


/********************
 * Timer Functions
 */
typedef void*    osapi_Timer_t;

typedef uint32_t osapi_TimerId_t;

typedef enum
{
    OSAPI_TIMER_TYPE_ONE_SHOT,
    OSAPI_TIMER_TYPE_PERIODIC
} osapi_TimerType_t;

typedef void (*osapi_TimerCallback_t)( osapi_Timer_t   timer,
                                       osapi_TimerId_t id );

/*============================================================================*/
osapi_Timer_t
osapi_Timer_create( osapi_TimerId_t       id,
                    osapi_Timeout_t       timeout,
                    osapi_TimerType_t     type,
                    osapi_TimerCallback_t callback );

/*============================================================================*/
bool
osapi_Timer_start( osapi_Timer_t timer );

/*============================================================================*/
bool
osapi_Timer_stop( osapi_Timer_t timer );

/*============================================================================*/
bool
osapi_Timer_resume( osapi_Timer_t timer );
