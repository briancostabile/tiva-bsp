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
/*============================================================================*/
/**
 * @file tst_Menu.c
 * @brief
 */

#include "bsp_Types.h"
#include "tst.h"
#include "tst_Mem.h"
#include "tst_Sys.h"
#include "tst_Usb.h"


/*==============================================================================
 *                                 Globals
 *============================================================================*/

tst_TableElement_t tst_MenuTable[] =
{
    TST_SUBMENU_ELEMENT( "mem", "Memory commands", tst_Mem_menu ),
    TST_SUBMENU_ELEMENT( "sys", "System commands", tst_Sys_menu ),
    TST_SUBMENU_ELEMENT( "usb", "USB commands", tst_Usb_menu ),
    TST_END_ELEMENT
};

/*============================================================================*/
static void
svc_Cmd_dumpHelp( const tst_TableElement_t* tbl_ptr )
{
    while( tbl_ptr->handler != NULL )
    {
        TST_PRINT_HELP( tbl_ptr );
        tbl_ptr++;
    }
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
tst_Status_t
svc_Cmd_handlerText( int argc, char** argv )
{
    uint8_t                   argi;
    const tst_TableElement_t* tbl_ptr;
    int32_t                   ret = TST_STATUS_ERROR;

    tbl_ptr = tst_MenuTable;

    for( argi= 0; argi<argc; argi++ )
    {
        // Check for help first
        if( *argv[argi] == '?' )
        {
            // Dump list of commands at this level and exit
            svc_Cmd_dumpHelp( tbl_ptr );
            break;
        }

        // Find command in the current menu table
        while( tbl_ptr->handler != NULL )
        {
            if( strcmp( tbl_ptr->cmd, argv[argi] ) == 0 )
            {
                if( tbl_ptr->is_menu == true )
                {
                    tbl_ptr = tbl_ptr->submenu;

                    // If there are no arguments to pass to a handler
                    // then save as prepended command
                    if( argi == (argc - 1) )
                    {

                    }
                }
                else
                {
                    // Call function with remaining arguments
                    argi++;
                    ret = tbl_ptr->handler( (argc - argi), &argv[argi] );
                    argi = argc; // force exit the outer for loop
                }
                break;
            }
            tbl_ptr++;
        }
    }

    return( ret );
}

/*============================================================================*/
int32_t
svc_Cmd_handlerBinary( uint8_t* buf, uint16_t len )
{
    return( 0 );
}

