/*============================================================================*/
/**
 * @file bsp_Assert.h
 * @brief Contains macro for assert
 */
#ifndef BSP_ASSERT_H
#define BSP_ASSERT_H

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
#if defined( BSP_ASSERT_ENABLE )
#if BSP_ASSERT_HALT
#warning HALT ON ASSERTION ENABLED!
#define BSP_ASSERT( _cond )                         \
{                                                   \
    if( !(_cond) )                                  \
    {                                               \
        bsp_Assert_halt( __FUNCTION__, __LINE__ );  \
    }                                               \
}
#else
#define BSP_ASSERT( _cond )                         \
{                                                   \
    if( !(_cond) )                                  \
    {                                               \
        bsp_Assert_reset( __FUNCTION__, __LINE__ ); \
    }                                               \
}
#endif
#else
#define BSP_ASSERT( _cond )
#endif


/*============================================================================*/
/**
 * @brief Macro that works just like assert except this will never be compiled
 *        out of the build.
 */
#define BSP_ASSERT_SALSA_CHECK( _cond )             \
{                                                   \
    if( !(_cond) )                                  \
    {                                               \
        bsp_Assert_reset( __FUNCTION__, __LINE__ ); \
    }                                               \
}


/*==============================================================================
 *                                 Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Assert_halt( const char* funcName,
                 uint16_t    lineNum );

/*============================================================================*/
void
bsp_Assert_reset( const char* funcName,
                  uint16_t    lineNum );

#endif
