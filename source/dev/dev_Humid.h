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
 * @file dev_Humid.h
 * @brief Contains Macros, defines, and prototypes for the Temperature/Humidity sensor.
 */
#pragma once

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
// Signed Fixed point 10.6 values
typedef int16_t dev_Humid_MeasHumidity_t;
typedef int16_t dev_Humid_MeasTemperature_t;

typedef void (*dev_Humid_MeasCallback_t)( dev_Humid_MeasHumidity_t    humidity,
                                          dev_Humid_MeasTemperature_t temperature );
typedef void (*dev_Humid_MeasCallbackHumidity_t)( dev_Humid_MeasHumidity_t meas );
typedef void (*dev_Humid_MeasCallbackTemperature_t)( dev_Humid_MeasTemperature_t meas );

/*==============================================================================
 *                               Prototypes
 *============================================================================*/
/*===========================================================================*/
void
dev_Humid_init( void );

/*===========================================================================*/
void
dev_Humid_measTrigger( dev_Humid_MeasCallback_t callback );

/*===========================================================================*/
void
dev_Humid_measTriggerHumidity( dev_Humid_MeasCallbackHumidity_t callback );

/*===========================================================================*/
void
dev_Humid_measReadHumidity( dev_Humid_MeasHumidity_t* measPtr );

/*===========================================================================*/
void
dev_Humid_measTriggerTemperature( dev_Humid_MeasCallbackHumidity_t callback );

/*===========================================================================*/
void
dev_Humid_measReadTemperature( dev_Humid_MeasTemperature_t* measPtr );
