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
 * @file tst.h
 * @brief Contains the root of the test menu
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include <stdint.h>

/*==============================================================================
 *                                Defines
 *============================================================================*/
/*============================================================================*/
#define TST_SUBMENU_ELEMENT( _cmd, _help, _submenu ) { _cmd, _help, .submenu = (_submenu), true }
#define TST_HANDLER_ELEMENT( _cmd, _help, _handler ) { _cmd, _help, .handler = (_handler), false }
#define TST_END_ELEMENT                              TST_HANDLER_ELEMENT("","",NULL)

#define TST_PRINT_HELP( _tbl_ptr ) { printf( "%s: %s"NL, (_tbl_ptr)->cmd, (_tbl_ptr)->help ); }

// Negative return values indicate an error
#define TST_STATUS_OK    0
#define TST_STATUS_ERROR -1
typedef int32_t tst_Status_t;

// macros to define strings for commands, help, and test output
#define TST_STR_CMD(_name, _str) STRING(_name, _str, ".tst_str_cmd")
#define TST_STR_HLP(_name, _str) STRING(_name, _str, ".tst_str_hlp")
#define TST_STR_OUT(_name, _str) STRING(_name, _str, ".tst_str_out")

#ifndef NL
#define NL "\n"
#endif

/*==============================================================================
 *                                 Types
 *============================================================================*/
typedef tst_Status_t (*tst_HandlerText_t)( int argc, char** argv );

/*============================================================================*/
typedef struct tst_TableElement_s {
    const char* const cmd;
    const char* const help;
    union
    {
        tst_HandlerText_t                handler;
        const struct tst_TableElement_s* submenu;
    };
    bool_t is_menu;
} tst_TableElement_t;

extern const tst_TableElement_t tst_MenuTable[];