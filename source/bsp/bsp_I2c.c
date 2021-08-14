/**
 * Copyright 2017 Brian Costabile
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
// Global pointers to manage the pending transaction queue. The queue is used
// when there's an active transaction
static bsp_I2c_MasterTrans_t* bsp_I2c_masterTransQueueHeadPtr;
static bsp_I2c_MasterTrans_t* bsp_I2c_masterTransQueueTailPtr;


/*============================================================================*/
// Global containing a copy of the current active transaction. This is kept as
// a copy to allow clients to reuse their own transaction globals as soon
// as the transaction complete callback is called. If transactions queue up,
// they are copied into this global before the transaction begins.
static bsp_I2c_MasterTrans_t bsp_I2c_masterTransActive;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
#define BSP_I2C_NEXT_BYTE_SEND(_baseAddr, _transPtr)           \
{                                                              \
    ROM_I2CMasterDataPut( (_baseAddr), *(_transPtr)->buffer ); \
    (_transPtr)->buffer = ((_transPtr)->buffer + 1);           \
    (_transPtr)->len    = ((_transPtr)->len - 1);              \
}

#define BSP_I2C_NEXT_BYTE_RCV(_baseAddr, _transPtr)            \
{                                                              \
    *(_transPtr)->buffer = ROM_I2CMasterDataGet( (_baseAddr) );\
    (_transPtr)->buffer  = ((_transPtr)->buffer + 1);          \
    (_transPtr)->len     = ((_transPtr)->len - 1);             \
}

/*============================================================================*/
static void
bsp_I2c_masterTransStart( uint32_t               baseAddr,
                          bsp_I2c_MasterTrans_t* transPtr )
{
    // Set the slave address and the r/w bit
    ROM_I2CMasterSlaveAddrSet( baseAddr,
                               transPtr->addr,
                               (transPtr->type == BSP_I2C_TRANS_TYPE_READ) );

    // Only supports 100 and 400KHz for now
    MAP_I2CMasterInitExpClk( baseAddr,
                             MAP_SysCtlClockGet(),
                             (transPtr->speed == BSP_I2C_SPEED_FAST) );

    if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
    {
        ROM_I2CMasterControl( baseAddr, (transPtr->len == 1) ?
                                            I2C_MASTER_CMD_SINGLE_RECEIVE :
                                            I2C_MASTER_CMD_BURST_RECEIVE_START );
    }
    else
    {
        // Set the next byte to transmit and trigger the I2C peripheral
        BSP_I2C_NEXT_BYTE_SEND( baseAddr, transPtr );
        ROM_I2CMasterControl( baseAddr, (transPtr->len == 0) ?
                                            I2C_MASTER_CMD_SINGLE_SEND :
                                            I2C_MASTER_CMD_BURST_SEND_START );
    }

    return;
}


/*============================================================================*/
static void
bsp_I2c_isrSlaveCommon( uint32_t baseAddr,
                        uint32_t intStatus )
{
    return;
}


/*============================================================================*/
static void
bsp_I2c_isrMasterCommon( uint32_t baseAddr,
                         uint32_t intStatus )
{
    BSP_TRACE_I2C_ISR_MASTER_ENTER();
    if( (intStatus & I2C_MCS_IDLE) != 0 )
    {
        BSP_TRACE_I2C_STATUS_IDLE();
        bsp_I2c_MasterTrans_t* transPtr = &bsp_I2c_masterTransActive;

        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
        }

        // Call callback
        transPtr->callback( 0, transPtr->usrData );

        // mark the active transaction as inactive
        transPtr->nextPtr = NULL;

        // See if there is a pending transaction in the queue
        if( bsp_I2c_masterTransQueueHeadPtr != NULL )
        {
            memcpy( &bsp_I2c_masterTransActive, bsp_I2c_masterTransQueueHeadPtr, sizeof(bsp_I2c_masterTransActive) );

            bsp_I2c_masterTransQueueHeadPtr = bsp_I2c_masterTransQueueHeadPtr->nextPtr;

            // Check if the queue is empty
            if( bsp_I2c_masterTransQueueHeadPtr == NULL )
            {
                bsp_I2c_masterTransQueueTailPtr = NULL;
            }
            bsp_I2c_masterTransActive.nextPtr = &bsp_I2c_masterTransActive;
            bsp_I2c_masterTransStart( baseAddr, &bsp_I2c_masterTransActive );
        }
    }
    else if( (intStatus & I2C_MCS_BUSBSY) != 0 )
    {
        BSP_TRACE_I2C_STATUS_BUS_BUSY();
        bsp_I2c_MasterTrans_t* transPtr = &bsp_I2c_masterTransActive;
        uint32_t i2cCmd;

        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            // Get the next byte received and trigger the I2C peripheral
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
            i2cCmd = (transPtr->len > 1) ? I2C_MASTER_CMD_BURST_RECEIVE_CONT : I2C_MASTER_CMD_BURST_RECEIVE_FINISH;
        }
        else if( transPtr->type == BSP_I2C_TRANS_TYPE_WRITE_READ )
        {
            // Switch to read mode
            transPtr->type = BSP_I2C_TRANS_TYPE_READ;

            // Reset the slave address and the r/w bit
            ROM_I2CMasterSlaveAddrSet( baseAddr, transPtr->addr, true );

            i2cCmd = I2C_MASTER_CMD_BURST_RECEIVE_START;
        }
        else // Assume Write
        {
            // Set the next byte to transmit and trigger the I2C peripheral
            BSP_I2C_NEXT_BYTE_SEND( baseAddr, transPtr );
            i2cCmd = (transPtr->len > 0) ? I2C_MASTER_CMD_BURST_SEND_CONT : I2C_MASTER_CMD_BURST_SEND_FINISH;
        }
        ROM_I2CMasterControl( baseAddr, i2cCmd );
    }
    else
    {
        BSP_TRACE_I2C_STATUS_OTHER();
        if( (intStatus & I2C_MCS_ARBLST) != 0 )
        {
            // Check retry count and retry or fail
        }

        if( (intStatus & I2C_MCS_DATACK) != 0 )
        {
            // Check retry count and retry or fail
        }

        if( (intStatus & I2C_MCS_ADRACK) != 0 )
        {
            // Don't need this
        }

        if( (intStatus & I2C_MCS_ERROR) != 0 )
        {
            // Check retry count and retry or fail
        }
    }
    BSP_TRACE_I2C_ISR_MASTER_EXIT();
    return;
}


/*============================================================================*/
void
bsp_I2c_isrCommon( bsp_I2c_Id_t id )
{
    uint32_t baseAddr = bsp_I2c_staticInfo[id].baseAddr;

    // Read out and clear the masked interrupt status registers
    uint32_t intStatusMaster = BSP_I2C_REG( baseAddr, MCS );
    MAP_I2CMasterIntClear( baseAddr );

    if( intStatusMaster != 0 )
    {
        bsp_I2c_isrMasterCommon( baseAddr, intStatusMaster );
    }

    uint32_t intStatusSlave = BSP_I2C_REG( baseAddr, SCSR );
    MAP_I2CSlaveIntClear( baseAddr );

    if( intStatusSlave != 0 )
    {
        bsp_I2c_isrSlaveCommon( baseAddr, intStatusSlave );
    }
    return;
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_I2c_init( void )
{
    bsp_I2c_masterTransQueueHeadPtr = NULL;
    bsp_I2c_masterTransQueueTailPtr = NULL;
    memset( &bsp_I2c_masterTransActive, 0, sizeof(bsp_I2c_masterTransActive) );

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
        MAP_I2CMasterIntDisable( infoPtr->baseAddr );
        MAP_I2CMasterIntClear( infoPtr->baseAddr );
        MAP_I2CSlaveIntDisable( infoPtr->baseAddr );
        MAP_I2CSlaveIntClear( infoPtr->baseAddr );

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
    MAP_I2CMasterIntDisable( baseAddr );
    MAP_I2CMasterIntClear( baseAddr );

    // Disable Master mode
    MAP_I2CMasterDisable( baseAddr );

    // Enable/Disable HW block
    if( control == BSP_I2C_CONTROL_ENABLE )
    {
        MAP_I2CMasterEnable( baseAddr );

        // Setup default slow 100Hz bus clock
        MAP_I2CMasterInitExpClk( baseAddr,
                                 MAP_SysCtlClockGet(),
                                 false );

        // Re-enable Master interrupts and master Mode
        MAP_I2CMasterIntEnable( baseAddr );
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
}


/*============================================================================*/
void
bsp_I2c_masterTransQueue( bsp_I2c_Id_t           id,
                          bsp_I2c_MasterTrans_t* transPtr )
{
    BSP_MCU_CRITICAL_SECTION_ENTER();

    // Passed in transaction is always at the end of the queue
    transPtr->nextPtr = NULL;

    if( (transPtr->len == 0) || (transPtr->buffer == NULL) )
    {
        // Call callback with error and don't queue the transaction
        return;
    }

    // nextPtr will be NULL if there is no active transaction underway
    if( bsp_I2c_masterTransActive.nextPtr == NULL )
    {
        memcpy( &bsp_I2c_masterTransActive, transPtr, sizeof(bsp_I2c_masterTransActive) );
        bsp_I2c_masterTransActive.nextPtr = &bsp_I2c_masterTransActive;
        bsp_I2c_masterTransStart( bsp_I2c_staticInfo[id].baseAddr, &bsp_I2c_masterTransActive );
    }
    else
    {
        // There is an active transaction so queue it up
        if( bsp_I2c_masterTransQueueHeadPtr == NULL )
        {
            // Only thing in the queue so setup the head/tail properly
            bsp_I2c_masterTransQueueHeadPtr = transPtr;
            bsp_I2c_masterTransQueueTailPtr = transPtr;
        }
        else
        {
            // put at the end of the list
            bsp_I2c_masterTransQueueTailPtr->nextPtr = transPtr;
            bsp_I2c_masterTransQueueTailPtr = transPtr;
        }
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
}
