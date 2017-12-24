/*============================================================================*/
/**
 * @file bsp_Pragma.h
 * @brief Contains macros for compiler specific pragmas
 */
#ifndef BSP_PRAGMA_H
#define BSP_PRAGMA_H

/*==============================================================================
 *                                   Defines
 *============================================================================*/
/*============================================================================*/
/**
 * @brief Define for the Compiler version
 */
#if defined( __TI_COMPILER_VERSION__ )
    #define BSP_PRAGMA_COMPILER_CCS TRUE
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
#define BSP_PRAGMA(x) _Pragma(#x)


/*============================================================================*/
/**
 * @brief Macro to disable the optimization for the specified function. Note:
 *        function must follow this declaration
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_OPT_DISABLE(_func) BSP_PRAGMA(FUNCTION_OPTIONS(_func,"--opt_level=0"))
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
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_NO_INIT
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
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_DATA_LOCATION(_data,_segment) BSP_PRAGMA(DATA_SECTION(_data, _segment))
#endif


/*============================================================================*/
/**
 * @brief Macro to align data
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_DATA_ALIGNMENT(_data,_alignment) BSP_PRAGMA(DATA_ALIGN(_data, _alignment))
#endif


/*============================================================================*/
/**
 * @brief Macro to force data to be linked even if there are no references to
 *        the data
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_DATA_REQUIRED(_data) BSP_PRAGMA(RETAIN(_data))
#endif

/*============================================================================*/
/**
 * @brief Macro to byte pack C structures
 */
#if defined(BSP_PRAGMA_COMPILER_CCS)
#define BSP_PRAGMA_PACK_START
#define BSP_PRAGMA_PACK_END
#define BSP_PRAGMA_PACKED __attribute__ ((__packed__))
#endif

#endif
