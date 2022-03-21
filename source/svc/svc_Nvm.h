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
 * @file svc_Nvm.h
 * @brief Contains the structures, definitions and functions to access NVM
 *        structures
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Pragma.h"

#define SVC_NVM_DATA_SIGNATURE 0x616c7545

typedef struct svc_Nvm_DataHdr_s {
    uint32_t crc;
    uint32_t signature;
    uint16_t type;
    uint16_t version;
    uint32_t len;
} svc_Nvm_DataHdr_t;

#if defined(PRODUCT)
/* The name of the product turns into the tail end of the headerfile that is included */
#include BUILD_INCLUDE_STRING(svc_Nvm_, PRODUCT)
#else
typedef svc_Nvm_DataHdr_t svc_Nvm_Data_t;
static inline void
svc_Nvm_updateCalData(void *dataPtr, uint32_t chIdx, int16_t vBusOffset, int16_t vShuntOffset)
{
    return;
}
static inline bool svc_Nvm_validData(void *dataPtr)
{
    return true;
}
static inline void svc_Nvm_setHdr(void *dataPtr)
{
    return;
}
static inline void svc_Nvm_initData(void *dataPtr)
{
    return;
}
#endif

/*============================================================================*/
void svc_Nvm_init(void);

/*============================================================================*/
void svc_Nvm_save(void);

/*============================================================================*/
static inline void *svc_Nvm_dataPtr(void)
{
    extern svc_Nvm_Data_t svc_Nvm_data;
    return ((void *)&svc_Nvm_data);
}
