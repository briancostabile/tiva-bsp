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
 * @file bsp_I2c.c
 * @brief Contains Functions for configuring and accessing the I2C peripheral
 *        bus
 */

#include "bsp_Types.h"
#include "bsp_I2c.h"
#include "bsp_Interrupt.h"
#include "bsp_Gpio.h"
#include "bsp_Mcu.h"
#include "bsp_Trace.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"

/*============================================================================*/
#define BSP_I2C_REG( _base, _name ) ADDR_TO_REG(((_base) + I2C_O_##_name))

#define BSP_I2C_FREQUENCY_STD       100000
#define BSP_I2C_FREQUENCY_FAST      400000
#define BSP_I2C_FREQUENCY_FAST_PLUS 1000000
#define BSP_I2C_FREQUENCY_HIGH      2800000



/*============================================================================*/
#if defined BSP_PLATFORM_I2C_LIST
static const struct
{
    bsp_I2c_Id_t     id;
    bsp_I2c_PinSel_t selScl;
    bsp_I2c_PinSel_t selSda;
} bsp_I2c_idTable[] = BSP_PLATFORM_I2C_LIST;
#endif

/*============================================================================*/
// Global containing a copy of the current active transaction. This is kept as
// a copy to allow clients to reuse their own transaction globals as soon
// as the transaction complete callback is called. If transactions queue up,
// they are copied into this global before the transaction begins.
/*============================================================================*/
// Global pointers to manage the pending transaction queue. The queue is used
// when there's an active transaction
typedef struct bsp_I2c_MasterTransInfo_s
{
    bsp_I2c_MasterTrans_t* queueHeadPtr;
    bsp_I2c_MasterTrans_t* queueTailPtr;
    bsp_I2c_MasterTrans_t  active;
    uint8_t                stopFilter;
} bsp_I2c_MasterTransInfo_t;

static bsp_I2c_MasterTransInfo_t bsp_I2c_masterTransInfo[ BSP_I2C_PLATFORM_NUM ];

// Use Direct Register writes to reduce overhead
#define BSP_I2C_FIFO_STATUS(_baseAddr)     ADDR_TO_REG((_baseAddr) + I2C_O_FIFOSTATUS)
#define BSP_I2C_FIFO_PUT(_baseAddr, _data) ADDR_TO_REG((_baseAddr) + I2C_O_FIFODATA) = (_data)
#define BSP_I2C_FIFO_GET(_baseAddr)        ADDR_TO_REG((_baseAddr) + I2C_O_FIFODATA)

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
#define BSP_I2C_NEXT_BYTE_SEND(_baseAddr, _transPtr)            \
{                                                               \
    MAP_I2CMasterDataPut( (_baseAddr), *(_transPtr)->wBuffer ); \
    (_transPtr)->wBuffer = ((_transPtr)->wBuffer + 1);          \
    (_transPtr)->wLen    = ((_transPtr)->wLen - 1);             \
}

#define BSP_I2C_NEXT_BYTE_RCV(_baseAddr, _transPtr)              \
{                                                                \
    *(_transPtr)->rBuffer = MAP_I2CMasterDataGet( (_baseAddr) ); \
    (_transPtr)->rBuffer  += ((_transPtr)->rReverse) ? -1 : 1;   \
    (_transPtr)->rLen     = ((_transPtr)->rLen - 1);             \
}

#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
#define BSP_I2C_NEXT_BYTE_SEND_FIFO(_baseAddr, _transPtr)                 \
{                                                                         \
    while( ((_transPtr)->wLen > 0) &&                                     \
           (BSP_I2C_FIFO_STATUS(_baseAddr) &  I2C_FIFO_TX_FULL) == 0x00 ) \
    {                                                                     \
        BSP_I2C_FIFO_PUT( (_baseAddr), *(_transPtr)->wBuffer );           \
        (_transPtr)->wBuffer = ((_transPtr)->wBuffer + 1);                \
        (_transPtr)->wLen    = ((_transPtr)->wLen - 1);                   \
    }                                                                     \
}

#define BSP_I2C_NEXT_BYTE_RCV_FIFO(_baseAddr, _transPtr)                  \
{                                                                         \
    while( (BSP_I2C_FIFO_STATUS(_baseAddr) & I2C_FIFO_RX_EMPTY) == 0x00 ) \
    {                                                                     \
        *(_transPtr)->rBuffer = BSP_I2C_FIFO_GET( (_baseAddr) );          \
        (_transPtr)->rBuffer  += ((_transPtr)->rReverse) ? -1 : 1;        \
        (_transPtr)->rLen     = ((_transPtr)->rLen - 1);                  \
    }                                                                     \
}
#endif

//******
// Macros to reduce the error checking overhead built into the driverlib
//
//MAP_I2CMasterIntDisableEx( baseAddr, mask );
#define BSP_I2C_MASTER_INT_DISABLE_EX( _baseAddr, _mask ) \
{                                                         \
    ADDR_TO_REG((_baseAddr) + I2C_O_MIMR) &= ~(_mask);    \
}

//MAP_I2CMasterIntDisableEx( baseAddr, 0xFFFFFFFF );
//MAP_I2CMasterIntDisable( baseAddr );
#define BSP_I2C_MASTER_INT_DISABLE_EX_ALL( _baseAddr )      \
{                                                           \
    BSP_I2C_MASTER_INT_DISABLE_EX( _baseAddr, 0xFFFFFFFF ); \
}

//MAP_I2CMasterIntEnableEx( baseAddr, mask );
#define BSP_I2C_MASTER_INT_ENABLE_EX( _baseAddr, _mask )     \
{                                                     \
    ADDR_TO_REG((_baseAddr) + I2C_O_MIMR) |= (_mask); \
}

//MAP_I2CMasterIntClearEx( baseAddr, mask );
#define BSP_I2C_MASTER_INT_CLEAR_EX( _baseAddr, _mask )     \
{                                                    \
    ADDR_TO_REG((_baseAddr) + I2C_O_MICR) = (_mask); \
}

//MAP_I2CMasterIntClearEx( baseAddr, 0xFFFFFFFF );
#define BSP_I2C_MASTER_INT_CLEAR_EX_ALL( _baseAddr )      \
{                                                  \
    BSP_I2C_MASTER_INT_CLEAR_EX( _baseAddr, 0xFFFFFFFF ); \
}

//MAP_I2CRxFIFOConfigSet( baseAddr, val );
#define BSP_I2C_RX_FIFO_CONFIG_SET( _baseAddr, val )        \
{                                                           \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) &= 0x0000FFFF; \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) |= val;        \
}

//MAP_I2CTxFIFOConfigSet( baseAddr, val );
#define BSP_I2C_TX_FIFO_CONFIG_SET( _baseAddr, val )        \
{                                                           \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) &= 0xFFFF0000; \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) |= val;        \
}

//MAP_I2CMasterBurstLengthSet( baseAddr, val );
#define BSP_I2C_MASTER_BURST_LEN_SET( _baseAddr, val ) \
{                                                      \
    ADDR_TO_REG((_baseAddr) + I2C_O_MBLEN) = val;      \
}

//MAP_I2CMasterControl( baseAddr, val );
#define BSP_I2C_MASTER_CONTROL_SET( _baseAddr, val ) \
{                                                    \
    MAP_I2CMasterControl((_baseAddr), val);          \
}

//MAP_I2CMasterSlaveAddrSet( baseAddr, addr, read );
#define BSP_I2C_MASTER_SLAVE_ADDR_SET( _baseAddr, addr, read ) \
{                                                              \
    ADDR_TO_REG((_baseAddr) + I2C_O_MSA) = (addr << 1) | read; \
}

#define BSP_I2C_MASTER_INT_STATUS_EX( _baseAddr, _masked ) \
    (_masked) ?                                            \
        ADDR_TO_REG((_baseAddr) + I2C_O_MMIS) :            \
        ADDR_TO_REG((_baseAddr) + I2C_O_MRIS)              \

//MAP_I2CRxFIFOFlush( baseAddr );
#define BSP_I2C_RX_FIFO_FLUSH( _baseAddr )                           \
{                                                                    \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) |= I2C_FIFOCTL_RXFLUSH; \
}

//MAP_I2CTxFIFOFlush( baseAddr );
#define BSP_I2C_TX_FIFO_FLUSH( _baseAddr )                           \
{                                                                    \
    ADDR_TO_REG((_baseAddr) + I2C_O_FIFOCTL) |= I2C_FIFOCTL_TXFLUSH; \
}

#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
/*============================================================================*/
// Direct register writes to reduce error checking overhead
static void
bsp_I2c_masterTransStartFifo( uint32_t               baseAddr,
                              bsp_I2c_MasterTrans_t* transPtr,
                              uint8_t                stopFilter )
{
    /****** Read/Write ******/
    uint32_t intctrl;
    uint32_t ctrl;
    BSP_I2C_MASTER_INT_DISABLE_EX_ALL( baseAddr );
    BSP_I2C_MASTER_INT_CLEAR_EX_ALL( baseAddr );
    if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
    {
        uint8_t rTrig = (transPtr->rLen < 8) ? transPtr->rLen : 8;
        BSP_I2C_RX_FIFO_CONFIG_SET( baseAddr, (I2C_FIFO_CFG_RX_MASTER | (rTrig << 16)) );
        BSP_I2C_MASTER_BURST_LEN_SET( baseAddr, transPtr->rLen ); //assumes less than 256
        ctrl = (transPtr->rLen == 1) ? I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE : I2C_MASTER_CMD_FIFO_BURST_RECEIVE_START;
        intctrl = I2C_MASTER_INT_RX_FIFO_REQ;
    }
    else
    {
        BSP_I2C_TX_FIFO_CONFIG_SET( baseAddr, I2C_FIFO_CFG_TX_MASTER );
        BSP_I2C_MASTER_BURST_LEN_SET( baseAddr, transPtr->wLen ); //assumes less than 256
        BSP_I2C_NEXT_BYTE_SEND_FIFO( baseAddr, transPtr );
        ctrl = ((transPtr->wLen == 0) && (transPtr->rLen == 0)) ? I2C_MASTER_CMD_FIFO_SINGLE_SEND : I2C_MASTER_CMD_FIFO_BURST_SEND_START;
        intctrl = I2C_MASTER_INT_TX_FIFO_EMPTY;
    }
    BSP_I2C_MASTER_INT_ENABLE_EX( baseAddr, intctrl );
    BSP_I2C_MASTER_CONTROL_SET( baseAddr, (ctrl & stopFilter) );
    return;
}
#endif

#if (BSP_I2C_PLATFORM_USE_FIFO == 0)
/*============================================================================*/
static void
bsp_I2c_masterTransStartNoFifo( uint32_t               baseAddr,
                                bsp_I2c_MasterTrans_t* transPtr,
                                uint8_t                stopFilter )
{
    /****** Read/Write ******/
    uint32_t ctrl;
    if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
    {
        ctrl= (transPtr->rLen == 1) ? I2C_MASTER_CMD_SINGLE_RECEIVE : I2C_MASTER_CMD_BURST_RECEIVE_START;
    }
    else
    {
        // Set the next byte to transmit and trigger the I2C peripheral
        BSP_I2C_NEXT_BYTE_SEND( baseAddr, transPtr );
        ctrl= ((transPtr->wLen == 0) && (transPtr->rLen == 0)) ? I2C_MASTER_CMD_SINGLE_SEND : I2C_MASTER_CMD_BURST_SEND_START;
    }

    // Re-enable Master interrupts and master Mode
    MAP_I2CMasterIntEnable( baseAddr );
    BSP_I2C_MASTER_CONTROL_SET( baseAddr, (ctrl & stopFilter) );

    return;
}
#endif

/*============================================================================*/
static void
bsp_I2c_masterTransStart( uint32_t               baseAddr,
                          bsp_I2c_MasterTrans_t* transPtr,
                          uint8_t                stopFilter )
{
    BSP_TRACE_I2C_TRANS_START_ENTER();

    /****** Set Speed ******/
    volatile uint32_t* clkReg = ADDR_TO_PTR(baseAddr + I2C_O_MTPR);
    if (transPtr->speed == BSP_I2C_SPEED_HIGH)
    {
        *clkReg = ((BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ / (6 * BSP_I2C_FREQUENCY_HIGH)) - 1) | I2C_MTPR_HS;
    }
    else if (transPtr->speed == BSP_I2C_SPEED_FAST_PLUS)
    {
        *clkReg = ((BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ / (20 * BSP_I2C_FREQUENCY_FAST_PLUS)) - 3);
    }
    else if (transPtr->speed == BSP_I2C_SPEED_FAST)
    {
        *clkReg = ((BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ / (20 * BSP_I2C_FREQUENCY_FAST)) - 1);
    }
    else // if (transPtr->speed == BSP_I2C_SPEED_STANDARD)
    {
        *clkReg = ((BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ / (20 * BSP_I2C_FREQUENCY_STD)) - 1);
    }

    /****** Set Address ******/
    // Set the slave address and the r/w bit
    BSP_I2C_MASTER_SLAVE_ADDR_SET( baseAddr,
                                   transPtr->addr,
                                   (transPtr->type == BSP_I2C_TRANS_TYPE_READ) );

    transPtr->rBuffer += (transPtr->rReverse) ? (transPtr->rLen - 1) : 0;
#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
    bsp_I2c_masterTransStartFifo( baseAddr, transPtr, stopFilter );
#else
    bsp_I2c_masterTransStartNoFifo( baseAddr, transPtr, stopFilter );
#endif
    BSP_TRACE_I2C_TRANS_START_EXIT();
    return;
}

/*============================================================================*/
static void
bsp_I2c_transComplete( bsp_I2c_Id_t id,
                       uint32_t     baseAddr )
{
    bsp_I2c_MasterTrans_t* transPtr = &bsp_I2c_masterTransInfo[id].active;
    // Call callback
    transPtr->callback( 0, transPtr->usrData );

    // mark the active transaction as inactive
    transPtr->nextPtr = NULL;

    // See if there is a pending transaction in the queue
    bsp_I2c_MasterTransInfo_t* transInfoPtr = &bsp_I2c_masterTransInfo[id];
    if ( transInfoPtr->queueHeadPtr != NULL )
    {
        memcpy( &transInfoPtr->active, transInfoPtr->queueHeadPtr, sizeof(bsp_I2c_MasterTrans_t) );
        transInfoPtr->queueHeadPtr = transInfoPtr->queueHeadPtr->nextPtr;

        // Check if the queue is empty
        if( transInfoPtr->queueHeadPtr == NULL )
        {
            transInfoPtr->queueTailPtr = NULL;
        }
        transInfoPtr->active.nextPtr = &transInfoPtr->active;
        bsp_I2c_masterTransStart( baseAddr, &transInfoPtr->active, transInfoPtr->stopFilter );
    }
    else
    {
        /* DONE */
        BSP_I2C_MASTER_INT_DISABLE_EX_ALL( baseAddr );
    }
}

#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
/*============================================================================*/
static void
bsp_I2c_isrMasterCommonFifo( bsp_I2c_Id_t id,
                             uint32_t     baseAddr,
                             uint32_t     intStatus )
{
    uint32_t ctrl;
    uint32_t intctrl = 0;
    uint8_t burstLen;
    bsp_I2c_MasterTrans_t* transPtr = &bsp_I2c_masterTransInfo[id].active;
    uint8_t stopFilter = bsp_I2c_masterTransInfo[id].stopFilter;

    /* TX Fifo Interrupt */
    if( (intStatus & I2C_MASTER_INT_TX_FIFO_EMPTY) != 0 )
    {
        intStatus &= ~I2C_MASTER_INT_TX_FIFO_EMPTY;
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_I2C_MASTER_INT_DISABLE_EX( baseAddr, I2C_MASTER_INT_TX_FIFO_EMPTY );
        BSP_I2C_MASTER_INT_CLEAR_EX( baseAddr, I2C_MASTER_INT_TX_FIFO_EMPTY );
        if( transPtr->type == BSP_I2C_TRANS_TYPE_WRITE_READ )
        {
             // Switch to read mode
            transPtr->type = BSP_I2C_TRANS_TYPE_READ;

            // Disable TX Fifo trigger before switching to Rx
            BSP_I2C_TX_FIFO_CONFIG_SET( baseAddr, I2C_FIFO_CFG_TX_MASTER | I2C_FIFO_CFG_TX_NO_TRIG );

            // Reset the slave address and the r/w bit
            BSP_I2C_MASTER_SLAVE_ADDR_SET( baseAddr, transPtr->addr, true );
            uint8_t rTrig = (transPtr->rLen < 8) ? transPtr->rLen : 8;
            BSP_I2C_RX_FIFO_CONFIG_SET( baseAddr, I2C_FIFO_CFG_RX_MASTER | (rTrig << 16) );
            BSP_I2C_RX_FIFO_FLUSH( baseAddr );
            burstLen = (transPtr->rLen < 255) ? transPtr->rLen : 255;
            BSP_I2C_MASTER_BURST_LEN_SET( baseAddr, burstLen );
            ctrl = (transPtr->rLen == 1) ? I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE : I2C_MASTER_CMD_FIFO_BURST_RECEIVE_START;
            intctrl = I2C_MASTER_INT_RX_FIFO_REQ;
            BSP_I2C_MASTER_INT_ENABLE_EX( baseAddr, intctrl );
            BSP_I2C_MASTER_CONTROL_SET( baseAddr, (ctrl & stopFilter) );
        }
        else // Assume Write
        {
            if( transPtr->wLen == 0 )
            {
                bsp_I2c_transComplete( id, baseAddr );
            }
            else
            {
                // Set the next byte to transmit and trigger the I2C peripheral
                BSP_I2C_NEXT_BYTE_SEND_FIFO( baseAddr, transPtr );
                if( transPtr->wLen <= 0 )
                {
                    ctrl = I2C_MASTER_CMD_FIFO_BURST_SEND_FINISH;
                }
                else
                {
                    burstLen = (transPtr->wLen < 255) ? transPtr->wLen : 255;
                    BSP_I2C_MASTER_BURST_LEN_SET( baseAddr, burstLen );
                    ctrl = (transPtr->wLen == 1) ? I2C_MASTER_CMD_FIFO_SINGLE_SEND : I2C_MASTER_CMD_FIFO_BURST_SEND_CONT;
                }
                intctrl = I2C_MASTER_INT_TX_FIFO_EMPTY;
                BSP_I2C_MASTER_INT_ENABLE_EX( baseAddr, intctrl );
                BSP_I2C_MASTER_CONTROL_SET( baseAddr, (ctrl & stopFilter) );
            }
        }
    }

    /* RX Fifo Interrupt */
    if( (intStatus & I2C_MASTER_INT_RX_FIFO_REQ) != 0 )
    {
        intStatus &= ~I2C_MASTER_INT_RX_FIFO_REQ;
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_I2C_NEXT_BYTE_RCV_FIFO( baseAddr, transPtr );

        if( transPtr->rLen <= 0 )
        {
            BSP_I2C_RX_FIFO_CONFIG_SET( baseAddr, I2C_FIFO_CFG_RX_MASTER | I2C_FIFO_CFG_TX_NO_TRIG );
            BSP_I2C_RX_FIFO_FLUSH( baseAddr );
            BSP_I2C_MASTER_INT_DISABLE_EX( baseAddr, I2C_MASTER_INT_RX_FIFO_REQ );
            BSP_I2C_MASTER_INT_CLEAR_EX( baseAddr, I2C_MASTER_INT_RX_FIFO_REQ );
            if( stopFilter == 0 )
            {
                BSP_I2C_MASTER_CONTROL_SET( baseAddr, I2C_MASTER_CMD_BURST_SEND_STOP );
            }

            bsp_I2c_transComplete( id, baseAddr );
        }
        else
        {
            burstLen = (transPtr->rLen < 255) ? transPtr->rLen : 255;
            BSP_I2C_MASTER_BURST_LEN_SET( baseAddr, burstLen );
            ctrl = (transPtr->wLen == 1) ? I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE : I2C_MASTER_CMD_FIFO_BURST_RECEIVE_CONT;
            BSP_I2C_MASTER_CONTROL_SET( baseAddr, (ctrl & stopFilter) );
        }
    }

    /* Unexpected Interrupt */
    if( intStatus != 0 )
    {
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
    }

    return;
}
#endif

#if (BSP_I2C_PLATFORM_USE_FIFO == 0)
/*============================================================================*/
static void
bsp_I2c_isrMasterCommonNoFifo( bsp_I2c_Id_t id,
                               uint32_t     baseAddr,
                               uint32_t     intStatus )
{
    uint32_t intStatusMaster = BSP_I2C_REG( baseAddr, MCS );
    bsp_I2c_MasterTrans_t* transPtr = &bsp_I2c_masterTransInfo[id].active;
    if( (intStatusMaster & I2C_MCS_IDLE) != 0 )
    {
        BSP_TRACE_I2C_STATUS_OTHER();
        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
        }
        bsp_I2c_transComplete( id, baseAddr );
    }
    else if( (intStatusMaster & I2C_MCS_BUSBSY) != 0 )
    {
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
        uint32_t i2cCmd;

        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            // Get the next byte received and trigger the I2C peripheral
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
            i2cCmd = (transPtr->rLen > 1) ? I2C_MASTER_CMD_BURST_RECEIVE_CONT : I2C_MASTER_CMD_BURST_RECEIVE_FINISH;
        }
        else if( transPtr->type == BSP_I2C_TRANS_TYPE_WRITE_READ )
        {
            // Switch to read mode
            transPtr->type = BSP_I2C_TRANS_TYPE_READ;

            // Reset the slave address and the r/w bit
            BSP_I2C_MASTER_SLAVE_ADDR_SET( baseAddr, transPtr->addr, true );

            i2cCmd = I2C_MASTER_CMD_BURST_RECEIVE_START;
        }
        else // Assume Write
        {
            // Set the next byte to transmit and trigger the I2C peripheral
            BSP_I2C_NEXT_BYTE_SEND( baseAddr, transPtr );
            i2cCmd = (transPtr->wLen > 0) ? I2C_MASTER_CMD_BURST_SEND_CONT : I2C_MASTER_CMD_BURST_SEND_FINISH;
        }
        ROM_I2CMasterControl( baseAddr, i2cCmd );
    }
    else
    {
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
        BSP_TRACE_I2C_STATUS_OTHER();
        if( (intStatusMaster & I2C_MCS_ARBLST) != 0 )
        {
            // Check retry count and retry or fail
        }

        if( (intStatusMaster & I2C_MCS_DATACK) != 0 )
        {
            // Check retry count and retry or fail
        }

        if( (intStatusMaster & I2C_MCS_ADRACK) != 0 )
        {
            // Don't need this
        }

        if( (intStatusMaster & I2C_MCS_ERROR) != 0 )
        {
            // Check retry count and retry or fail
        }
    }

    return;
}
#endif

/*============================================================================*/
void
bsp_I2c_isrCommon( bsp_I2c_Id_t id )
{
    BSP_TRACE_INT_ENTER();
    uint32_t baseAddr = bsp_I2c_staticInfo[id].baseAddr;

    // Read out and clear the masked interrupt status registers
    uint32_t intStatusMaster = BSP_I2C_MASTER_INT_STATUS_EX( baseAddr, true );
    BSP_I2C_MASTER_INT_CLEAR_EX( baseAddr, intStatusMaster );
    if( intStatusMaster != 0 )
    {
        BSP_TRACE_I2C_ISR_MASTER_ENTER();
#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
        bsp_I2c_isrMasterCommonFifo( id, baseAddr, intStatusMaster );
#else
        bsp_I2c_isrMasterCommonNoFifo( id, baseAddr, intStatusMaster );
#endif
        BSP_TRACE_I2C_ISR_MASTER_EXIT();
    }

    BSP_TRACE_INT_EXIT();
    return;
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_I2c_init( void )
{
    for( int id=0; id<DIM(bsp_I2c_masterTransInfo); id++ )
    {
        bsp_I2c_masterTransInfo[id].queueHeadPtr = NULL;
        bsp_I2c_masterTransInfo[id].queueTailPtr = NULL;
        memset( &bsp_I2c_masterTransInfo[id].active, 0, sizeof(bsp_I2c_MasterTrans_t) );
    }

    // Disable all I2C blocks
    for( size_t i=0; i<DIM(bsp_I2c_staticInfo); i++ )
    {
        bsp_Interrupt_disable( bsp_I2c_staticInfo[i].intId );
        MAP_SysCtlPeripheralDisable( bsp_I2c_staticInfo[i].sysCtrlAddr );
    }

#if defined BSP_PLATFORM_I2C_LIST
    // Enable I2C devices platform requires
    for( size_t i=0; i<DIM(bsp_I2c_idTable); i++ )
    {
        bsp_I2c_Id_t                id = bsp_I2c_idTable[i].id;
        const bsp_I2c_StaticInfo_t* infoPtr = &bsp_I2c_staticInfo[id];
        const bsp_I2c_PinInfo_t*    pinInfoPtrScl = &infoPtr->sclPinInfoTable[bsp_I2c_idTable[i].selScl];
        const bsp_I2c_PinInfo_t*    pinInfoPtrSda = &infoPtr->sdaPinInfoTable[bsp_I2c_idTable[i].selSda];

        MAP_SysCtlPeripheralEnable( infoPtr->sysCtrlAddr );

        BSP_I2C_MASTER_INT_DISABLE_EX_ALL( infoPtr->baseAddr );
        MAP_I2CMasterIntClear( infoPtr->baseAddr );
        BSP_I2C_MASTER_INT_DISABLE_EX( infoPtr->baseAddr, 0xFFFFFFFF );

        MAP_I2CSlaveIntDisable( infoPtr->baseAddr );
        MAP_I2CSlaveIntClear( infoPtr->baseAddr );
        MAP_I2CSlaveIntDisableEx( infoPtr->baseAddr, 0xFFFFFFFF );

        bsp_Gpio_configInput( pinInfoPtrScl->portId,
                              pinInfoPtrScl->mask,
                              FALSE,
                              BSP_GPIO_PULL_NONE );

        bsp_Gpio_configAltFunction( pinInfoPtrScl->portId,
                                    pinInfoPtrScl->mask,
                                    pinInfoPtrScl->altFuncId );

        bsp_Gpio_configInput( pinInfoPtrSda->portId,
                              pinInfoPtrSda->mask,
                              TRUE,
                              BSP_GPIO_PULL_UP );

        bsp_Gpio_configAltFunction( pinInfoPtrSda->portId,
                                    pinInfoPtrSda->mask,
                                    pinInfoPtrSda->altFuncId );
#if (BSP_I2C_PLATFORM_USE_FIFO == 1)
        BSP_I2C_RX_FIFO_CONFIG_SET( infoPtr->baseAddr, I2C_FIFO_CFG_RX_MASTER | I2C_FIFO_CFG_RX_NO_TRIG );
        BSP_I2C_RX_FIFO_FLUSH( infoPtr->baseAddr );
        BSP_I2C_TX_FIFO_CONFIG_SET( infoPtr->baseAddr, I2C_FIFO_CFG_TX_MASTER | I2C_FIFO_CFG_TX_NO_TRIG );
        BSP_I2C_TX_FIFO_FLUSH( infoPtr->baseAddr );
#endif
        /* Enable I2C interrupt at the NVIC */
        bsp_Interrupt_enable( infoPtr->intId );
    }
#endif
    return;
}


/*============================================================================*/
void
bsp_I2c_masterControl( bsp_I2c_Id_t      id,
                       bsp_I2c_Control_t control )
{
    uint32_t baseAddr = bsp_I2c_staticInfo[id].baseAddr;

    BSP_MCU_CRITICAL_SECTION_ENTER();

    // Disable and clear all Master interrupts
    BSP_I2C_MASTER_INT_DISABLE_EX_ALL( baseAddr );
    MAP_I2CMasterIntClear( baseAddr );

    // Disable Master mode interrupts
    BSP_I2C_MASTER_INT_DISABLE_EX_ALL( baseAddr );
    // Enable/Disable HW block
    if( control == BSP_I2C_CONTROL_DISABLE )
    {
        MAP_I2CMasterDisable( baseAddr );
    }
    else
    {
        MAP_I2CMasterEnable( baseAddr );

        // Setup default slow 100Hz bus clock
        MAP_I2CMasterInitExpClk( baseAddr,
                                 MAP_SysCtlClockGet(),
                                 false );

        if( control == BSP_I2C_CONTROL_ENABLE_HS )
        {
            *(volatile uint32_t*)(baseAddr + I2C_O_MTPR) = 11;
            *(volatile uint32_t*)(baseAddr + I2C_O_MSA) = 0x08;
            BSP_I2C_MASTER_CONTROL_SET( baseAddr, I2C_MASTER_CMD_HS_MASTER_CODE_SEND );
            while(MAP_I2CMasterBusy( baseAddr ));
            bsp_I2c_masterTransInfo[id].stopFilter = ~I2C_MASTER_CMD_BURST_SEND_STOP;
        }
        else
        {
            bsp_I2c_masterTransInfo[id].stopFilter = 0;
        }
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
}


/*============================================================================*/
void
bsp_I2c_masterTransQueue( bsp_I2c_Id_t           id,
                          bsp_I2c_MasterTrans_t* transPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();
    BSP_TRACE_I2C_TRANS_QUEUE_ENTER();

    // Passed in transaction is always at the end of the queue
    transPtr->nextPtr = NULL;

    if( ((transPtr->wLen == 0) || (transPtr->wBuffer == NULL)) &&
        ((transPtr->rLen == 0) || (transPtr->rBuffer == NULL)) )
    {
        // Call callback with error and don't queue the transaction
        return;
    }

    // nextPtr will be NULL if there is no active transaction underway
    bsp_I2c_MasterTransInfo_t* transInfoPtr = &bsp_I2c_masterTransInfo[id];
    if( transInfoPtr->active.nextPtr == NULL )
    {
        memcpy( &transInfoPtr->active, transPtr, sizeof(bsp_I2c_MasterTrans_t) );
        transInfoPtr->active.nextPtr = &transInfoPtr->active;
        bsp_I2c_masterTransStart( bsp_I2c_staticInfo[id].baseAddr, &transInfoPtr->active, transInfoPtr->stopFilter );
    }
    else
    {
        // There is an active transaction so queue it up
        if( transInfoPtr->queueHeadPtr == NULL )
        {
            // Only thing in the queue so setup the head/tail properly
            transInfoPtr->queueHeadPtr = transPtr;
            transInfoPtr->queueTailPtr = transPtr;
        }
        else
        {
            // put at the end of the list
            transInfoPtr->queueTailPtr->nextPtr = transPtr;
            transInfoPtr->queueTailPtr = transPtr;
        }
    }

    BSP_TRACE_I2C_TRANS_QUEUE_EXIT();
    BSP_MCU_CRITICAL_SECTION_EXIT();
}
