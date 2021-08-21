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
 * @file bsp_Reset.h
 * @brief Contains Macros and defines for the reset reason.
 */
#pragma once

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
/*===========================================================================*/
// A type to hold the concatenated HW+SW reset reason
typedef uint32_t bsp_Reset_Reason_t;

/*===========================================================================*/
enum
{
    BSP_RESET_HWREASON_EXT  = 0x0001,
    BSP_RESET_HWREASON_POR  = 0x0002,
    BSP_RESET_HWREASON_BOR  = 0x0004,
    BSP_RESET_HWREASON_WDT0 = 0x0008,
    BSP_RESET_HWREASON_SW   = 0x0010,
    BSP_RESET_HWREASON_WDT1 = 0x0020,
    BSP_RESET_HWREASON_HIB  = 0x0040,
    BSP_RESET_HWREASON_HREQ = 0x1000
};
typedef uint8_t bsp_Reset_HwReason_t;

/*===========================================================================*/
enum
{
    BSP_RESET_SWREASON_UNKNOWN       = 0x0000,
    BSP_RESET_SWREASON_ASSERT        = 0x0001,
    BSP_RESET_SWREASON_UNHANDLED_INT = 0x0002,
    BSP_RESET_SWREASON_FAULT         = 0x0003,
    BSP_RESET_SWREASON_EXIT          = 0x0004,
    BSP_RESET_SWREASON_OS_MALLOC     = 0x0005,
    BSP_RESET_SWREASON_OS_STACK      = 0x0006,
};
typedef uint8_t bsp_Reset_SwReason_t;

/*==============================================================================
 *                               Prototypes
 *============================================================================*/
/*===========================================================================*/
void
bsp_Reset_init( void );

/*===========================================================================*/
bsp_Reset_Reason_t
bsp_Reset_getReason( void );

/*===========================================================================*/
bsp_Reset_SwReason_t
bsp_Reset_getSwReason( void );

/*===========================================================================*/
bsp_Reset_HwReason_t
bsp_Reset_getHwReason( void );

/*===========================================================================*/
const char*
bsp_Reset_getAssertFunction( void );

/*===========================================================================*/
uint16_t
bsp_Reset_getAssertLine( void );

/*===========================================================================*/
uint32_t
bsp_Reset_getResetCount( void );

/*===========================================================================*/
void
bsp_Reset_setAssertInfo( const char* function_str, uint16_t line );

/*===========================================================================*/
void
bsp_Reset_systemReset( bsp_Reset_SwReason_t reason );

/*===========================================================================*/
void
bsp_Reset_systemFault( uint32_t* stackRegs );

/*===========================================================================*/
uint32_t *
bsp_Reset_getFaultRegs( void );
