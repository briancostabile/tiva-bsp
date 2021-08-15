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
/*===========================================================================*/
/**
 * @file bsp_Reset.c
 * @brief Contains Macros and defines for the HW reset reason.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"


/*=============================================================================
 *                                   Globals
 *===========================================================================*/
/*******************************
 * The following globals are copied at boot to "last" globals and then they are
 * cleared and maintained throughout the operation. One every reset they are
 * latched into the corresponding "last" variable.
 ******************************/
bsp_Reset_Reason_t bsp_Reset_lastReason;
uint16_t           bsp_Reset_lastAssertLine;
const char*        bsp_Reset_lastAssertFunction;

uint16_t    bsp_Reset_assertLine BSP_ATTR_NO_INIT;
const char* bsp_Reset_assertFunction BSP_ATTR_NO_INIT;

/*
 * The following globals are placed in fixed locations in RAM so they will
 * be in the same location even after a FW upgrade.
 */
volatile uint32_t    BSP_ATTR_SECTION(".reset_info") bsp_Reset_count;
bsp_Reset_SwReason_t BSP_ATTR_SECTION(".reset_info") bsp_Reset_swReason;
uint32_t             BSP_ATTR_SECTION(".reset_info") bsp_Reset_faultRegs[8];


/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
bsp_Reset_init( void )
{
    uint32_t hwResetCause = MAP_SysCtlResetCauseGet();
    MAP_SysCtlResetCauseClear( hwResetCause );

    // Reset the reset count on a POR
    if( hwResetCause & BSP_RESET_HWREASON_POR )
    {
        bsp_Reset_count = 0;
        bsp_Reset_swReason = BSP_RESET_SWREASON_UNKNOWN;
    }
    else
    {
        bsp_Reset_count++;
    }

    // Clear out the saved fault registers if the reset reason is not a fault
    if( bsp_Reset_swReason != BSP_RESET_SWREASON_FAULT )
    {
        memset( bsp_Reset_faultRegs, 0, sizeof(bsp_Reset_faultRegs) );
    }

    bsp_Reset_HwReason_t hwReason = ( (hwResetCause & 0x0000007F) |
                                      (hwResetCause >> 6) );

    bsp_Reset_lastReason = ( ((uint32_t)hwReason << 16) |
                             ((uint32_t)bsp_Reset_swReason) );
    bsp_Reset_swReason   = BSP_RESET_SWREASON_UNKNOWN;

    bsp_Reset_lastAssertFunction = bsp_Reset_assertFunction;
    bsp_Reset_lastAssertLine = bsp_Reset_assertLine;

    bsp_Reset_assertFunction = NULL;
    bsp_Reset_assertLine = 0;

    return;
}

/*===========================================================================*/
void
bsp_Reset_systemReset( bsp_Reset_SwReason_t reason )
{
    BSP_MCU_INT_DISABLE();
    bsp_Reset_swReason = reason;

    /* No return from this */
    while(1);
    //MAP_SysCtlReset();
}

/*===========================================================================*/
void
bsp_Reset_systemFault( uint32_t* stackRegs )
{
    bsp_Reset_faultRegs[0] = stackRegs[ 0 ]; //r0
    bsp_Reset_faultRegs[1] = stackRegs[ 1 ]; //r1
    bsp_Reset_faultRegs[2] = stackRegs[ 2 ]; //r2
    bsp_Reset_faultRegs[3] = stackRegs[ 3 ]; //r3
    bsp_Reset_faultRegs[4] = stackRegs[ 4 ]; //r12
    bsp_Reset_faultRegs[5] = stackRegs[ 5 ]; //lr
    bsp_Reset_faultRegs[6] = stackRegs[ 6 ]; //pc
    bsp_Reset_faultRegs[7] = stackRegs[ 7 ]; //psr

    bsp_Reset_swReason = BSP_RESET_SWREASON_FAULT;

    /* No return from this */
    while(1);
    //MAP_SysCtlReset();
}

/*===========================================================================*/
void
bsp_Reset_setAssertInfo( const char* function_str, uint16_t line )
{
    bsp_Reset_assertFunction = function_str;
    bsp_Reset_assertLine = line;
}

/*===========================================================================*/
bsp_Reset_Reason_t
bsp_Reset_getReason( void )
{
    return( bsp_Reset_lastReason );
}

/*===========================================================================*/
bsp_Reset_SwReason_t
bsp_Reset_getSwReason( void )
{
    return( bsp_Reset_lastReason & 0x00FF );
}

/*===========================================================================*/
bsp_Reset_HwReason_t
bsp_Reset_getHwReason( void )
{
    return( (bsp_Reset_lastReason >> 16) & 0x00FF );
}

/*===========================================================================*/
const char*
bsp_Reset_getAssertFunction( void )
{
    return( bsp_Reset_lastAssertFunction );
}

/*===========================================================================*/
uint16_t
bsp_Reset_getAssertLine( void )
{
    return( bsp_Reset_lastAssertLine );
}

/*===========================================================================*/
uint32_t
bsp_Reset_getResetCount( void )
{
    return( bsp_Reset_count );
}

/*===========================================================================*/
uint32_t *
bsp_Reset_getFaultRegs( void )
{
    return( bsp_Reset_faultRegs );
}
