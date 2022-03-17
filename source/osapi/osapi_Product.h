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
 * @file osapi_Product.h
 *
 * @brief
 *    The purpose of this file is to conditionally include the proper product
 *    specific header file. A compile time flag must be defined on the command
 *    line to specify a product -DPRODUCT=<productName>. This component
 *    defines all of the OSAPI defines for each supported <productName>.
 */
#pragma once

/*==============================================================================
 *                                 Defines
 *============================================================================*/
#define OSAPI_MEMORY_HDR_SIZE_32 1
#define OSAPI_MEMORY_HDR_SIZE_8  (OSAPI_MEMORY_HDR_SIZE_32 * 4)

#define OSAPI_MEMORY_POOL_DEFINE( _size, _cnt ) \
uint32_t  osapi_MemoryPool##_size[ ((_cnt) * ((_size + OSAPI_MEMORY_HDR_SIZE_8) / 4)) ]; \
uint32_t* osapi_MemoryPoolFree##_size;

#define OSAPI_MEMORY_POOL_INFO_ELEMENT( _size ) \
{ (_size),  sizeof(osapi_MemoryPool##_size),   osapi_MemoryPool##_size,   &osapi_MemoryPoolFree##_size   }


#if defined(PRODUCT)
/* The name of the product turns into the tail end of the headerfile that is included */
#include BUILD_INCLUDE_STRING(osapi_, PRODUCT)
#else
# error "PRODUCT must be defined on command line"
#endif
