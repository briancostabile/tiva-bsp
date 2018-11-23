/**
 * Copyright 2018 Brian Costabile
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
 * @file dev_Temp.h
 * @brief Contains Macros, defines, and prototypes for the Temperature/Humidity sensor.
 */
#ifndef DEV_TEMP_H
#define DEV_TEMP_H

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
// Signed Fixed point 10.6 values
typedef int16_t dev_Temp_MeasHumid_t;
typedef int16_t dev_Temp_MeasTemp_t;

typedef void (*dev_Temp_MeasCallback_t)( dev_Temp_MeasTemp_t temp, dev_Temp_MeasHumid_t hum );

/*==============================================================================
 *                               Prototypes
 *============================================================================*/
/*===========================================================================*/
void
dev_Temp_init( dev_Temp_MeasCallback_t callback );

/*===========================================================================*/
void
dev_Temp_measTrigger( dev_Temp_MeasCallback_t callback );

/*===========================================================================*/
void
dev_Temp_measRead( dev_Temp_MeasTemp_t*   tempPtr,
                   dev_Temp_MeasHumid_t* relHumPtr );

#endif