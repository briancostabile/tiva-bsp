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
/*===========================================================================*/
/**
 * @file bsp_Build.c
 * @brief Contains Build strings.
 */
#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_Build.h"

#define BSP_BUILD_VERSION_FW (bsp_Build_Version_t)0x00000001
#define BSP_BUILD_VERSION_HW (bsp_Build_Version_t)0x00000101


#define BSP_BUILD_STR(_name, _str) STRING(_name, _str, ".build_str")
/*=============================================================================
 *                                   Globals
 *===========================================================================*/
BSP_BUILD_STR( bsp_Build_product, CONVERT_NAME_TO_STRING( PRODUCT ) );
BSP_BUILD_STR( bsp_Build_platform, CONVERT_NAME_TO_STRING( PLATFORM ) );
BSP_BUILD_STR( bsp_Build_timestamp, __TIMESTAMP__ );
const bsp_Build_Version_t bsp_Build_versionHw = BSP_BUILD_VERSION_HW;
const bsp_Build_Version_t bsp_Build_versionFw = BSP_BUILD_VERSION_FW;
