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
#ifndef OSAPI_H
#define OSAPI_H

#include "bsp_Types.h"
#include "bsp_Platform.h"

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

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
osapi_init( void );


/*============================================================================*/
void
osapi_Scheduler_run( void );


/*============================================================================*/
void
osapi_Thread_create( osapi_ThreadInitInfo_t* tInitPtr );


/*============================================================================*/
void
osapi_Thread_sleep( uint32_t sleepTimeMs );


/*============================================================================*/
osapi_Semaphore_t
osapi_Semaphore_create( void );

/*============================================================================*/
bool
osapi_Semaphore_take( osapi_Semaphore_t sem, osapi_Timeout_t timeout );

/*============================================================================*/
void
osapi_Semaphore_give( osapi_Semaphore_t sem );

#endif
