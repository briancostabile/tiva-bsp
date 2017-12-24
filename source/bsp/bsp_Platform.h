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
