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
 * @file bsp_Types.h
 * @brief Contains base types and defines.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#if defined(__TI_COMPILER_VERSION__)
#include <file.h>
#else
#include <sys/file.h>
#endif

/*============================================================================*/
/* Boolean enumerations */
#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

typedef uint8_t bool_t;

/*============================================================================*/
/* NULL Pointer definition */
#ifndef NULL
#define NULL ((void *)0)
#endif

/*============================================================================*/
/**
 * @brief Macro to compute the Dimensions of an array.
 *
 * With the following definition:
 *     uint32_t array[10];
 * The DIM Macro would be:
 *     DIM(array) == 10
 */
#define DIM(_x) (sizeof((_x)) / sizeof((_x)[0]))

/*============================================================================*/
/**
 * @brief Macro to return the larger of two numbers.
 */
#ifdef MAX
#undef MAX
#endif
#define MAX(_a, _b) (((_a) < (_b)) ? (_b) : (_a))

/*============================================================================*/
/**
 * @brief Macro to return the smaller of two numbers.
 */
#ifdef MIN
#undef MIN
#endif
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

/*============================================================================*/
/**
 * @brief Macro to return the absolute value of a signed integer.
 */
#ifndef ABS
#define ABS(_a) (((_a) < 0) ? -(_a) : (_a))
#endif

/*===========================================================================*/
/**
 * @brief
 *   This macro converts the passed in argument into a string
 */
#define XSTR(_name)                   #_name
#define CONVERT_NAME_TO_STRING(_name) XSTR(_name)

/*===========================================================================*/
/**
 * @brief
 *   This macro builds a single file name from the 2 passed in parts.
 */
#define BUILD_FILE_NAME(_prefix, _postfix) _prefix##_postfix.h

/*===========================================================================*/
/**
 * @brief
 *   This macro converts the passed in filename prefix and postfix name into
 *   the name of a headerfile to include. This macro can be used publicly to
 *   create include files for platform specific implementations. The macros are
 *   broken up into several steps to force the evaluation of each step.
 */
#define BUILD_INCLUDE_STRING(_prefix, _postfix) \
    CONVERT_NAME_TO_STRING(BUILD_FILE_NAME(_prefix, _postfix))

/*===========================================================================*/
/**
 * @brief
 *   This macro converts a 32bit address to a pointer
 */
#define ADDR_TO_PTR(_addr) ((volatile uint32_t *)(_addr))

/*===========================================================================*/
/**
 * @brief
 *   This macro converts a 32bit address to a dereferenced 32-bit register
 *   pointer
 */
#define ADDR_TO_REG(_addr) *ADDR_TO_PTR((_addr))

/*===========================================================================*/
/**
 * @brief
 *   This macro is used to define a string and place it into a specified section
 */
#define STRING(_name, _str, _sect) const char BSP_ATTR_SECTION(_sect) _name[] = _str
