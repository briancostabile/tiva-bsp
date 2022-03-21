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
 * @file bsp_Ssi.h
 * @brief Contains types and prototypes to access the DMA engine
 */
#pragma once

#include "bsp_Platform.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"

#include BUILD_INCLUDE_STRING(bsp_Ssi_, PLATFORM_CORE)

/*==============================================================================
 *                                Types
 *============================================================================*/

/*============================================================================*/
typedef uint32_t bsp_Ssi_Speed_t;

/*============================================================================*/
#define BSP_SSI_CONTROL_DISABLE ((bsp_Ssi_Control_t)0)
#define BSP_SSI_CONTROL_ENABLE  ((bsp_Ssi_Control_t)1)
typedef uint8_t bsp_Ssi_Control_t;

/*============================================================================*/
#define BSP_SSI_TRANS_TYPE_WRITE      ((bsp_Ssi_TransType_t)0)
#define BSP_SSI_TRANS_TYPE_WRITE_READ ((bsp_Ssi_TransType_t)1)
typedef uint8_t bsp_Ssi_TransType_t;

/*============================================================================*/
// Mode 0 :Pol-0 Pha-0
// Mode 1 :Pol-0 Pha-1
// Mode 2 :Pol-1 Pha-0
// Mode 3 :Pol-1 Pha-1
#define BSP_SSI_TRANS_MODE_0 ((bsp_Ssi_TransMode_t)0)
#define BSP_SSI_TRANS_MODE_1 ((bsp_Ssi_TransMode_t)1)
#define BSP_SSI_TRANS_MODE_2 ((bsp_Ssi_TransMode_t)2)
#define BSP_SSI_TRANS_MODE_3 ((bsp_Ssi_TransMode_t)3)
typedef uint8_t bsp_Ssi_TransMode_t;

/*============================================================================*/
/*============================================================================*/
typedef void (*bsp_Ssi_MasterTransCallback_t)(void *userData);

typedef struct bsp_Ssi_MasterTrans_n {
    struct bsp_Ssi_MasterTrans_n *nextPtr;
    bsp_Ssi_Speed_t               speed;
    bsp_Ssi_TransType_t           type;
    bsp_Ssi_TransMode_t           mode;
    bool_t                        rReverse;
    size_t                        sLen;
    size_t                        rLen;
    uint8_t *                     rBuffer;
    size_t                        wLen;
    uint8_t *                     wBuffer;
    uint32_t                      csPort;
    uint32_t                      csMask;
    uint32_t                      csMaskEn;
    bsp_Ssi_MasterTransCallback_t callback;
    void *                        usrData;
} bsp_Ssi_MasterTrans_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Ssi_init(void);

/*============================================================================*/
void bsp_Ssi_masterControl(bsp_Ssi_Id_t id, bsp_Ssi_Control_t control);

/*============================================================================*/
void bsp_Ssi_masterTransQueue(bsp_Ssi_Id_t id, bsp_Ssi_MasterTrans_t *transPtr);
