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
 * @file bsp_Ssi.c
 * @brief Contains Functions for configuring and accessing the Synchronous Serial Interface
 */
#include "bsp_Dma.h"
#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include "bsp_Clk.h"
#include "bsp_Ssi.h"
#include "bsp_Mcu.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"
#include "inc/hw_ssi.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
//#define SSI_TXEOT               0x00000040  // Transmit FIFO is empty

#define BSP_SSI_ALL_INT_FLAGS \
    (SSI_TXEOT | SSI_DMATX | SSI_DMARX | SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR)
#define BSP_SSI_ALL_DMA_FLAGS (SSI_DMA_RX | SSI_DMA_TX)

/*==============================================================================
 *                              Globals
 *============================================================================*/
/*============================================================================*/
#if defined BSP_PLATFORM_SSI_LIST
static const struct {
    bsp_Ssi_Id_t     id;
    bsp_Ssi_PinSel_t selClk;
    bsp_Ssi_PinSel_t selFss;
    bsp_Ssi_PinSel_t selDat0;
    bsp_Ssi_PinSel_t selDat1;
    bsp_Ssi_PinSel_t selDat2;
    bsp_Ssi_PinSel_t selDat3;
} bsp_Ssi_idTable[] = BSP_PLATFORM_SSI_LIST;
#endif

/*============================================================================*/
// Global containing a copy of the current active transaction. This is kept as
// a copy to allow clients to reuse their own transaction globals as soon
// as the transaction complete callback is called. If transactions queue up,
// they are copied into this global before the transaction begins.
/*============================================================================*/
// Global pointers to manage the pending transaction queue. The queue is used
// when there's an active transaction
typedef struct bsp_Ssi_MasterTransInfo_s {
    bsp_Ssi_MasterTrans_t *queueHeadPtr;
    bsp_Ssi_MasterTrans_t *queueTailPtr;
    bsp_Ssi_MasterTrans_t  active;
} bsp_Ssi_MasterTransInfo_t;

bsp_Ssi_MasterTransInfo_t bsp_Ssi_masterTransInfo[BSP_SSI_PLATFORM_NUM];

// Use Direct Register writes to reduce overhead
#define BSP_SSI_FIFO_STATUS(_baseAddr)     ADDR_TO_REG((_baseAddr) + SSI_O_SR)
#define BSP_SSI_FIFO_PUT(_baseAddr, _data) ADDR_TO_REG((_baseAddr) + SSI_O_DR) = (uint32_t)(_data)
#define BSP_SSI_FIFO_GET(_baseAddr)        (uint8_t)(ADDR_TO_REG((_baseAddr) + SSI_O_DR))

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
#define BSP_SSI_NEXT_BYTE_SEND_FIFO(_baseAddr, _transPtr)                                  \
    {                                                                                      \
        while (((_transPtr)->wLen > 0) && (BSP_SSI_FIFO_STATUS(_baseAddr) & SSI_SR_TNF)) { \
            BSP_SSI_FIFO_PUT((_baseAddr), *(_transPtr)->wBuffer);                          \
            (_transPtr)->wBuffer = ((_transPtr)->wBuffer + 1);                             \
            (_transPtr)->wLen    = ((_transPtr)->wLen - 1);                                \
        }                                                                                  \
    }

#define BSP_SSI_NEXT_BYTE_RCV_FIFO(_baseAddr, _transPtr)              \
    {                                                                 \
        while ((_transPtr)->rLen > 0) {                               \
            *(_transPtr)->rBuffer = BSP_SSI_FIFO_GET((_baseAddr));    \
            (_transPtr)->rBuffer += ((_transPtr)->rReverse) ? -1 : 1; \
            (_transPtr)->rLen = ((_transPtr)->rLen - 1);              \
        }                                                             \
    }

/*===========================================================================*/
static void bsp_Ssi_csEnable(bsp_Ssi_MasterTrans_t *transPtr)
{
    bsp_Gpio_write(transPtr->csPort, transPtr->csMask, transPtr->csMaskEn);
    return;
}

/*===========================================================================*/
static void bsp_Ssi_csDisable(bsp_Ssi_MasterTrans_t *transPtr)
{
    bsp_Gpio_write(transPtr->csPort, transPtr->csMask, (transPtr->csMask ^ transPtr->csMaskEn));
    return;
}

/*============================================================================*/
static void bsp_Ssi_masterTxFifoFill(uint32_t baseAddr, bsp_Ssi_MasterTrans_t *transPtr)
{
    MAP_SSIIntDisable(baseAddr, SSI_TXEOT);
    BSP_SSI_NEXT_BYTE_SEND_FIFO(baseAddr, transPtr);
    MAP_SSIIntEnable(baseAddr, SSI_TXEOT);
    return;
}

/*============================================================================*/
static void bsp_Ssi_masterRxFifoFlush(uint32_t baseAddr)
{
    uint32_t tmp;
    while (MAP_SSIDataGetNonBlocking(baseAddr, &tmp) != 0)
        ;
    return;
}

/*============================================================================*/
static void bsp_Ssi_masterTransStart(uint32_t baseAddr, bsp_Ssi_MasterTrans_t *transPtr)
{
    uint32_t advMode;

    MAP_SSIDisable(baseAddr);
    MAP_SSIConfigSetExpClk(
        baseAddr,
        bsp_Clk_sysClkGet(),
        transPtr->mode,
        SSI_MODE_MASTER,
        transPtr->speed,
        8);    // Data-Width-bits

    // setup the type of transaction write-only or write-read
    if ((transPtr->type == BSP_SSI_TRANS_TYPE_WRITE) || (transPtr->rBuffer != NULL)) {
        advMode = SSI_ADV_MODE_WRITE;
    }
    else {
        advMode = SSI_ADV_MODE_READ_WRITE;
    }
    SSIAdvModeSet(baseAddr, advMode);
    transPtr->rBuffer += (transPtr->rReverse) ? (transPtr->rLen - 1) : 0;
    transPtr->sLen = transPtr->wLen - transPtr->rLen;

    // Clear out Rx Fifo, Fill Tx Fifo then enable device
    bsp_Ssi_masterRxFifoFlush(baseAddr);
    MAP_SSIEnable(baseAddr);
    bsp_Ssi_csEnable(transPtr);
    bsp_Ssi_masterTxFifoFill(baseAddr, transPtr);

    return;
}

/*============================================================================*/
static void bsp_Ssi_transComplete(bsp_Ssi_Id_t id, uint32_t baseAddr)
{
    bsp_Ssi_MasterTransInfo_t *transInfoPtr = &bsp_Ssi_masterTransInfo[id];

    // Call callback
    transInfoPtr->active.callback(transInfoPtr->active.usrData);

    // See if there is a pending transaction in the queue
    if (transInfoPtr->queueHeadPtr != NULL) {
        memcpy(&transInfoPtr->active, transInfoPtr->queueHeadPtr, sizeof(bsp_Ssi_MasterTrans_t));
        transInfoPtr->queueHeadPtr = transInfoPtr->queueHeadPtr->nextPtr;

        // Check if the queue is empty
        if (transInfoPtr->queueHeadPtr == NULL) {
            transInfoPtr->queueTailPtr = NULL;
        }
        transInfoPtr->active.nextPtr = &transInfoPtr->active;
        bsp_Ssi_masterTransStart(baseAddr, &transInfoPtr->active);
    }
    else {
        transInfoPtr->active.nextPtr = NULL;
        MAP_SSIIntDisable(baseAddr, BSP_SSI_ALL_INT_FLAGS);
    }
}

/*============================================================================*/
void bsp_Ssi_isrCommon(bsp_Ssi_Id_t id)
{
    uint32_t               baseAddr  = bsp_Ssi_staticInfo[id].baseAddr;
    bsp_Ssi_MasterTrans_t *transPtr  = &bsp_Ssi_masterTransInfo[id].active;
    uint32_t               intStatus = MAP_SSIIntStatus(baseAddr, true);
    MAP_SSIIntClear(baseAddr, intStatus);

    /* TX Fifo Interrupt */
    if ((intStatus & SSI_TXEOT) != 0) {
        // Grab read data if any is expected
        if (transPtr->rBuffer != NULL) {
            // Skip over any bytes received in the address/command
            // portion of the transaction
            uint32_t tmp;
            while (transPtr->sLen > 0) {
                MAP_SSIDataGetNonBlocking(baseAddr, &tmp);
                transPtr->sLen--;
            }
            BSP_SSI_NEXT_BYTE_RCV_FIFO(baseAddr, transPtr);
        }

        if (transPtr->wLen == 0) {
            bsp_Ssi_csDisable(transPtr);
            bsp_Ssi_transComplete(id, baseAddr);
        }
        else {
            // Fill up Tx FIFO
            bsp_Ssi_masterTxFifoFill(baseAddr, transPtr);
        }
    }

    return;
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Ssi_init(void)
{
    for (int id = 0; id < DIM(bsp_Ssi_masterTransInfo); id++) {
        bsp_Ssi_masterTransInfo[id].queueHeadPtr = NULL;
        bsp_Ssi_masterTransInfo[id].queueTailPtr = NULL;
        memset(&bsp_Ssi_masterTransInfo[id].active, 0, sizeof(bsp_Ssi_MasterTrans_t));
    }

    // Disable all SSI blocks
    for (size_t i = 0; i < DIM(bsp_Ssi_staticInfo); i++) {
        bsp_Interrupt_disable(bsp_Ssi_staticInfo[i].intId);
        MAP_SysCtlPeripheralDisable(bsp_Ssi_staticInfo[i].sysCtrlAddr);
    }

#if defined BSP_PLATFORM_SSI_LIST
    // Enable SSI devices platform requires
    for (size_t i = 0; i < DIM(bsp_Ssi_idTable); i++) {
        bsp_Ssi_Id_t                id      = bsp_Ssi_idTable[i].id;
        const bsp_Ssi_StaticInfo_t *infoPtr = &bsp_Ssi_staticInfo[id];
        bsp_Ssi_PinSel_t            pinSel;

        MAP_SysCtlPeripheralEnable(infoPtr->sysCtrlAddr);
        while (MAP_SysCtlPeripheralReady(infoPtr->sysCtrlAddr) == FALSE)
            ;

        MAP_SSIDMADisable(infoPtr->baseAddr, BSP_SSI_ALL_DMA_FLAGS);
        MAP_SSIIntDisable(infoPtr->baseAddr, BSP_SSI_ALL_INT_FLAGS);
        MAP_SSIIntClear(infoPtr->baseAddr, BSP_SSI_ALL_INT_FLAGS);

        MAP_SSIAdvModeSet(infoPtr->baseAddr, SSI_ADV_MODE_LEGACY);
        SSIClockSourceSet(infoPtr->baseAddr, SSI_CLOCK_SYSTEM);

        pinSel = bsp_Ssi_idTable[i].selClk;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configAltFunction(
                infoPtr->clkPinInfoTable[pinSel].portId,
                infoPtr->clkPinInfoTable[pinSel].mask,
                infoPtr->clkPinInfoTable[pinSel].altFuncId);
        }
        pinSel = bsp_Ssi_idTable[i].selFss;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configAltFunction(
                infoPtr->fssPinInfoTable[pinSel].portId,
                infoPtr->fssPinInfoTable[pinSel].mask,
                infoPtr->fssPinInfoTable[pinSel].altFuncId);
        }
        pinSel = bsp_Ssi_idTable[i].selDat0;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configAltFunction(
                infoPtr->dat0PinInfoTable[pinSel].portId,
                infoPtr->dat0PinInfoTable[pinSel].mask,
                infoPtr->dat0PinInfoTable[pinSel].altFuncId);
        }
        pinSel = bsp_Ssi_idTable[i].selDat1;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configInput(
                infoPtr->dat1PinInfoTable[pinSel].portId,
                infoPtr->dat1PinInfoTable[pinSel].mask,
                FALSE,
                BSP_GPIO_PULL_NONE);

            bsp_Gpio_configAltFunction(
                infoPtr->dat1PinInfoTable[pinSel].portId,
                infoPtr->dat1PinInfoTable[pinSel].mask,
                infoPtr->dat1PinInfoTable[pinSel].altFuncId);
        }
        pinSel = bsp_Ssi_idTable[i].selDat2;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configAltFunction(
                infoPtr->dat2PinInfoTable[pinSel].portId,
                infoPtr->dat2PinInfoTable[pinSel].mask,
                infoPtr->dat2PinInfoTable[pinSel].altFuncId);
        }
        pinSel = bsp_Ssi_idTable[i].selDat3;
        if (pinSel != BSP_SSI_PIN_SEL_NONE) {
            bsp_Gpio_configAltFunction(
                infoPtr->dat3PinInfoTable[pinSel].portId,
                infoPtr->dat3PinInfoTable[pinSel].mask,
                infoPtr->dat3PinInfoTable[pinSel].altFuncId);
        }

#if (BSP_SSI_PLATFORM_USE_DMA == 1)
        /* Configure DMA */
#endif
        /* Enable SSI interrupt at the NVIC */
        bsp_Interrupt_enable(infoPtr->intId);
    }
#endif

    return;
}

/*============================================================================*/
void bsp_Ssi_masterControl(bsp_Ssi_Id_t id, bsp_Ssi_Control_t control)
{
    uint32_t baseAddr = bsp_Ssi_staticInfo[id].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    // Disable and clear all interrupts
    MAP_SSIIntDisable(baseAddr, BSP_SSI_ALL_INT_FLAGS);
    MAP_SSIIntClear(baseAddr, BSP_SSI_ALL_INT_FLAGS);

    // Enable/Disable HW block
    if (control == BSP_SSI_CONTROL_DISABLE) {
        MAP_SSIDisable(baseAddr);
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}

/*============================================================================*/
void bsp_Ssi_masterTransQueue(bsp_Ssi_Id_t id, bsp_Ssi_MasterTrans_t *transPtr)
{
    BSP_MCU_CRITICAL_SECTION_ENTER();

    // Passed in transaction is always at the end of the queue
    transPtr->nextPtr = NULL;

    if ((transPtr->wLen == 0) || (transPtr->wBuffer == NULL) || (transPtr->callback == NULL) ||
        ((transPtr->type == BSP_SSI_TRANS_TYPE_WRITE_READ) && (transPtr->rBuffer == NULL)) ||
        (transPtr->rLen > transPtr->wLen)) {
        // Call callback with error and don't queue the transaction
        return;
    }

    // nextPtr will be NULL if there is no active transaction underway
    bsp_Ssi_MasterTransInfo_t *transInfoPtr = &bsp_Ssi_masterTransInfo[id];
    if (transInfoPtr->active.nextPtr == NULL) {
        memcpy(&transInfoPtr->active, transPtr, sizeof(bsp_Ssi_MasterTrans_t));
        transInfoPtr->active.nextPtr = &transInfoPtr->active;
        bsp_Ssi_masterTransStart(bsp_Ssi_staticInfo[id].baseAddr, &transInfoPtr->active);
    }
    else {
        // There is an active transaction so queue it up
        if (transInfoPtr->queueHeadPtr == NULL) {
            // Only thing in the queue so setup the head/tail properly
            transInfoPtr->queueHeadPtr = transPtr;
            transInfoPtr->queueTailPtr = transPtr;
        }
        else {
            // put at the end of the list
            transInfoPtr->queueTailPtr->nextPtr = transPtr;
            transInfoPtr->queueTailPtr          = transPtr;
        }
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
    return;
}
