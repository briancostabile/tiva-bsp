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
 * @file bsp_UsbCdc.h
 * @brief Contains types and defines for the USB CDC driver
 */
#pragma once

#include "bsp_Types.h"

/*==================================================================================================
 *                                           Types
 *================================================================================================*/
typedef void (*bsp_UsbCdc_DataAvailableCallback_t)(size_t cnt);

/*==================================================================================================
 *                                        Public Functions
 *================================================================================================*/
/*============================================================================*/
bsp_UsbCdc_DataAvailableCallback_t bsp_UsbCdc_registerCallbackDataAvailable(
    int                                fd,
    bsp_UsbCdc_DataAvailableCallback_t callback);

void *bsp_UsbCdc_init(void *compositeEntry);
int   bsp_UsbCdc_open(void);
int   bsp_UsbCdc_close(int fd);
int   bsp_UsbCdc_read(int fd, char *buffer, size_t count);
int   bsp_UsbCdc_write(int fd, const char *buffer, size_t count);
