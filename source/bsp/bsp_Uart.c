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
 * @file bsp_Uart.c
 * @brief Contains Functions for hooking into a UART
 */

#include "bsp_Types.h"
#include "bsp_Uart.h"
#include "bsp_Mcu.h"
#include "bsp_Clk.h"
#include "bsp_Assert.h"
#include "bsp_Interrupt.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"


/*==============================================================================
 *                                  Defines
 *============================================================================*/
#define BSP_UART_INT_MASK_ALL (UART_IM_9BITIM | UART_IM_OEIM | UART_IM_BEIM | UART_IM_PEIM | UART_IM_FEIM | UART_IM_RTIM | UART_IM_TXIM | UART_IM_RXIM | UART_IM_DSRMIM | UART_IM_DCDMIM | UART_IM_CTSMIM | UART_IM_RIMIM)
#define BSP_UART_INT_MASK_ERR (UART_IM_OEIM | UART_IM_BEIM | UART_IM_PEIM | UART_IM_FEIM)
#define BSP_UART_INT_MASK_RX  (UART_IM_RTIM |UART_IM_RXIM)
#define BSP_UART_INT_MASK_TX  (UART_IM_TXIM)

#define BSP_UART_TX_INT_MODE_EOT   (UART_CTL_EOT)
#define BSP_UART_TX_INT_MODE_FIFO  (~UART_CTL_EOT)

/*==============================================================================
 *                                 Types
 *============================================================================*/

/*============================================================================*/
/**
 * @brief Structure to hold a client initiated Receive transaction
 */
typedef struct
{
    bool_t                valid;
    void*                 callbackArg;
    bsp_Uart_RxCallback_t rxCallback;
    uint8_t*              rcvDataPtr;
    size_t                rcvDataLen;
    size_t                rcvIdx;
} bsp_Uart_RxTrans_t;


/*============================================================================*/
/**
 * @brief Structure to hold a client initiated transmit transaction
 */
typedef struct
{
    bool_t                valid;
    void*                 callbackArg;
    bsp_Uart_TxCallback_t txCallback;
    uint8_t*              sndDataPtr;
    size_t                sndDataLen;
    size_t                sndIdx;
} bsp_Uart_TxTrans_t;

/*============================================================================*/
/**
 * @brief Structure to hold dynamic info about the UART
 */
typedef struct {
    bsp_Uart_RxTrans_t rxTransInfo;
    bsp_Uart_TxTrans_t txTransInfo;
}bsp_Uart_DynamicInfo_t;



/*==============================================================================
 *                                 Globals
 *============================================================================*/
/*============================================================================*/
bsp_Uart_DynamicInfo_t bsp_Uart_dynamicInfoTable[ BSP_UART_PLATFORM_NUM_UARTS ];


/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
bsp_Uart_txIsrCommon( bsp_Uart_Id_t id )
{
    uint32_t            baseAddr;
    bsp_Uart_TxTrans_t* txTransInfoPtr;

    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;
    txTransInfoPtr = &(bsp_Uart_dynamicInfoTable[id].txTransInfo);

    MAP_UARTIntClear( baseAddr, UART_IM_TXIM );

    /* Check for an active transaction */
    if( txTransInfoPtr->valid == TRUE )
    {
        /* Check to see if we sent all that was requested */
        if( txTransInfoPtr->sndIdx == txTransInfoPtr->sndDataLen )
        {
            txTransInfoPtr->valid = FALSE;

            MAP_UARTIntDisable( baseAddr, UART_IM_TXIM );

            if( txTransInfoPtr->txCallback != NULL )
            {
                txTransInfoPtr->txCallback( txTransInfoPtr->callbackArg );
            }
        }
        else
        {
            /* Fill up the FIFO */
            while( (txTransInfoPtr->sndIdx < txTransInfoPtr->sndDataLen) &&
                   (MAP_UARTCharPutNonBlocking( baseAddr, txTransInfoPtr->sndDataPtr[txTransInfoPtr->sndIdx] )) )
            {
                (txTransInfoPtr->sndIdx)++;
            }
        }
    }
    else
    {
        /* Copy No Data pattern */
        MAP_UARTCharPut( baseAddr, 0x00 );
    }


    return;
}

/*============================================================================*/
static void
bsp_Uart_rxIsrCommon( bsp_Uart_Id_t id )
{
    bsp_Uart_RxTrans_t* rxTransInfoPtr;
    uint32_t baseAddr;

    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;
    rxTransInfoPtr = &(bsp_Uart_dynamicInfoTable[id].rxTransInfo);

    /* Check for an active transaction */
    if( rxTransInfoPtr->valid == TRUE )
    {
        /* Get as much as possible from the receive FIFO */
        while( (MAP_UARTCharsAvail( baseAddr ) == TRUE) &&
               (rxTransInfoPtr->rcvIdx < rxTransInfoPtr->rcvDataLen) )
        {
            (rxTransInfoPtr->rcvDataPtr)[rxTransInfoPtr->rcvIdx] = MAP_UARTCharGetNonBlocking( baseAddr );
            (rxTransInfoPtr->rcvIdx)++;
        }

        /* Check to see if we read in all that was requested */
        if( rxTransInfoPtr->rcvIdx >= rxTransInfoPtr->rcvDataLen )
        {
            rxTransInfoPtr->valid = FALSE;
            if( rxTransInfoPtr->rxCallback != NULL )
            {
                rxTransInfoPtr->rxCallback( rxTransInfoPtr->callbackArg,
                                            rxTransInfoPtr->rcvDataPtr,
                                            rxTransInfoPtr->rcvDataLen );
            }
        }
    }
    return;
}


/*============================================================================*/
void
bsp_Uart_isrCommon( bsp_Uart_Id_t id )
{
    uint32_t intStatus;
    uint32_t baseAddr;

    BSP_TRACE_UART_INT_ENTER();

    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;

    intStatus = MAP_UARTIntStatus( baseAddr, TRUE );
    MAP_UARTIntClear( baseAddr, intStatus );

    /* Check for Receive Data */
    if( ((intStatus & UART_IM_RXIM) != 0) ||
        ((intStatus & UART_IM_RTIM) != 0) )
    {
        bsp_Uart_rxIsrCommon( id );
    }

    /* Check for TX Complete */
    if( (intStatus & UART_IM_TXIM) != 0 )
    {
        bsp_Uart_txIsrCommon( id );
    }

    BSP_TRACE_UART_INT_EXIT();

    return;
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Uart_init( void )
{
    bsp_Uart_Id_t i;

    for( i=0; i<DIM(bsp_Uart_staticInfoTable); i++ )
    {
        /* Disable UART peripheral block */
        MAP_SysCtlPeripheralDisable( bsp_Uart_staticInfoTable[i].sysCtrlAddr );
        bsp_Clk_delayNs( 100 );

        bsp_Uart_dynamicInfoTable[i].rxTransInfo.valid = FALSE;
        bsp_Uart_dynamicInfoTable[i].txTransInfo.valid = FALSE;
    }


    return;
}


/*============================================================================*/
void
bsp_Uart_control( bsp_Uart_Id_t     id,
                  bsp_Uart_PinSel_t rxSel,
                  bsp_Uart_PinSel_t txSel,
                  bsp_Uart_PinSel_t rtsSel,
                  bsp_Uart_PinSel_t ctsSel )
{
    uint32_t baseAddr;

    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;

    /* Configure the Pins to bring UART out */
    bsp_Gpio_configInput( bsp_Uart_staticInfoTable[id].rxPinInfoTable[rxSel].portId,
                          bsp_Uart_staticInfoTable[id].rxPinInfoTable[rxSel].mask,
                          FALSE,
                          BSP_GPIO_PULL_NONE );

    bsp_Gpio_configAltFunction( bsp_Uart_staticInfoTable[id].rxPinInfoTable[rxSel].portId,
                                bsp_Uart_staticInfoTable[id].rxPinInfoTable[rxSel].mask,
                                bsp_Uart_staticInfoTable[id].rxPinInfoTable[rxSel].altFuncId );

    bsp_Gpio_configOutput( bsp_Uart_staticInfoTable[id].txPinInfoTable[txSel].portId,
                           bsp_Uart_staticInfoTable[id].txPinInfoTable[txSel].mask,
                           FALSE,
                           BSP_GPIO_PULL_NONE );

    bsp_Gpio_configAltFunction( bsp_Uart_staticInfoTable[id].txPinInfoTable[txSel].portId,
                                bsp_Uart_staticInfoTable[id].txPinInfoTable[txSel].mask,
                                bsp_Uart_staticInfoTable[id].txPinInfoTable[txSel].altFuncId );

    /* Enable UART peripheral block */
    MAP_SysCtlPeripheralEnable( bsp_Uart_staticInfoTable[id].sysCtrlAddr );
    bsp_Clk_delayNs( 100 );

    /* Configure source Clock for UART */
    MAP_UARTClockSourceSet( baseAddr, UART_CC_CS_SYSCLK );

    /* Enable FIFOs */
    MAP_UARTFIFOEnable( baseAddr );

    /* Clear and Disable Interrupts */
    MAP_UARTIntClear( baseAddr, BSP_UART_INT_MASK_ALL );
    MAP_UARTIntDisable( baseAddr, BSP_UART_INT_MASK_ALL );

    /* Enable UART interrupt at the NVIC */
    bsp_Interrupt_enable( bsp_Uart_staticInfoTable[id].intId );

    return;
}


/*============================================================================*/
void
bsp_Uart_config( bsp_Uart_Id_t      id,
                 bsp_Uart_Baud_t    baud,
                 bsp_Uart_Parity_t  parity,
                 bsp_Uart_StopBit_t stop,
                 bsp_Uart_DataBit_t data,
                 bsp_Uart_Flow_t    flow )
{
    uint32_t baseAddr;
    uint32_t configBitmask;

    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;

    /* Setup config bitmask for dataBits, stopBits, flowControl */
    configBitmask = 0;
    switch( parity )
    {
        case BSP_UART_PARITY_ODD:   {configBitmask |= UART_LCRH_PEN;} break;
        case BSP_UART_PARITY_EVEN:  {configBitmask |= (UART_LCRH_PEN | UART_LCRH_EPS);} break;
        case BSP_UART_PARITY_ONE:   {configBitmask |= (UART_LCRH_PEN | UART_LCRH_EPS | UART_LCRH_SPS);} break;
        case BSP_UART_PARITY_ZERO:  {configBitmask |= (UART_LCRH_PEN | UART_LCRH_SPS);} break;
    }

    switch( stop )
    {
        case BSP_UART_STOP_BIT_2: {configBitmask |= UART_LCRH_STP2;} break;
    }

    switch( data )
    {
        case BSP_UART_DATA_BIT_6: {configBitmask |= UART_LCRH_WLEN_6;} break;
        case BSP_UART_DATA_BIT_7: {configBitmask |= UART_LCRH_WLEN_7;} break;
        case BSP_UART_DATA_BIT_8: {configBitmask |= UART_LCRH_WLEN_8;} break;
    }

    /* Configure for 8 databits, no flowControl */
    MAP_UARTConfigSetExpClk( baseAddr,
                             BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ,
                             baud,
                             configBitmask );

    /* Set the Tx interrupt mode */
    MAP_UARTTxIntModeSet( baseAddr, BSP_UART_TX_INT_MODE_EOT );

    /* Enable Uart */
    MAP_UARTEnable( baseAddr );

    return;
}


/*============================================================================*/
void
bsp_Uart_snd( bsp_Uart_Id_t         id,
              void*                 sndDataPtr,
              size_t                numBytes,
              void*                 callbackArg,
              bsp_Uart_TxCallback_t callback )
{
    bsp_Uart_TxTrans_t* txTransInfoPtr;
    uint32_t baseAddr;

    BSP_TRACE_UART_SND_ENTER();
    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;
    txTransInfoPtr = &(bsp_Uart_dynamicInfoTable[id].txTransInfo);

    /* Don't use the UART if there is an active transaction going on */
    BSP_ASSERT( !(txTransInfoPtr->valid) );
    BSP_ASSERT( numBytes != 0 );

    if( callback != NULL )
    {
        BSP_MCU_CRITICAL_SECTION_ENTER();
        txTransInfoPtr->txCallback  = callback;
        txTransInfoPtr->callbackArg = callbackArg;
        txTransInfoPtr->sndDataPtr  = (uint8_t *)sndDataPtr;
        txTransInfoPtr->sndDataLen  = numBytes;
        txTransInfoPtr->sndIdx = 0;
        BSP_MCU_CRITICAL_SECTION_EXIT();

        /* Wait for the previous Tx to complete */
        while( MAP_UARTBusy( baseAddr ) == TRUE );

        txTransInfoPtr->valid = TRUE;

        while( (txTransInfoPtr->sndIdx < txTransInfoPtr->sndDataLen) &&
               (MAP_UARTCharPutNonBlocking( baseAddr, txTransInfoPtr->sndDataPtr[txTransInfoPtr->sndIdx] )) )
        {
            (txTransInfoPtr->sndIdx)++;
        }

        /* Enable the Tx interrupt to continue/complete the transaction */
        MAP_UARTIntEnable( baseAddr, BSP_UART_INT_MASK_TX );
    }
    else
    {
        size_t i;

        /* No Callback so send all data inline */
        for( i=0; i<numBytes; i++ )
        {
            MAP_UARTCharPut( baseAddr, ((uint8_t *)sndDataPtr)[i] );

            /* Wait for transfer to complete */
            while( MAP_UARTBusy( baseAddr ) == TRUE );
        }
    }
    BSP_TRACE_UART_SND_EXIT();

    return;
}


/*============================================================================*/
size_t
bsp_Uart_rcv( bsp_Uart_Id_t         id,
              void*                 bufPtr,
              size_t                cnt,
              void*                 callbackArg,
              bsp_Uart_RxCallback_t callback )
{
    bsp_Uart_RxTrans_t* rxTransInfoPtr;
    size_t numWaiting;
    uint32_t baseAddr;

    BSP_TRACE_UART_RCV_ENTER();
    baseAddr = bsp_Uart_staticInfoTable[id].baseAddr;
    rxTransInfoPtr = &(bsp_Uart_dynamicInfoTable[id].rxTransInfo);

    /* Don't use the UART if there's an ongoing transaction */
    BSP_ASSERT( !(rxTransInfoPtr->valid) );

    BSP_MCU_CRITICAL_SECTION_ENTER();

    numWaiting = 0;

    rxTransInfoPtr->rcvDataPtr  = (uint8_t *)bufPtr;
    rxTransInfoPtr->callbackArg = callbackArg;
    rxTransInfoPtr->rxCallback  = callback;
    rxTransInfoPtr->rcvDataLen  = cnt;
    rxTransInfoPtr->rcvIdx = 0;
    rxTransInfoPtr->valid = TRUE;

    /* Get as much as possible from the receive FIFO */
    while( (MAP_UARTCharsAvail( baseAddr ) == TRUE) &&
           (rxTransInfoPtr->rcvIdx < rxTransInfoPtr->rcvDataLen) )
    {
        (rxTransInfoPtr->rcvDataPtr)[rxTransInfoPtr->rcvIdx] = MAP_UARTCharGetNonBlocking( baseAddr );
        (rxTransInfoPtr->rcvIdx)++;
        numWaiting++;
    }

    /* Enable the interrupt if there wasn't enough data to fulfill the request */
    if( numWaiting != cnt )
    {
        /* Enable the Rx interrupt to start the transaction */
        MAP_UARTIntEnable( baseAddr, BSP_UART_INT_MASK_RX );
    }
    else
    {
        rxTransInfoPtr->valid = FALSE;
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();

    /* If this is a synchronous call, wait for the
       data to be received by the Rx ISR */
    if( callback == NULL )
    {
        while( rxTransInfoPtr->valid ) asm( " NOP" );
    }
    BSP_TRACE_UART_RCV_EXIT();

    return( numWaiting );
}

