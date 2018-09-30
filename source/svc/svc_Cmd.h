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
 * @file svc_Cmd.h
 * @brief Contains wrapper APIs and defines for OS services
 */
#ifndef SVC_CMD_H
#define SVC_CMD_H

#include "bsp_Types.h"
#include "bsp_Platform.h"
#include <stdint.h>

/*==============================================================================
 *                                Defines
 *============================================================================*/

/*==============================================================================
 *                                 Types
 *============================================================================*/

/*============================================================================*/
// Prototypes for functions that must be supplied by client
int32_t svc_Cmd_handlerText( int argc, char** argv );
int32_t svc_Cmd_handlerBinary( uint8_t* buf, uint16_t len );

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
svc_Cmd_init( void );

#endif