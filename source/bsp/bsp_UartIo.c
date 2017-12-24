/*============================================================================*/
/**
 * @file bsp_UartConsole.c
 * @brief Contains Functions for configuring and accessing the UARTs
 */

#include "bsp_Platform.h"
#include "bsp_UartIo.h"
#include "bsp_Uart.h"
#include "bsp_Clk.h"
#include "bsp_Gpio.h"
#include "bsp_Mcu.h"
#include "bsp_Assert.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include <string.h>


/*==============================================================================
 *                                Macros
 *============================================================================*/

/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_WRITE_PTR( _infoPtr )                 \
    &((_infoPtr)->rxBufInfo.ptr[ (_infoPtr)->rxBufInfo.writeIdx ])


/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_READ_PTR( _infoPtr )                  \
    &((_infoPtr)->rxBufInfo.ptr[ (_infoPtr)->rxBufInfo.readIdx ])


/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_AVAIL( _infoPtr )            \
( ((_infoPtr)->rxBufInfo.writeIdx > (_infoPtr)->rxBufInfo.readIdx) ?     \
      ((_infoPtr)->rxBufInfo.size - (_infoPtr)->rxBufInfo.writeIdx) :    \
      ((_infoPtr)->rxBufInfo.readIdx - (_infoPtr)->rxBufInfo.writeIdx) )


/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_WRITE( _infoPtr, _nBytes )                     \
{                                                                           \
    if( (_nBytes) > 0 )                                                     \
    {                                                                       \
        BSP_MCU_CRITICAL_SECTION_ENTER();                                   \
        if( (_infoPtr)->rxBufInfo.readIdx == (_infoPtr)->rxBufInfo.size )   \
        {                                                                   \
            (_infoPtr)->rxBufInfo.readIdx = (_infoPtr)->rxBufInfo.writeIdx; \
        }                                                                   \
        (_infoPtr)->rxBufInfo.writeIdx += (_nBytes);                        \
        if( (_infoPtr)->rxBufInfo.writeIdx == (_infoPtr)->rxBufInfo.size )  \
        {                                                                   \
            (_infoPtr)->rxBufInfo.writeIdx = 0;                             \
        }                                                                   \
        BSP_MCU_CRITICAL_SECTION_EXIT();                                    \
    }                                                                       \
}


/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_DATA( _infoPtr )             \
( ((_infoPtr)->rxBufInfo.readIdx >= (_infoPtr)->rxBufInfo.writeIdx) ?    \
      ((_infoPtr)->rxBufInfo.size - (_infoPtr)->rxBufInfo.readIdx) :     \
      ((_infoPtr)->rxBufInfo.writeIdx - (_infoPtr)->rxBufInfo.readIdx) )


/*============================================================================*/
#define BSP_UARTIO_RX_BUFFER_READ( _infoPtr, _nBytes )                        \
{                                                                             \
    if( (_nBytes) > 0 )                                                       \
    {                                                                         \
        BSP_MCU_CRITICAL_SECTION_ENTER();                                     \
        (_infoPtr)->rxBufInfo.readIdx += (_nBytes);                           \
        if( (_infoPtr)->rxBufInfo.readIdx == (_infoPtr)->rxBufInfo.size )     \
        {                                                                     \
            (_infoPtr)->rxBufInfo.readIdx = 0;                                \
        }                                                                     \
        if( (_infoPtr)->rxBufInfo.readIdx == (_infoPtr)->rxBufInfo.writeIdx ) \
        {                                                                     \
            (_infoPtr)->rxBufInfo.readIdx = (_infoPtr)->rxBufInfo.size;       \
        }                                                                     \
        BSP_MCU_CRITICAL_SECTION_EXIT();                                      \
    }                                                                         \
}


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_WRITE_PTR( _infoPtr )                 \
    &((_infoPtr)->txBufInfo.ptr[ (_infoPtr)->txBufInfo.writeIdx ])


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_READ_PTR( _infoPtr )                 \
    &((_infoPtr)->txBufInfo.ptr[ (_infoPtr)->txBufInfo.readIdx ])


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_NUM_CONTIGUOUS_AVAIL( _infoPtr )            \
( ((_infoPtr)->txBufInfo.writeIdx > (_infoPtr)->txBufInfo.readIdx) ?     \
      ((_infoPtr)->txBufInfo.size - (_infoPtr)->txBufInfo.writeIdx) :    \
      ((_infoPtr)->txBufInfo.readIdx - (_infoPtr)->txBufInfo.writeIdx) )


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_WRITE( _infoPtr, _nBytes )                     \
{                                                                           \
    if( (_nBytes) > 0 )                                                     \
    {                                                                       \
        BSP_MCU_CRITICAL_SECTION_ENTER();                                   \
        if( (_infoPtr)->txBufInfo.readIdx == (_infoPtr)->txBufInfo.size )   \
        {                                                                   \
            (_infoPtr)->txBufInfo.readIdx = (_infoPtr)->txBufInfo.writeIdx; \
        }                                                                   \
        (_infoPtr)->txBufInfo.writeIdx += (_nBytes);                        \
        if( (_infoPtr)->txBufInfo.writeIdx == (_infoPtr)->txBufInfo.size )  \
        {                                                                   \
            (_infoPtr)->txBufInfo.writeIdx = 0;                             \
        }                                                                   \
        BSP_MCU_CRITICAL_SECTION_EXIT();                                    \
    }                                                                       \
}


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_NUM_CONTIGUOUS_DATA( _infoPtr )             \
( ((_infoPtr)->txBufInfo.readIdx >= (_infoPtr)->txBufInfo.writeIdx) ?    \
      ((_infoPtr)->txBufInfo.size - (_infoPtr)->txBufInfo.readIdx) :     \
      ((_infoPtr)->txBufInfo.writeIdx - (_infoPtr)->txBufInfo.readIdx) )


/*============================================================================*/
#define BSP_UARTIO_TX_BUFFER_READ( _infoPtr, _nBytes )                        \
{                                                                             \
    if( (_nBytes) > 0 )                                                       \
    {                                                                         \
        BSP_MCU_CRITICAL_SECTION_ENTER();                                     \
        (_infoPtr)->txBufInfo.readIdx += (_nBytes);                           \
        if( (_infoPtr)->txBufInfo.readIdx == (_infoPtr)->txBufInfo.size )     \
        {                                                                     \
            (_infoPtr)->txBufInfo.readIdx = 0;                                \
        }                                                                     \
        if( (_infoPtr)->txBufInfo.readIdx == (_infoPtr)->txBufInfo.writeIdx ) \
        {                                                                     \
            (_infoPtr)->txBufInfo.readIdx = (_infoPtr)->txBufInfo.size;       \
        }                                                                     \
        BSP_MCU_CRITICAL_SECTION_EXIT();                                      \
    }                                                                         \
}


/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
typedef struct
{
    uint8_t* ptr;
    size_t   size;
    size_t   writeIdx;
    size_t   readIdx;
} bsp_UartIo_BufferInfo_t;

/*============================================================================*/
/*
 * Flag to track whether or not Rx is currently enabled
 * Flag to track whether or not Tx is currently enabled
 *
 * Global used to track the amount of data sent to the USART driver each time
 * data is sent. This is not a running count of the num bytes sent in the
 * previous call to the USART driver. This is needed so the serial component
 * knows how much to free from the tx buffer once the USART calls the callback
 * indicating that the transmit was completed.
 *
 * Global used to track the amount of tx data dropped due to buffer overflows
 *
 * Global to hold hte read callback function pointer.
 */
typedef struct
{
    bsp_Uart_Id_t                  uartId;
    bsp_Uart_PinSel_t              rxPinSel;
    bsp_Uart_PinSel_t              txPinSel;
    bsp_Uart_Baud_t                baud;
    bsp_UartIo_BufferInfo_t        rxBufInfo;
    bsp_UartIo_BufferInfo_t        txBufInfo;
    bool_t                         rxEnabled;
    bool_t                         txEnabled;
    uint32_t                       txDataCount;
    uint32_t                       txDataDroppedCount;
    bsp_UartIo_DataAvailCallback_t dataAvailCallback;
    FILE                           file;
} bsp_UartIo_InternalInfo_t;


/*==============================================================================
 *                                 Globals
 *============================================================================*/
/*============================================================================*/
uint8_t bsp_UartIo_rxBuffer0[ BSP_PLATFORM_IO_UART0_RX_BUF_LEN ];
uint8_t bsp_UartIo_txBuffer0[ BSP_PLATFORM_IO_UART0_TX_BUF_LEN ];

/*============================================================================*/
bsp_UartIo_InternalInfo_t bsp_UartIo_internalInfoTable[] = {
    { BSP_PLATFORM_IO_UART0_ID,
      BSP_PLATFORM_IO_UART0_RX_PIN_SEL,
      BSP_PLATFORM_IO_UART0_TX_PIN_SEL,
      BSP_PLATFORM_IO_UART0_BAUD,
      {bsp_UartIo_rxBuffer0, DIM(bsp_UartIo_rxBuffer0), 0, DIM(bsp_UartIo_rxBuffer0) },
      {bsp_UartIo_txBuffer0, DIM(bsp_UartIo_txBuffer0), 0, DIM(bsp_UartIo_txBuffer0) },
      FALSE, FALSE, 0, 0, NULL }
};

/*==============================================================================
 *                            Local Functions
 *============================================================================*/

/*============================================================================*/
/**
 * Copy data as we can from the USART driver into the buffer.
 */
static void bsp_UartIo_rxHandler( void* arg, void* dataPtr, size_t numRead );
static void
bsp_UartIo_rxHandler( void*  arg,
                      void*  dataPtr,
                      size_t numRead )
{
    size_t bufferSpace;
    size_t extraReadCnt;
    bsp_UartIo_InternalInfo_t* infoPtr;

    infoPtr = (bsp_UartIo_InternalInfo_t*)arg;

    /* Bump up the write index by the amount copied in by the USART driver */
    BSP_UARTIO_RX_BUFFER_WRITE( infoPtr, numRead );

    /* Check how much contiguous space is available */
    bufferSpace = BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_AVAIL( infoPtr );

    extraReadCnt = 1;

    /* Setup to get more data in the buffer */
    while( (bufferSpace != 0) && (extraReadCnt != 0) )
    {
        /* Read more data */
        extraReadCnt = bsp_Uart_rcv( infoPtr->uartId,
                                     BSP_UARTIO_RX_BUFFER_WRITE_PTR( infoPtr ),
                                     1,
                                     infoPtr,
                                     &bsp_UartIo_rxHandler );

        BSP_UARTIO_RX_BUFFER_WRITE( infoPtr, extraReadCnt );

        bufferSpace = BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_AVAIL( infoPtr );
    }

    /* If there's no more buffer space then set the flag to indicate that
     * the UART has no pending rx transactions
     */
    if( bufferSpace == 0 )
    {
        /* Once some buffer space becomes available then it will be re enabled */
        infoPtr->rxEnabled = FALSE;
    }

    if( infoPtr->dataAvailCallback != NULL )
    {
        infoPtr->dataAvailCallback();
    }

    return;
}

/*============================================================================*/
/**
 * The callback continues forwarding data from the serial buffer into the USART
 * driver if there is any. If not then it just sets a global and returns.
 */
static void bsp_UartIo_txDoneHandler( void* arg );
static void
bsp_UartIo_txDoneHandler( void* arg )
{
    size_t                     contigDataLen;
    bsp_UartIo_InternalInfo_t* infoPtr;

    infoPtr = (bsp_UartIo_InternalInfo_t*)arg;

    /* Assume everything we requested to send was sent before
     * this callback was called.
     */
    BSP_UARTIO_TX_BUFFER_READ( infoPtr, infoPtr->txDataCount );

    contigDataLen = BSP_UARTIO_TX_BUFFER_NUM_CONTIGUOUS_DATA( infoPtr );

    if( contigDataLen == 0 )
    {
        /* There's nothing else to send, so signal the client. */
        infoPtr->txEnabled = FALSE;
    }
    else
    {
        /* Save off the number of bytes that we expect to be
         * sent before the next callback.
         */
        infoPtr->txDataCount = contigDataLen;

        /* Send the data. */
        bsp_Uart_snd( infoPtr->uartId,
                      BSP_UARTIO_TX_BUFFER_READ_PTR( infoPtr ),
                      infoPtr->txDataCount,
                      (void*)infoPtr,
                      &bsp_UartIo_txDoneHandler );
    }

    return;
}



/*============================================================================*/
static size_t
bsp_UartIo_bufferWrite( bsp_UartIo_InternalInfo_t* infoPtr,
                        void*                      dataPtr,
                        size_t                     cnt )
{
    size_t  numAvail;
    size_t  numBuffered;
    size_t  dataInBuffer;
    bool_t  bufferOverflow;
    bool_t  triggerTx;

    BSP_ASSERT( (dataPtr != NULL) && (cnt > 0) );

    numAvail = 0;
    numBuffered = 0;
    bufferOverflow = FALSE;

    /* Copy the data from the client pointer into the buffer. If there is
     * not enough room then some data will be dropped.
     * Note: If the Tx is enabled while filling the buffer with new data then
     *       that new data will go out with the previous transmit and a new
     *       call to the USART driver is not necessary.
     */
    while( (numBuffered < cnt) && (bufferOverflow == FALSE) )
    {
        BSP_MCU_CRITICAL_SECTION_ENTER();
        numAvail = BSP_UARTIO_TX_BUFFER_NUM_CONTIGUOUS_AVAIL( infoPtr );

        if( numAvail == 0 )
        {
            /* Drop the data that doesn't fit and exit */
            infoPtr->txDataDroppedCount += (cnt - numBuffered);
            bufferOverflow = TRUE;
        }
        else
        {
            /* Need to copy client data into the serial buffer in a contiguous
             * section. If client is requesting to send more data than is
             * available at the end of the circular serial buffer then copy in
             * as much as will fit at the end and loop through again.
             */
            numAvail = (numAvail > (cnt-numBuffered)) ? (cnt-numBuffered) : numAvail;

            /* Copy to the write buffer */
            memcpy( BSP_UARTIO_TX_BUFFER_WRITE_PTR( infoPtr ),
                    &(((uint8_t *)dataPtr)[numBuffered]),
                    numAvail );

            /* Update the write index */
            BSP_UARTIO_TX_BUFFER_WRITE( infoPtr, numAvail );
            numBuffered += numAvail;
        }
        BSP_MCU_CRITICAL_SECTION_EXIT();
    }

    /* check to see if a tx needs to be triggered. It's possible that a tx was
     * occuring while the buffer was being filled in which case the data that
     * was just put into the buffer may have been sent out.
     */
    triggerTx = FALSE;
    BSP_MCU_CRITICAL_SECTION_ENTER();
    dataInBuffer = BSP_UARTIO_TX_BUFFER_NUM_CONTIGUOUS_DATA( infoPtr );
    if( (dataInBuffer > 0) && (infoPtr->txEnabled == FALSE) )
    {
        /* There's data to be sent and tx is not enabled so trigger a tx */
        triggerTx = TRUE;
    }
    BSP_MCU_CRITICAL_SECTION_EXIT();

    if( triggerTx == TRUE )
    {
        infoPtr->txEnabled = TRUE;
        infoPtr->txDataCount = dataInBuffer;

        /* Send the data. */
        bsp_Uart_snd( infoPtr->uartId,
                      BSP_UARTIO_TX_BUFFER_READ_PTR( infoPtr ),
                      infoPtr->txDataCount,
                      (void*)infoPtr,
                      &bsp_UartIo_txDoneHandler );
    }

    return( numBuffered );
}


/*============================================================================*/
static size_t
bsp_UartIo_bufferRead( bsp_UartIo_InternalInfo_t* infoPtr,
                       void*                      dataPtr,
                       size_t                     cnt )
{
    size_t  numRead;
    size_t  bufCount;
    size_t  bufferSpace;
    bool_t  bufEmpty;
    uint8_t dataCnt;

    BSP_ASSERT( (dataPtr != NULL) && (cnt > 0) );

    numRead = 0;
    bufEmpty = FALSE;

    /* Copy as much of the requested data from the serial buffer into the
     * client buffer
     */
    while( (numRead < cnt) && (bufEmpty == FALSE) )
    {
        bufCount = BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_DATA( infoPtr );

        if( bufCount > 0 )
        {
            /* Copy the data from the rx buffer into the client buffer */
            bufCount = ((bufCount+numRead) > cnt) ? (cnt-numRead) : bufCount;

            memcpy( &(((uint8_t *)dataPtr)[numRead]),
                    BSP_UARTIO_RX_BUFFER_READ_PTR( infoPtr ),
                    bufCount );

            BSP_UARTIO_RX_BUFFER_READ( infoPtr, bufCount );
            numRead += bufCount;
        }
        else
        {
            bufEmpty = TRUE;
        }
    }

    /* Data was copied out of the serial buffer so there should be some
     * space available in the serial buffer. Double check that there's still
     * space with interrupts disabled and whether or not the receiver is
     * enabled. If there's space and the receiver isn't enabled then read from
     * the UART.
     * Note: Interrupts must be disabled because the serial buffer is written
     *       to and the UART read operation is setup from interrupt context
     *       in the Serial_rxCallback.
     */
    BSP_MCU_CRITICAL_SECTION_ENTER();

    bufferSpace = BSP_UARTIO_RX_BUFFER_NUM_CONTIGUOUS_AVAIL( infoPtr );

    if( (infoPtr->rxEnabled == FALSE) && (bufferSpace > 0) )
    {
        /* Read more data */
        dataCnt = bsp_Uart_rcv( infoPtr->uartId,
                                BSP_UARTIO_RX_BUFFER_WRITE_PTR( infoPtr ),
                                1,
                                (void*)infoPtr,
                                &bsp_UartIo_rxHandler );

        if( dataCnt != 0 )
        {
            /* The UART had data waiting so now the RX buffer is definitely
             * not empty. The UART is not enabled to receive because only
             * 1 byte was requested which means there is no outstanding
             * transaction in the UART.
             */
            BSP_UARTIO_RX_BUFFER_WRITE( infoPtr, dataCnt );
        }
        else
        {
            /* There was no data sitting in the UART so the rx is enabled
             * waiting for more data to fill the buffer. The Serial_rxCallback
             * will be called once data arrives in the UART
             */
            infoPtr->rxEnabled = TRUE;
        }
    }

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return( numRead );
}


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_UartIo_init( void )
{
    uint8_t i;
    char filenameBuf[6] = "uartX";

    for( i=0; i<DIM(bsp_UartIo_internalInfoTable); i++ )
    {
        /* Initialize the buffer info */
        bsp_UartIo_internalInfoTable[i].rxBufInfo.writeIdx = 0;
        bsp_UartIo_internalInfoTable[i].rxBufInfo.readIdx  = bsp_UartIo_internalInfoTable[i].rxBufInfo.size;
        bsp_UartIo_internalInfoTable[i].txBufInfo.writeIdx = 0;
        bsp_UartIo_internalInfoTable[i].txBufInfo.readIdx  = bsp_UartIo_internalInfoTable[i].txBufInfo.size;

        bsp_UartIo_internalInfoTable[i].txDataDroppedCount = 0;
        bsp_UartIo_internalInfoTable[i].txDataCount        = 0;
        bsp_UartIo_internalInfoTable[i].dataAvailCallback  = NULL;

        bsp_UartIo_internalInfoTable[i].txEnabled = false;
        bsp_UartIo_internalInfoTable[i].rxEnabled = true; /* Rx enabled on call to uartRcv */

        memset( bsp_UartIo_internalInfoTable[i].rxBufInfo.ptr, 0, sizeof(bsp_UartIo_internalInfoTable[i].rxBufInfo.size) );
        memset( bsp_UartIo_internalInfoTable[i].txBufInfo.ptr, 0, sizeof(bsp_UartIo_internalInfoTable[i].txBufInfo.size) );

        /* Setup physical UART to use */
        bsp_Uart_control( bsp_UartIo_internalInfoTable[i].uartId,
                          bsp_UartIo_internalInfoTable[i].rxPinSel,
                          bsp_UartIo_internalInfoTable[i].txPinSel,
                          BSP_UART_PIN_SEL_NONE, //RTS is not used
                          BSP_UART_PIN_SEL_NONE ); //CTS is not used

        bsp_Uart_config( bsp_UartIo_internalInfoTable[i].uartId,
                         bsp_UartIo_internalInfoTable[i].baud,
                         BSP_UART_PARITY_NONE,
                         BSP_UART_STOP_BIT_1,
                         BSP_UART_DATA_BIT_8,
                         BSP_UART_FLOW_NONE );

        snprintf( filenameBuf, sizeof(filenameBuf), "uart%d", bsp_UartIo_internalInfoTable[i].uartId );

        /* Add IO device to be used in stdio */
        add_device( filenameBuf,
                    _MSA, /* Single stream open at a time (_MSA is for multiple) */
                    bsp_UartIo_open,
                    bsp_UartIo_close,
                    bsp_UartIo_read,
                    bsp_UartIo_write,
                    bsp_UartIo_lseek,
                    bsp_UartIo_unlink,
                    bsp_UartIo_rename );

        bsp_UartIo_internalInfoTable[i].file.fd        = (int)&(bsp_UartIo_internalInfoTable[i]);
        bsp_UartIo_internalInfoTable[i].file.pos       = NULL;
        bsp_UartIo_internalInfoTable[i].file.bufend    = NULL;
        bsp_UartIo_internalInfoTable[i].file.buff_stop = NULL;
        bsp_UartIo_internalInfoTable[i].file.flags     = 0;

        /* Open the file for reading/writing */
        fopen( filenameBuf, "rw" );

        /* Ready to receive */
        bsp_Uart_rcv( bsp_UartIo_internalInfoTable[i].uartId,
                      BSP_UARTIO_RX_BUFFER_WRITE_PTR( &bsp_UartIo_internalInfoTable[i] ),
                      1,
                      (void*)&bsp_UartIo_internalInfoTable[i],
                      &bsp_UartIo_rxHandler );
    }

    return;
}


/*============================================================================*/
void
bsp_UartIo_registerDataAvailableCallback( int                            file_descriptor,
                                          bsp_UartIo_DataAvailCallback_t callback )
{
    ((bsp_UartIo_InternalInfo_t*)file_descriptor)->dataAvailCallback = callback;
    return;
}


/*============================================================================*/
int
bsp_UartIo_open( const char*  path,
                 unsigned int flags,
                 int          llv_fd )
{
    uint8_t pathLen;
    int ret;

    pathLen = strlen(path);

    /* Todo: Do something smarter here */
    if( path[(pathLen-1)] == '0' )
    {
        ret = (int)&(bsp_UartIo_internalInfoTable[0]);
    }
    else
    {
        ret = (int)&(bsp_UartIo_internalInfoTable[1]);
    }

    return( ret );
}


/*============================================================================*/
int
bsp_UartIo_close( int file_descriptor )
{
    return( 0 );
}


/*============================================================================*/
int
bsp_UartIo_read( int    file_descriptor,
                 char*  buffer,
                 size_t count )
{
    return( (int)(bsp_UartIo_bufferRead( (bsp_UartIo_InternalInfo_t*)file_descriptor, buffer, count )) );
}


/*============================================================================*/
int
bsp_UartIo_write( int         file_descriptor,
                  const char* buffer,
                  size_t      count )
{
    return( (int)(bsp_UartIo_bufferWrite( (bsp_UartIo_InternalInfo_t*)file_descriptor, (void *)buffer, count )) );
}


/*============================================================================*/
off_t
bsp_UartIo_lseek( int   file_descriptor,
                  off_t offset,
                  int   origin )
{
    return( (off_t)-1 );
}


/*============================================================================*/
int
bsp_UartIo_unlink( const char* path )
{
    return( -1 );
}


/*============================================================================*/
int
bsp_UartIo_rename( const char* old_name,
                   const char* new_name )
{
    return( -1 );
}

