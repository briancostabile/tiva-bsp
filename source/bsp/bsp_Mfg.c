/**
 * Copyright 2022 Brian Costabile
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
 * @file bsp_Mfg.c
 * @brief Contains Functions for accessing manufacturing section of flash hard
 *        coded to be at the very end of flash
 */
#include "bsp_Dma.h"
#include "bsp_Platform.h"
#include "bsp_Pragma.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_flash.h"

/*==============================================================================
 *                                 Defines
 *============================================================================*/
#define BSP_MFG_SECTION_SIGNATURE 0x616c7545

// Macro to get a pointer to the header which is at the very end of flash
#define BSP_MFG_HDR_PTR(_flashSize) \
    (bsp_Mfg_SectionHdr_t *)((_flashSize) - sizeof(bsp_Mfg_SectionHdr_t))

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
// Todo: Allow for product specific manufacturing data as well
typedef struct BSP_ATTR_PACKED {
    uint64_t rand0;
    uint64_t rand1;
    uint32_t productId;
    uint16_t serialNum;
    uint16_t hwRevision;
} bsp_Mfg_Data_t;

/*============================================================================*/
typedef struct BSP_ATTR_PACKED {
    uint8_t  usrLen : 8;
    uint8_t  bspLen : 8;
    uint8_t  rsv : 8;
    uint8_t  fmt : 8;
    uint32_t signature;
} bsp_Mfg_SectionHdr_t;

/*==============================================================================
 *                                Globals
 *============================================================================*/
bsp_Mfg_Data_t const *bsp_Mfg_dataPtr    = NULL;
void const *          bsp_Mfg_usrDataPtr = NULL;

// Default everything to 0 in case flash is not set
static const bsp_Mfg_Data_t bsp_Mfg_DataDefault = {0, 0, 0, 0, 0};

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Mfg_init(void)
{
    uint32_t              flashSize = MAP_SysCtlFlashSizeGet();
    bsp_Mfg_SectionHdr_t *hdrPtr    = BSP_MFG_HDR_PTR(flashSize);

    if ((hdrPtr->signature == BSP_MFG_SECTION_SIGNATURE) && (hdrPtr->fmt == 1) &&
        (hdrPtr->bspLen == sizeof(bsp_Mfg_Data_t))) {
        bsp_Mfg_dataPtr    = (bsp_Mfg_Data_t const *)(((uint32_t)hdrPtr) - hdrPtr->bspLen);
        bsp_Mfg_usrDataPtr = (void const *)(((uint32_t)hdrPtr) - (hdrPtr->bspLen + hdrPtr->usrLen));
    }
    else {
        bsp_Mfg_dataPtr    = &bsp_Mfg_DataDefault;
        bsp_Mfg_usrDataPtr = NULL;
    }

    return;
}

/*============================================================================*/
uint32_t bsp_Mfg_getProductId(void)
{
    return (bsp_Mfg_dataPtr->productId);
}

/*============================================================================*/
uint16_t bsp_Mfg_getSerialNumber(void)
{
    return (bsp_Mfg_dataPtr->serialNum);
}

/*============================================================================*/
uint16_t bsp_Mfg_getHwRevision(void)
{
    return (bsp_Mfg_dataPtr->hwRevision);
}

/*============================================================================*/
uint64_t bsp_Mfg_getRand0(void)
{
    return (bsp_Mfg_dataPtr->rand0);
}

/*============================================================================*/
uint64_t bsp_Mfg_getRand1(void)
{
    return (bsp_Mfg_dataPtr->rand1);
}

/*============================================================================*/
void const *bsp_Mfg_getUsrDataPtr(void)
{
    return (bsp_Mfg_usrDataPtr);
}
