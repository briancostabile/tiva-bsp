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
 * @file svc_CmdEh_text.c
 * @brief
 */

#include "bsp_Types.h"
#include "tst.h"
#include "tst_Mem.h"
#include "tst_Sys.h"
#include "tst_Led.h"
#include "tst_Usb.h"


/*==============================================================================
 *                                 Globals
 *============================================================================*/
const tst_TableElement_t svc_CmdEh_textMenu[] =
{
    TST_SUBMENU_ELEMENT( "mem", "Memory commands", tst_Mem_menu ),
    TST_SUBMENU_ELEMENT( "sys", "System commands", tst_Sys_menu ),
    TST_SUBMENU_ELEMENT( "usb", "USB commands", tst_Usb_menu ),
    TST_SUBMENU_ELEMENT( "led", "LED commands", tst_Led_menu ),
    TST_END_ELEMENT
};

