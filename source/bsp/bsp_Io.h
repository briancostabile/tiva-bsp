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
 * @file bsp_Io.h
 * @brief Contains function prototypes for connecting up libc with serial devices
 */
#pragma once

#include "bsp_Types.h"

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
typedef int (*bsp_Io_Open_t)(const char *path, unsigned int flags, int llv_fd);

/*============================================================================*/
typedef int (*bsp_Io_Close_t)(int file_descriptor);

/*============================================================================*/
typedef int (*bsp_Io_Read_t)(int file_descriptor, char *buffer, size_t count);

/*============================================================================*/
typedef int (*bsp_Io_Write_t)(int file_descriptor, const char *buffer, size_t count);

/*============================================================================*/
typedef off_t (*bsp_Io_Lseek_t)(int file_descriptor, off_t offset, int origin);

/*============================================================================*/
typedef int (*bsp_Io_Unlink_t)(const char *path);

/*============================================================================*/
typedef int (*bsp_Io_Rename_t)(const char *old_name, const char *new_name);

/*==============================================================================
 *                              Prototypes
 *============================================================================*/
/*============================================================================*/
void bsp_Io_init(void);

/*============================================================================*/
int bsp_Io_fileToDfd(FILE *file);

/*============================================================================*/
void bsp_Io_addDevice(
    const char *    path,
    void *          cookie,
    bsp_Io_Open_t   open,
    bsp_Io_Close_t  close,
    bsp_Io_Read_t   read,
    bsp_Io_Write_t  write,
    bsp_Io_Lseek_t  lseek,
    bsp_Io_Unlink_t unlink,
    bsp_Io_Rename_t rename);
