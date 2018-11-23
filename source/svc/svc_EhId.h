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
 * @file svc_EhId.h
 * @brief Contains Event Handler Ids and types
 */
#ifndef SVC_EH_ID_H
#define SVC_EH_ID_H

#include "bsp_Types.h"
#include "bsp_Platform.h"


#if defined(PRODUCT)
/* The name of the product turns into the tail end of the headerfile that is included */
#include BUILD_INCLUDE_STRING(svc_EhId_, PRODUCT)
#else
# error "PRODUCT must be defined on command line"
#endif

/*============================================================================*/
typedef uint8_t svc_EhId_t;

#endif