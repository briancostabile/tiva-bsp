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
 * @file bsp_Trace.h
 * @brief Contains Macros and defines Debug Tracing. These can easily compile
 *        in/out are relatively lightweight when compiled in. Generally, they
 *        should be disabled for release
 */
#pragma once

#include "bsp_Gpio.h"

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
typedef struct {
    bsp_Gpio_PortId_t  portId;
    bsp_Gpio_BitMask_t mask;
} bsp_Trace_IoInfo_t;

/* Include after defining IoInfo structure because platform depends on that structure */
#include BUILD_INCLUDE_STRING(bsp_Trace_, PLATFORM)

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Trace_init(void);
