/**
 * Copyright 2018 Brian Costabile
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
 * @file osapi_TivaBspSensor.h
 * @brief Contains the product specific OS configuration
 */
#ifndef OSAPI_TIVA_BSP_SENSOR_H
#define OSAPI_TIVA_BSP_SENSOR_H

#include "bsp_Platform.h"

#define OSAPI_THREAD_MAX_CNT 4
#define OSAPI_SEMAPHORE_MAX_CNT 4
#define OSAPI_QUEUE_MAX_CNT 4
#define OSAPI_TIMER_MAX_CNT 6
#define OSAPI_MS_PER_TICK 1

// Define the memory pools (byte-size, count)
OSAPI_MEMORY_POOL_DEFINE(   8, 8 )
OSAPI_MEMORY_POOL_DEFINE(  16, 4 )
OSAPI_MEMORY_POOL_DEFINE(  32, 4 )
OSAPI_MEMORY_POOL_DEFINE(  64, 2 )
OSAPI_MEMORY_POOL_DEFINE( 128, 2 )

/* Arranged smallest to largest block-size */
#define OSAPI_MEMORY_POOL_ELEMENTS         \
    OSAPI_MEMORY_POOL_INFO_ELEMENT(   8 ), \
    OSAPI_MEMORY_POOL_INFO_ELEMENT(  16 ), \
    OSAPI_MEMORY_POOL_INFO_ELEMENT(  32 ), \
    OSAPI_MEMORY_POOL_INFO_ELEMENT(  64 ), \
    OSAPI_MEMORY_POOL_INFO_ELEMENT( 128 )


#endif