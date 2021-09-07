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
 * @file svc_CmdEh.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_UartIo.h"
#include "bsp_Mcu.h"
#include "bsp_Io.h"
#include "svc_CmdEh.h"
#include "svc_SerIoEh.h"
#include "osapi.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "svc_TestEh.h"

// Menu is defined in tst directory
#include "tst.h"

#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#include "svc_Log.h"

/*==============================================================================
 *                                  Defines
 *============================================================================*/
#define SVC_CMDEH_BUFFER_SIZE    64

// The command terminal supports up arrow for command history
#define SVC_CMDEH_TXT_UP_ARROW "\x1B\x5B\x41"

// Define a header to dump out the serial console at boot
#define SVC_CMDEH_TXT_HDR    "\nCopyright 2021 Brian Costabile\n"\
                             "Test Command Console\n%c"

// Define a single character Prompt
#define SVC_CMDEH_TXT_PROMPT '>'

// Depending on what the serial terminal program sends in for enter/return
// this may need to change
#define SVC_CMDEH_TXT_END    '\n'

// If using the binary command interface, then the commands must start
// with this character.
#define SVC_CMDEH_BIN_START  '$'

// Define the number of command line buffers to keep. In reality, there's one
// less than this value because once a command is triggered, the next command
// buffer is reset.
#define SVC_CMDEH_BUFFER_HISTORY 2

// Some macros to manage the command buffer(s)
#define SVC_CMDEH_BUFFER_IDX svc_CmdEh_bufferIdxArray[svc_CmdEh_bufferSel]
#define SVC_CMDEH_BUFFER     svc_CmdEh_bufferArray[svc_CmdEh_bufferSel]

#define SVC_CMDEH_BUFFER_ADD_CHAR(_ch)                                        \
{                                                                             \
    svc_CmdEh_bufferArray[svc_CmdEh_bufferSel][SVC_CMDEH_BUFFER_IDX] = (_ch); \
    SVC_CMDEH_BUFFER_IDX++;                                                   \
    svc_CmdEh_bufferArray[svc_CmdEh_bufferSel][SVC_CMDEH_BUFFER_IDX] = 0;     \
}

#define SVC_CMDEH_BUFFER_REMOVE_CHAR()                                    \
{                                                                         \
    SVC_CMDEH_BUFFER_IDX--;                                               \
    svc_CmdEh_bufferArray[svc_CmdEh_bufferSel][SVC_CMDEH_BUFFER_IDX] = 0; \
}

#define SVC_CMDEH_BUFFER_RESET()                                          \
{                                                                         \
    SVC_CMDEH_BUFFER_IDX = 0;                                             \
    svc_CmdEh_bufferArray[svc_CmdEh_bufferSel][SVC_CMDEH_BUFFER_IDX] = 0; \
}

#define SVC_CMDEH_BIN_LEN_DIGITS 2
#define SVC_CMDEH_BIN_LEN_MAX    (SVC_CMDEH_BUFFER_SIZE * SVC_CMDEH_BUFFER_HISTORY)
#define SVC_CMDEH_ARGC_MAX       (SVC_CMDEH_BUFFER_SIZE / 2)


/*============================================================================*/
uint16_t svc_CmdEh_bufferIdxArray[SVC_CMDEH_BUFFER_HISTORY];
uint8_t  svc_CmdEh_bufferArray[SVC_CMDEH_BUFFER_HISTORY][SVC_CMDEH_BUFFER_SIZE];
uint8_t  svc_CmdEh_bufferSel;


/*============================================================================*/
const svc_MsgFwk_MsgId_t svc_CmdEh_bcastMsgIds[] =
{
    SVC_SERIOEH_DATA_IND
};


/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_Cmd_dumpHelp( const tst_TableElement_t* tbl_ptr )
{
    while( tbl_ptr->handler != NULL )
    {
        TST_PRINT_HELP( tbl_ptr );
        tbl_ptr++;
    }
}

/*============================================================================*/
tst_Status_t
svc_CmdEh_handlerText( int argc, char** argv )
{
    uint8_t                   argi;
    const tst_TableElement_t* tbl_ptr;
    int32_t                   ret = TST_STATUS_ERROR;

    tbl_ptr = tst_MenuTable;

    for( argi= 0; argi<argc; argi++ )
    {
        // Check for help first
        if( *argv[argi] == '?' )
        {
            // Dump list of commands at this level and exit
            svc_Cmd_dumpHelp( tbl_ptr );
            break;
        }

        // Find command in the current menu table
        while( tbl_ptr->handler != NULL )
        {
            if( strcmp( tbl_ptr->cmd, argv[argi] ) == 0 )
            {
                if( tbl_ptr->is_menu == true )
                {
                    tbl_ptr = tbl_ptr->submenu;

                    // If there are no arguments to pass to a handler
                    // then save as prepended command
                    if( argi == (argc - 1) )
                    {

                    }
                }
                else
                {
                    // Call function with remaining arguments
                    argi++;
                    ret = tbl_ptr->handler( (argc - argi), &argv[argi] );
                    argi = argc; // force exit the outer for loop
                }
                break;
            }
            tbl_ptr++;
        }
    }

    return( ret );
}


/*============================================================================*/
static void
svc_CmdEh_processBinary( uint8_t* buf, uint16_t len )
{
    uint16_t i;
    SVC_LOG_INFO( "Binary Command: len:%d Data:", len );

    for( i=0; i<len; i++ )
    {
        SVC_LOG_INFO( "%02X ", buf[i] );
    }

    SVC_LOG_INFO(NL);

    extern int32_t svc_CmdEh_handlerBinary( uint8_t* buf, uint16_t len );
    svc_CmdEh_handlerBinary( buf, len );
    return;
}


/*============================================================================*/
static void
svc_CmdEh_processText( uint8_t* buf, uint8_t len )
{
    char* argv[SVC_CMDEH_ARGC_MAX];
    int argc = 0;
    char* ptr = (char*)&buf[0];

    // Tokenize command string on whitespaces
    while( ptr < (char*)&buf[len] )
    {
        while( isspace(((int)(*ptr))) && (ptr < (char*)&buf[len]) ) { *ptr++ = '\0'; } // NULL out whitespaces
        if( ptr < (char*)&buf[len] ) { argv[argc++] = ptr++; }
        while( !isspace(((int)(*ptr))) && (ptr < (char*)&buf[len]) ) { ptr++; } // Skip non-whitespaces
    }

    svc_CmdEh_handlerText( argc, argv );
    return;
}


/*============================================================================*/
static void
svc_CmdEh_bufferInit( void )
{
    svc_CmdEh_bufferSel = 0;
    memset(svc_CmdEh_bufferIdxArray, 0, sizeof(svc_CmdEh_bufferIdxArray));
    memset(svc_CmdEh_bufferArray, 0, sizeof(svc_CmdEh_bufferArray));
    return;
}


/*============================================================================*/
int svc_CmdEh_asciiToDec( char c )
{
    if     ( (c >= '0') && (c <= '9') ) { return( (c - '0') ); }
    else if( (c >= 'a') && (c <= 'f') ) { return( (c - 'a') + 10 ); }
    else if( (c >= 'A') && (c <= 'F') ) { return( (c - 'A') + 10 ); }
    return 0;
}


/*============================================================================*/
static void
svc_CmdEh_bufferAddChar( char c )
{
    static bool binary_mode = false;
    static bool binary_len_done = false;
    static int16_t binary_digits = 0;
    static int16_t binary_len = 0;
    static uint8_t binary_data = 0;
    static uint8_t esc_buf[3];
    static uint8_t esc_cnt = 0;
    bool cmd_done = false;

    // If a command starts with a BIN_START then the rest of the command buffer
    // is filled with a binary command and ends
    if( (binary_mode == false) && (SVC_CMDEH_BUFFER_IDX == 0) && (esc_cnt == 0) )
    {
        binary_mode = (c == SVC_CMDEH_BIN_START);
        binary_len = 0;
        binary_len_done = false;
        binary_digits = SVC_CMDEH_BIN_LEN_DIGITS;
        esc_cnt = 0;

        // If running in binary mode, Forget history and make use of all
        // buffers as a single binary command buffer
        if( binary_mode )
        {
            svc_CmdEh_bufferSel = 0;
            SVC_CMDEH_BUFFER_RESET();
            return; // No need to add char to buffer
        }
    }

    // Establish the length of the binary command
    if( (binary_mode == true) && (binary_len_done == false) )
    {
        binary_len *= 16;
        binary_len += svc_CmdEh_asciiToDec( c );
        if( --binary_digits <= 0 )
        {
            binary_len_done = true;
            binary_digits = (binary_len * 2);
            binary_data = 0;
        }
        return;
    }

    // Continue adding to the buffer so long as there's space.
    // Text commands end with NL or end of buffer with NULL
    // Binary commands end after expected length or end of buffer
    //   Binary length is number of bytes after the length byte
    if( binary_mode == true )
    {
        binary_data *= 16;
        binary_data += svc_CmdEh_asciiToDec( c );
        binary_digits--;
        // Every digit is a nibble so every other digit gets stored as a byte
        if( (binary_digits & 0x01) == 0 )
        {
            SVC_CMDEH_BUFFER_ADD_CHAR( (char)binary_data );
            binary_data = 0;
        }
        cmd_done = (binary_digits <= 0);
    }
    else
    {
        if( esc_cnt > 0 )
        {
            esc_buf[( sizeof(esc_buf) - esc_cnt )] = c;
            esc_cnt--;

            // Process escape
            if( esc_cnt == 0 )
            {
                if( memcmp( esc_buf, SVC_CMDEH_TXT_UP_ARROW, sizeof(esc_buf) ) == 0 ) // Up Arrow
                {
                    if( SVC_CMDEH_BUFFER_IDX > 0 )
                    {
                        // using carriage return will put the cursor at the prompt
                        putc( '\r', stdout );
                        printf( "%*c", (SVC_CMDEH_BUFFER_IDX + 1), ' ' );
                        putc( '\r', stdout );

                        // Restore the prompt
                        putc( SVC_CMDEH_TXT_PROMPT, stdout );
                    }
                    svc_CmdEh_bufferSel = ((svc_CmdEh_bufferSel + 1) % SVC_CMDEH_BUFFER_HISTORY);

                    // reconstruct the command string by replacing null characters with spaces
                    if( SVC_CMDEH_BUFFER_IDX > 0 )
                    {
                        for( uint8_t i=0; i<SVC_CMDEH_BUFFER_IDX; i++ )
                        {
                            if( SVC_CMDEH_BUFFER[i] == 0 )
                            {
                                SVC_CMDEH_BUFFER[i] = ' ';
                            }
                            putc( SVC_CMDEH_BUFFER[i], stdout );
                        }
                    }
                }
            }
        }
        else
        {
            if( c == '\x1B' ) // Start of an escape sequence
            {
                esc_buf[0] = c;
                esc_cnt = (sizeof(esc_buf) - 1);
            }
            else if( (c == '\b') || (c == '\x7F') ) // Backspace or delete
            {
                // Clean up the screen
                if( SVC_CMDEH_BUFFER_IDX >= 1 )
                {
                    printf( "\b \b" );
                    SVC_CMDEH_BUFFER_IDX--;
                }
            }
            else if( isprint((int)c) )
            {
                SVC_CMDEH_BUFFER_ADD_CHAR( c );
                putc(c, stdout);
            }
            else if ( c == SVC_CMDEH_TXT_END )
            {
                putc(c, stdout);
            }
            cmd_done = ((c == SVC_CMDEH_TXT_END) || (SVC_CMDEH_BUFFER_IDX >= (SVC_CMDEH_BUFFER_SIZE - 1)));
        }
    }

    // Process complete commands
    if( cmd_done == true )
    {
        if( binary_mode == true )
        {
            svc_CmdEh_processBinary( SVC_CMDEH_BUFFER, (uint16_t)SVC_CMDEH_BUFFER_IDX );
            SVC_CMDEH_BUFFER_RESET();
            binary_mode = false;
        }
        else
        {
            svc_CmdEh_processText( SVC_CMDEH_BUFFER, (uint8_t)SVC_CMDEH_BUFFER_IDX );
            putc( SVC_CMDEH_TXT_PROMPT, stdout );
        }
        svc_CmdEh_bufferSel = ((svc_CmdEh_bufferSel + 1) % SVC_CMDEH_BUFFER_HISTORY);
        SVC_CMDEH_BUFFER_RESET();
    }
    return;
}

bool svc_CmdEh_ioCallbackSuppressed = false;
/*============================================================================*/
static void
svc_CmdEh_ioCallback( void )
{
    // Message sending is supressed until input processing has completed
    if( svc_CmdEh_ioCallbackSuppressed == false )
    {
        svc_CmdEh_ioCallbackSuppressed = true;
        svc_MsgFwk_msgAllocAndBroadcast( SVC_SERIOEH_DATA_IND,
                                        sizeof(svc_SerIoEh_DataInd_t),
                                        NULL );
    }

    return;
}


/*============================================================================*/
static void
svc_CmdEh_init( void )
{
    svc_CmdEh_ioCallbackSuppressed = false;
    svc_CmdEh_bufferInit();

    // Grab the file descriptor for the serial device and register a callback
    // to be informed when data is received on stdin
    int fd = bsp_Io_fileToDfd( stdin );

#if (PLATFORM_IO == uart)
    bsp_UartIo_registerDataAvailableCallback( fd, svc_CmdEh_ioCallback );
#elif (PLATFORM_IO == usb)
    bsp_UsbIo_registerDataAvailableCallback( fd, svc_CmdEh_ioCallback );
#endif

    // Print out the command console header and first prompt
    printf( SVC_CMDEH_TXT_HDR, SVC_CMDEH_TXT_PROMPT );
}


/*============================================================================*/
static void
svc_CmdEh_msgHandler( svc_MsgFwk_Hdr_t* msgPtr )
{
    switch( msgPtr->id )
    {
        case SVC_SERIOEH_DATA_IND:
        {
            int c = EOF;
            clearerr( stdin );
            c = getc( stdin );
            while( c != EOF )
            {
                svc_CmdEh_bufferAddChar( (char)c );
                c = getc( stdin );
            }
            BSP_MCU_CRITICAL_SECTION_ENTER();
            svc_CmdEh_ioCallbackSuppressed = false;
            BSP_MCU_CRITICAL_SECTION_EXIT();
        }
        break;
    }
}

/*==============================================================================
 *                                 Globals
 *============================================================================*/
/*============================================================================*/
const svc_Eh_Info_t svc_CmdEh_info =
{
    SVC_EHID_CMD,
    DIM(svc_CmdEh_bcastMsgIds),
    svc_CmdEh_bcastMsgIds,
    svc_CmdEh_init,
    svc_CmdEh_msgHandler
};
