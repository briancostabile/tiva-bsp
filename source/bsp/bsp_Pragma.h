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
 * @file bsp_Pragma.h
 * @brief Contains macros for compiler specific pragmas
 */
#pragma once

/*==============================================================================
 *                                   Defines
 *============================================================================*/
/*============================================================================*/
/**
 * @brief Define for the Compiler version
 */
#if defined( __TI_COMPILER_VERSION__ )
    #define BSP_PRAGMA_COMPILER_CCS TRUE
#elif defined( __GNUC__ )
    #define BSP_PRAGMA_COMPILER_GNU TRUE
#else
    #error "Unsupported toolchain"
#endif


/*==============================================================================
 *                                   Macros
 *============================================================================*/
/*============================================================================*/
/**
 * @brief Macro for allowing pragmas in macros
 */
#define JOINSTR(x,y) XSTR(x ## y)
#define BSP_PRAGMA(x) _Pragma(#x)
#define BSP_PRAGMA_DIAG(compiler,x) BSP_PRAGMA(compiler diagnostic x)


/*============================================================================*/
/**
 * @brief Macro to disable the optimization for the specified function. Note:
 *        function must follow this declaration
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_OPT_DISABLE(_func) BSP_PRAGMA(FUNCTION_OPTIONS(_func,"--opt_level=0"))
#define BSP_ATTR_OPT_DISABLE          __attribute__ ((optimize("-O0")))
#elif defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_PRAGMA_OPT_DISABLE_START BSP_PRAGMA(GCC optimize ("-O0"))
#define BSP_PRAGMA_OPT_DISABLE_END   BSP_PRAGMA(GCC reset_options)
#define BSP_ATTR_OPT_DISABLE         __attribute__ ((optimize("-O0")))
#endif


/*============================================================================*/
/**
 * @brief Macro to disable the inline optimization
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_OPT_NO_INLINE(_func) BSP_PRAGMA(FUNC_CANNOT_INLINE(_func))
#endif


/*============================================================================*/
/**
 * @brief Macro to disable variable initialization. By default CCS doesn't
 *        initialize global variables unless they are explicitly set to something
 */
#if defined(BSP_PRAGMA_COMPILER_CCS) || defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_ATTR_NO_INIT __attribute__ ((section (".noinit")));
#endif


/*============================================================================*/
/**
 * @brief Macro to place a function or data at a specific named location
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_FUNCTION_LOCATION(_data,_segment) BSP_PRAGMA(CODE_SECTION(_data, _segment))
#endif


/*============================================================================*/
/**
 * @brief Macro to place a function or data at a specific named location
 */
#if defined(BSP_PRAGMA_COMPILER_CCS) || defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_ATTR_SECTION(_section) __attribute__ ((section (_section)))
#endif


/*============================================================================*/
/**
 * @brief Macro to align data
 */
#if defined(BSP_PRAGMA_COMPILER_CCS) || defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_ATTR_ALIGNMENT(_alignment) __attribute__ ((aligned (_alignment)))
#endif


/*============================================================================*/
/**
 * @brief Macro to force data to be linked even if there are no references to
 *        the data
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_DATA_REQUIRED(_data) BSP_PRAGMA(RETAIN(_data))
#define BSP_ATTR_USED __attribute__ ((used))
#elif defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_PRAGMA_DATA_REQUIRED(_data)
#define BSP_ATTR_USED __attribute__ ((used))
#endif

/*============================================================================*/
/**
 * @brief Macro to byte pack C structures
 */
#if defined(BSP_PRAGMA_COMPILER_CCS) || defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_ATTR_PACKED __attribute__ ((__packed__))
#endif

/*============================================================================*/
/**
 * @brief Macro to suppress warnings
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_COMPILER_WARNING_DISABLE_START(gcc_option,clang_unused,msvc_unused)
#define BSP_PRAGMA_COMPILER_WARNING_DISABLE_END(gcc_option,clang_unused,msvc_unused)
#elif defined(BSP_PRAGMA_COMPILER_GNU)
#define BSP_PRAGMA_COMPILER_WARNING_DISABLE_START(gcc_option,clang_unused,msvc_unused)  BSP_PRAGMA_DIAG(GCC,push) BSP_PRAGMA_DIAG(GCC,ignored JOINSTR(-W,gcc_option))
#define BSP_PRAGMA_COMPILER_WARNING_DISABLE_END(gcc_option,clang_unused,msvc_unused) BSP_PRAGMA_DIAG(GCC,pop)
#endif


/*============================================================================*/
/**
 * @brief Macro to force a function to inline
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
    #define BSP_PRAGMA_FORCE_INLINE(_func) BSP_PRAGMA(FUNC_ALWAYS_INLINE(_func)) \
                                           inline __attribute__(( always_inline))
#elif defined(BSP_PRAGMA_COMPILER_GNU)
    #define BSP_PRAGMA_FORCE_INLINE(_func) inline __attribute__(( always_inline)) _func
#endif
