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
 * @file bsp_Assert.h
 * @brief Contains macro for assert
 */
#pragma once

#include "bsp_Platform.h"
#include "bsp_Reset.h"

/*==============================================================================
 *                                   Macros
 *============================================================================*/

/*============================================================================*/
/**
 * @brief Assert Macro.
 *
 * This macro checks the passed in condition and if the condition fails then
 * it calls a fatal error. If the condition fails this macro will not return.
 * In non-debug builds this macro is empty.
 *
 * @param _cond: This is the condition to be evaluated. The condition must
 *               return TRUE or FALSE.
 * @return None.
 */
#if defined(BSP_ASSERT_ENABLE)
#if defined(BSP_ASSERT_HALT)
//#warning HALT ON ASSERTION ENABLED!
#define BSP_ASSERT(_cond)                            \
    {                                                \
        if (!(_cond)) {                              \
            bsp_Assert_halt(__FUNCTION__, __LINE__); \
        }                                            \
    }
#else
#define BSP_ASSERT(_cond)                             \
    {                                                 \
        if (!(_cond)) {                               \
            bsp_Assert_reset(__FUNCTION__, __LINE__); \
        }                                             \
    }
#endif
#else
#define BSP_ASSERT(_cond)
#endif

/*============================================================================*/
/**
 * @brief Macro that works just like assert except this will never be compiled
 *        out of the build.
 */
#define BSP_ASSERT_SALSA_CHECK(_cond)                 \
    {                                                 \
        if (!(_cond)) {                               \
            bsp_Assert_reset(__FUNCTION__, __LINE__); \
        }                                             \
    }

/*==============================================================================
 *                                 Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Assert_halt(const char *funcName, uint16_t lineNum);

/*============================================================================*/
void bsp_Assert_reset(const char *funcName, uint16_t lineNum);
