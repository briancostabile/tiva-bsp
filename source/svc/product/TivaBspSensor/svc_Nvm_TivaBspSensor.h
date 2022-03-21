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
 * @file svc_Nvm_TivaBspCore.h
 * @brief Contains the NVM structure
 */
#pragma once

#include "bsp_Pragma.h"
#include "bsp_Platform.h"
#include "bsp_Assert.h"

/*==============================================================================
 *                                Types
 *============================================================================*/

#define SVC_NVM_DATA_TYPE      0x1111
#define SVC_NVM_DATA_FORMAT_V1 0x0001

typedef struct svc_Nvm_ChannelInfo_s {
    int16_t vBusOffset;
    int16_t vShuntOffset;
} svc_Nvm_ChannelInfo_t;

typedef struct svc_Nvm_DataV1_s {
    uint32_t              numCh;
    svc_Nvm_ChannelInfo_t channelInfo[BSP_PLATFORM_PWRMON_NUM_CHANNELS];
} svc_Nvm_DataV1_t;

typedef struct svc_Nvm_Data_s {
    svc_Nvm_DataHdr_t hdr;
    union {
        svc_Nvm_DataV1_t v1;
    } data;
} svc_Nvm_Data_t;

/*============================================================================*/
static inline bool svc_Nvm_validData(void *dataPtr)
{
    svc_Nvm_DataHdr_t *hdrPtr = ((svc_Nvm_DataHdr_t *)dataPtr);

    return (
        (hdrPtr->signature == SVC_NVM_DATA_SIGNATURE) && (hdrPtr->type == SVC_NVM_DATA_TYPE) &&
        (hdrPtr->version == SVC_NVM_DATA_FORMAT_V1) && (hdrPtr->len == sizeof(svc_Nvm_DataV1_t)));
}

/*============================================================================*/
static inline void svc_Nvm_setHdr(void *dataPtr)
{
    svc_Nvm_DataHdr_t *hdrPtr = ((svc_Nvm_DataHdr_t *)dataPtr);
    hdrPtr->signature         = SVC_NVM_DATA_SIGNATURE;
    hdrPtr->type              = SVC_NVM_DATA_TYPE;
    hdrPtr->version           = SVC_NVM_DATA_FORMAT_V1;
    hdrPtr->len               = sizeof(svc_Nvm_DataV1_t);
    return;
}

/*============================================================================*/
static inline void svc_Nvm_initData(void *dataPtr)
{
    svc_Nvm_DataV1_t *dataPtrV1 = &((svc_Nvm_Data_t *)dataPtr)->data.v1;
    memset(dataPtrV1, 0, sizeof(svc_Nvm_DataV1_t));
    dataPtrV1->numCh = BSP_PLATFORM_PWRMON_NUM_CHANNELS;
    return;
}

/*============================================================================*/
static inline void
svc_Nvm_updateCalData(void *dataPtr, uint32_t chIdx, int16_t vBusOffset, int16_t vShuntOffset)
{
    svc_Nvm_DataV1_t *dataPtrV1 = &((svc_Nvm_Data_t *)dataPtr)->data.v1;
    BSP_ASSERT(chIdx < dataPtrV1->numCh);
    dataPtrV1->channelInfo[chIdx].vBusOffset   = vBusOffset;
    dataPtrV1->channelInfo[chIdx].vShuntOffset = vShuntOffset;
    return;
}

/*============================================================================*/
static inline int16_t svc_Nvm_getCalDataBusOffset(void *dataPtr, uint32_t chIdx)
{
    svc_Nvm_DataV1_t *dataPtrV1 = &((svc_Nvm_Data_t *)dataPtr)->data.v1;
    BSP_ASSERT(chIdx < dataPtrV1->numCh);
    return dataPtrV1->channelInfo[chIdx].vBusOffset;
}

/*============================================================================*/
static inline int16_t svc_Nvm_getCalDataShuntOffset(void *dataPtr, uint32_t chIdx)
{
    svc_Nvm_DataV1_t *dataPtrV1 = &((svc_Nvm_Data_t *)dataPtr)->data.v1;
    BSP_ASSERT(chIdx < dataPtrV1->numCh);
    return dataPtrV1->channelInfo[chIdx].vShuntOffset;
}

