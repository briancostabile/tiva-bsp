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
 * @file svc_Log.h
 * @brief Contains Simple macro wrapper for printing messages
 */
#pragma once

#include <stdio.h>

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif

/*==============================================================================
 *                                Defines
 *============================================================================*/
#define SVC_LOG_LEVEL_NONE  4
#define SVC_LOG_LEVEL_ERROR 3
#define SVC_LOG_LEVEL_WARN  2
#define SVC_LOG_LEVEL_INFO  1

#if (SVC_LOG_LEVEL <= SVC_LOG_LEVEL_INFO)
#define SVC_LOG_INFO( ... ) printf( __VA_ARGS__ )
#else
#define SVC_LOG_INFO( ... )
#endif

#if (SVC_LOG_LEVEL <= SVC_LOG_LEVEL_WARN)
#define SVC_LOG_WARN( ... ) printf( __VA_ARGS__ )
#else
#define SVC_LOG_WARN( ... )
#endif

#if (SVC_LOG_LEVEL <= SVC_LOG_LEVEL_ERROR)
#define SVC_LOG_ERROR( ... ) printf( __VA_ARGS__ )
#else
#define SVC_LOG_ERROR( ... )
#endif

#define ENDLINE "\n"
#define NL "\n"
