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
 * @file bsp_Dma.c
 * @brief Contains Functions for configuring and accessing the System DMA
 */
#include "bsp_Dma.h"
#include "bsp_Platform.h"
#include "bsp_Pragma.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "inc/hw_udma.h"

/*==============================================================================
 *                              Types
 *============================================================================*/
typedef struct BSP_ATTR_PACKED bsp_Dma_ChDescCtrl_s {
    uint32_t xferMode : 3;
    uint32_t nxtUseBurst : 1;
    uint32_t xfersize : 10;
    uint32_t arbSize : 4;
    uint32_t srcProt : 1;
    uint32_t rsvd1 : 2;
    uint32_t dstProt : 1;
    uint32_t rsvd2 : 2;
    uint32_t srcSize : 2;
    uint32_t srcInc : 2;
    uint32_t dstSize : 2;
    uint32_t dstInc : 2;
} bsp_Dma_ChDescCtrl_t;

typedef struct BSP_ATTR_PACKED bsp_Dma_ChDesc_s {
    void *               srcPtr;
    void *               dstPtr;
    bsp_Dma_ChDescCtrl_t ctrl;
    uint32_t             rsvd;
} bsp_Dma_ChDesc_t;

#define BSP_DMA_CH_NUM 32
/*==============================================================================
 *                              Globals
 *============================================================================*/
/*============================================================================*/
static BSP_ATTR_ALIGNMENT(1024) bsp_Dma_ChDesc_t bsp_Dma_chDesc[BSP_DMA_CH_NUM * 2];

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Dma_init(void)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    while (MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA) == FALSE)
        ;
    MAP_uDMAEnable();
    MAP_uDMAControlBaseSet(bsp_Dma_chDesc);
    MAP_uDMAIntClear(UDMA_CHIS_M);
    return;
}

/*============================================================================*/
void bsp_Dma_chConfig(
    bsp_Dma_ChId_t      chId,
    bsp_Dma_XferMode_t  mode,
    bsp_Dma_Increment_t srcInc,
    bsp_Dma_Increment_t srcSize,
    void *              srcPtr,
    bsp_Dma_Increment_t dstInc,
    bsp_Dma_Increment_t dstSize,
    void *              dstPtr,
    size_t              size)
{
    bsp_Dma_chDesc[chId].srcPtr = srcPtr;
    bsp_Dma_chDesc[chId].dstPtr = dstPtr;
    bsp_Dma_chDesc[chId].rsvd   = 0;

    bsp_Dma_chDesc[chId].ctrl.xferMode    = mode;
    bsp_Dma_chDesc[chId].ctrl.nxtUseBurst = 0;
    bsp_Dma_chDesc[chId].ctrl.xfersize    = size;
    bsp_Dma_chDesc[chId].ctrl.arbSize     = BSP_DMA_ARB_SIZE_16;
    bsp_Dma_chDesc[chId].ctrl.rsvd1       = 0;
    bsp_Dma_chDesc[chId].ctrl.dstProt     = BSP_DMA_PROT_NOT_PRIVILEGED;
    bsp_Dma_chDesc[chId].ctrl.rsvd2       = 0;
    bsp_Dma_chDesc[chId].ctrl.srcSize     = srcSize;
    bsp_Dma_chDesc[chId].ctrl.srcInc      = srcInc;
    bsp_Dma_chDesc[chId].ctrl.dstSize     = dstSize;
    bsp_Dma_chDesc[chId].ctrl.dstInc      = dstInc;
}

/*============================================================================*/
void bsp_Dma_chStart(bsp_Dma_ChId_t chId, bsp_Dma_XferDoneCallback_t callback)
{
    MAP_uDMAChannelEnable((0x00000001 << chId));
    return;
}
