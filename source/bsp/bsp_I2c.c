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
/**
 * @brief Structure to hold Pin muxing info for I2C pins
 */
typedef struct {
    bsp_Gpio_PortId_t    portId;
    bsp_Gpio_BitMask_t   mask;
    bsp_Gpio_AltFuncId_t altFuncId;
}bsp_I2c_PinInfo_t;


/*============================================================================*/
/**
 * @brief Structure to hold constant info about the I2C hardware
 */
typedef struct {
    uint32_t          baseAddr;
    uint32_t          sysCtrlAddr;
    uint32_t          intId;
    bsp_I2c_PinInfo_t sclPinInfo;
    bsp_I2c_PinInfo_t sdaPinInfo;
}bsp_I2c_StaticInfo_t;


/*============================================================================*/
static const bsp_I2c_StaticInfo_t bsp_I2c_staticInfo[] =
{
   { I2C0_BASE, SYSCTL_PERIPH_I2C0, BSP_INTERRUPT_ID_I2C0,
     {BSP_GPIO_PORT_ID(PB2), BSP_GPIO_MASK(PB2), BSP_GPIO_ALT_FUNC(PB2_I2C0SCL)}, // Pin 47
     {BSP_GPIO_PORT_ID(PB3), BSP_GPIO_MASK(PB3), BSP_GPIO_ALT_FUNC(PB3_I2C0SDA)}  // Pin 48
   },
   { I2C1_BASE, SYSCTL_PERIPH_I2C1, BSP_INTERRUPT_ID_I2C1,
     {BSP_GPIO_PORT_ID(PA6), BSP_GPIO_MASK(PA6), BSP_GPIO_ALT_FUNC(PA6_I2C1SCL)}, // Pin 23
     {BSP_GPIO_PORT_ID(PA7), BSP_GPIO_MASK(PA7), BSP_GPIO_ALT_FUNC(PA7_I2C1SDA)}  // Pin 24
   },
   { I2C2_BASE, SYSCTL_PERIPH_I2C2, BSP_INTERRUPT_ID_I2C2,
     {BSP_GPIO_PORT_ID(PE4), BSP_GPIO_MASK(PE4), BSP_GPIO_ALT_FUNC(PE4_I2C2SCL)}, // Pin 59
     {BSP_GPIO_PORT_ID(PE5), BSP_GPIO_MASK(PE5), BSP_GPIO_ALT_FUNC(PE5_I2C2SDA)}  // Pin 60
   },
   { I2C3_BASE, SYSCTL_PERIPH_I2C3, BSP_INTERRUPT_ID_I2C3,
     {BSP_GPIO_PORT_ID(PD0), BSP_GPIO_MASK(PD0), BSP_GPIO_ALT_FUNC(PD0_I2C3SCL)}, // Pin 61
     {BSP_GPIO_PORT_ID(PD1), BSP_GPIO_MASK(PD1), BSP_GPIO_ALT_FUNC(PD1_I2C3SDA)}  // Pin 62
   }
};


/*============================================================================*/
static const bsp_I2c_Id_t bsp_I2c_idTable[] =
{
   BSP_PLATFORM_I2C_LIST
};


static bsp_I2c_MasterTrans_t* bsp_I2c_masterTransQueueHeadPtr;
static bsp_I2c_MasterTrans_t* bsp_I2c_masterTransQueueTailPtr;


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
        bsp_I2c_MasterTrans_t* transPtr = bsp_I2c_masterTransQueueHeadPtr;

        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
        }

        // Special case when transaction structures are reused during i2c callbacks.
        // In that case the nextPtr will be pointing back to the current structure
        // That's fine and will cause the driver to re-execute whatever is in the
        // structure after the callback has been called. The structure is safe to
        // modify as soon as the callback is called. To identify when that has
        // happened, set the nextPtr to NULL before calling the callback and if
        // it's still NULL afterwards then that means the callback didn't reuse the
        // structure.
        if( transPtr->nextPtr == transPtr )
        {
            transPtr->nextPtr = NULL;
        }

        // Call callback
        transPtr->callback( 0, transPtr->usrData );

        // Unlink the transaction
        bsp_I2c_masterTransQueueHeadPtr = bsp_I2c_masterTransQueueHeadPtr->nextPtr;
        if( bsp_I2c_masterTransQueueHeadPtr == NULL )
        {
            bsp_I2c_masterTransQueueTailPtr = NULL;
        }

        // See if there's another transaction waiting to be started
        if( bsp_I2c_masterTransQueueHeadPtr != NULL )
        {
             bsp_I2c_masterTransStart( baseAddr, bsp_I2c_masterTransQueueHeadPtr );
        }
    }
    else if( (intStatus & I2C_MCS_BUSBSY) != 0 )
    {
        BSP_TRACE_I2C_STATUS_BUS_BUSY();
        bsp_I2c_MasterTrans_t* transPtr = bsp_I2c_masterTransQueueHeadPtr;
        uint32_t i2cCmd;

        if( transPtr->type == BSP_I2C_TRANS_TYPE_READ )
        {
            // Get the next byte received and trigger the I2C peripheral
            BSP_I2C_NEXT_BYTE_RCV( baseAddr, transPtr );
            i2cCmd = (transPtr->len > 1) ? I2C_MASTER_CMD_BURST_RECEIVE_CONT : I2C_MASTER_CMD_BURST_RECEIVE_FINISH;
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
static void
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

    // Disable all I2C blocks
    for( size_t i=0; i<DIM(bsp_I2c_staticInfo); i++ )
    {
        bsp_Interrupt_disable( bsp_I2c_staticInfo[i].intId );
        MAP_SysCtlPeripheralDisable( bsp_I2c_staticInfo[i].sysCtrlAddr );
    }

    // Enable I2C devices platform requires
    for( size_t i=0; i<DIM(bsp_I2c_idTable); i++ )
    {
        bsp_I2c_Id_t                     id = bsp_I2c_idTable[i];
        const bsp_I2c_StaticInfo_t* infoPtr = &bsp_I2c_staticInfo[id];

        MAP_SysCtlPeripheralEnable( infoPtr->sysCtrlAddr );
        MAP_I2CMasterIntDisable( infoPtr->baseAddr );
        MAP_I2CMasterIntClear( infoPtr->baseAddr );
        MAP_I2CSlaveIntDisable( infoPtr->baseAddr );
        MAP_I2CSlaveIntClear( infoPtr->baseAddr );

        bsp_Gpio_configInput( infoPtr->sclPinInfo.portId,
                              infoPtr->sclPinInfo.mask,
                              FALSE,
                              BSP_GPIO_PULL_NONE );

        bsp_Gpio_configAltFunction( infoPtr->sclPinInfo.portId,
                                    infoPtr->sclPinInfo.mask,
                                    FALSE,
                                    infoPtr->sclPinInfo.altFuncId );

        bsp_Gpio_configInput( infoPtr->sdaPinInfo.portId,
                              infoPtr->sdaPinInfo.mask,
                              TRUE,
                              BSP_GPIO_PULL_UP );

        bsp_Gpio_configAltFunction( infoPtr->sdaPinInfo.portId,
                                    infoPtr->sdaPinInfo.mask,
                                    FALSE,
                                    infoPtr->sdaPinInfo.altFuncId );

        /* Enable UART interrupt at the NVIC */
        bsp_Interrupt_enable( infoPtr->intId );
    }

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

    if( bsp_I2c_masterTransQueueHeadPtr == NULL )
    {
        // Only thing in the queue so trigger the transaction immediately
        bsp_I2c_masterTransQueueHeadPtr = transPtr;
        bsp_I2c_masterTransQueueTailPtr = transPtr;
        bsp_I2c_masterTransStart( bsp_I2c_staticInfo[id].baseAddr, transPtr );
    }
    else
    {
        // put at the end of the list
        bsp_I2c_masterTransQueueTailPtr->nextPtr = transPtr;
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();
}


/*============================================================================*/
void
bsp_I2c_interruptHandler0( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID0 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C0 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler1( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID1 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C1 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler2( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID2 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C2 );
    return;
}


/*============================================================================*/
void
bsp_I2c_interruptHandler3( void )
{
    bsp_I2c_isrCommon( BSP_I2C_ID3 );
    bsp_Interrupt_clearPending( BSP_INTERRUPT_ID_I2C3 );
    return;
}
