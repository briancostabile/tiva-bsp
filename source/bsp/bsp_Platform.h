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
 * @file bsp_Platform.h
 *
 * @brief
 *    The purpose of this file is to conditionally include the proper platform
 *    specific header file. A compile time flag must be defined on the command
 *    line to specify a platform -DPLATFORM=<platformName>. This component
 *    defines all of the defines for each supported <platformName>. A separate
 *    header file called main_Platform_<platformName>.h will be included by this
 *    header file.
 */
#ifndef BSP_PLATFORM_H
#define BSP_PLATFORM_H

#include "bsp_Types.h"
/*=============================================================================
 * Utility Macros
 */


#if defined(PLATFORM)
/* The name of the platform turns into the tail end of the headerfile that is included */
#include BUILD_INCLUDE_STRING(bsp_Platform_, PLATFORM)
#else
# error "PLATFORM must be defined on command line"
#endif

#endif
