/*============================================================================*/
/**
 * @file bsp_Trace.h
 * @brief Contains Macros and defines Debug Tracing. These can easily compile
 *        in/out are relatively lightweight when compiled in. Generally, they
 *        should be disabled for release
 */
#ifndef BSP_TRACE_H
#define BSP_TRACE_H

#include "bsp_Gpio.h"

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
typedef struct {
    bsp_Gpio_PortId_t    portId;
    bsp_Gpio_BitMask_t   mask;
}bsp_Trace_IoInfo_t;


/* Include after defining IoInfo structure because platform depends on that structure */
#include BUILD_INCLUDE_STRING( bsp_Trace_, PLATFORM )

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Trace_init( void );

#endif
