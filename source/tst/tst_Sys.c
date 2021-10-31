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
 * @file tst_Sys.c
 * @brief Test functions to read/write arbitrary memory regions
 */

#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Reset.h"
#include "bsp_Build.h"
#include "bsp_Mcu.h"
#include "tst.h"
#include "tst_Sys.h"
#include "osapi.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*==============================================================================
 *                                 Globals
 *============================================================================*/
// String globals
TST_STR_CMD( TST_SYS_VERSION_STR_CMD, "version" );
TST_STR_HLP( TST_SYS_VERSION_STR_HLP, "Dump Version" );

TST_STR_CMD( TST_SYS_RESET_STR_CMD,        "reset" );
TST_STR_CMD( TST_SYS_RESET_FORCE_STR_CMD,  "force" );
TST_STR_HLP( TST_SYS_RESET_FORCE_STR_HLP,  "Force Reset" );
TST_STR_CMD( TST_SYS_RESET_ASSERT_STR_CMD, "assert" );
TST_STR_HLP( TST_SYS_RESET_ASSERT_STR_HLP, "Force Assert" );
TST_STR_CMD( TST_SYS_RESET_FAULT_STR_CMD,  "fault" );
TST_STR_HLP( TST_SYS_RESET_FAULT_STR_HLP,  "Force Fault" );
TST_STR_CMD( TST_SYS_RESET_INFO_STR_CMD,   "info" );
TST_STR_HLP( TST_SYS_RESET_INFO_STR_HLP,   "Dump Reset Info" );

/*==============================================================================
 *                            Private Functions
 *============================================================================*/


/*============================================================================*/
static tst_Status_t
tst_Sys_resetInfo( int argc, char** argv )
{
    printf("Reset Count: %u"NL,(unsigned int)bsp_Reset_getResetCount() );
    printf("Reset Reason: 0x%08X"NL, (unsigned int)bsp_Reset_getReason() );
    if( bsp_Reset_getSwReason() == BSP_RESET_SWREASON_ASSERT )
    {
        printf("Assert: %s:%d"NL, bsp_Reset_getAssertFunction(), bsp_Reset_getAssertLine() );
    }
    if( bsp_Reset_getSwReason() == BSP_RESET_SWREASON_FAULT )
    {
        char* reg_names[] = { "r0 ", "r1 ", "r2 ", "r3 ", "r12", "lr ", "pc ", "psr" };
        uint32_t* reg_array = bsp_Reset_getFaultRegs();
        for( uint8_t i=0; i<DIM(reg_names); i++ )
        {
            printf( "%s: 0x%08X:%u:%d"NL, reg_names[i],
                    (unsigned int)reg_array[i], (unsigned int)reg_array[i], (int)reg_array[i] );
        }
    }

    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Sys_resetForce( int argc, char** argv )
{
    bsp_Reset_systemReset( BSP_RESET_SWREASON_UNKNOWN );
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Sys_resetAssert( int argc, char** argv )
{
    bsp_Assert_reset( __FUNCTION__, __LINE__ );
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Sys_resetFault( int argc, char** argv )
{
    ADDR_TO_REG(0xFFFFFFFF) = ADDR_TO_REG(0xFFFFFFFF) + 1;
    return( TST_STATUS_OK );
}

/*============================================================================*/
static tst_Status_t
tst_Sys_version( int argc, char** argv )
{
    printf( "Product: %s"NL, bsp_Build_product );
    printf( "Platform: %s"NL, bsp_Build_platform );
    printf( "Build Date: %s"NL, bsp_Build_timestamp );
    printf( "HW Version: 0x%08lX"NL, bsp_Build_versionHw );
    printf( "FW Version: 0x%08lX"NL, bsp_Build_versionFw );
    printf( "Serial Number, 0x%08lX"NL, BSP_PLATFORM_UNIQUE32() );
    return( TST_STATUS_OK );
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
// Helper macro to cleanup the table
#define TST_SYS_CMD( _uname, _lname ) TST_HANDLER_ELEMENT( TST_SYS_##_uname##_STR_CMD, \
                                                           TST_SYS_##_uname##_STR_HLP, \
                                                           tst_Sys_##_lname )

/*============================================================================*/
const tst_TableElement_t tst_Sys_resetMenu[] =
{
    TST_SYS_CMD( RESET_INFO,   resetInfo ),
    TST_SYS_CMD( RESET_FORCE,  resetForce ),
    TST_SYS_CMD( RESET_ASSERT, resetAssert ),
    TST_SYS_CMD( RESET_FAULT,  resetFault ),
    TST_END_ELEMENT
};

/*============================================================================*/
const tst_TableElement_t tst_Sys_menu[] =
{
    TST_SYS_CMD( VERSION,  version ),
    TST_SUBMENU_ELEMENT( TST_SYS_RESET_STR_CMD, "Reset commands", tst_Sys_resetMenu ),
    TST_END_ELEMENT
};
