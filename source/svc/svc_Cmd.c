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
 * @file svc_Cmd.c
 * @brief
 */

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_UartIo.h"
#include "bsp_Mcu.h"
#include "bsp_Io.h"
#include "svc_Cmd.h"
#include "osapi.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

// Fix reverse dependency. use section for binary and text handler auto-registration
#include "tst.h"

#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#include "svc_Log.h"

/*==============================================================================
 *                                  Defines
 *============================================================================*/
#define SVC_CMD_STACK_SIZE    2048
#define SVC_CMD_STACK_SIZE_32 (SVC_CMD_STACK_SIZE / 4)
#define SVC_CMD_BUFFER_SIZE   64
#define SVC_CMD_BIN_HDR_LEN   2
#define SVC_CMD_BIN_MAX_LEN   (SVC_CMD_BUFFER_SIZE - SVC_CMD_BIN_HDR_LEN)
#define SVC_CMD_ARGC_MAX      (SVC_CMD_BUFFER_SIZE / 2)

// The command terminal supports up arrow for command history
#define SVC_CMD_TXT_UP_ARROW "\x1B\x5B\x41"

// Define a header to dump out the serial console at boot
#define SVC_CMD_TXT_HDR    "\nCopyright 2017 Brian Costabile\n"\
                           "Test Command Console\n%c"

// Define a single character Prompt
#define SVC_CMD_TXT_PROMPT '>'

// Depending on what the serial terminal program sends in for enter/return
// this may need to change
#define SVC_CMD_TXT_END    '\n'

// If using the binary command interface, then the commands must start
// with this character.
#define SVC_CMD_BIN_START  '$'

// Define the number of command line buffers to keep. In reality, there's one
// less than this value because once a command is triggered, the next command
// buffer is reset.
#define SVC_CMD_BUFFER_HISTORY 2

// Some macros to manage the command buffer(s)
#define SVC_CMD_BUFFER_IDX svc_Cmd_bufferIdxArray[svc_Cmd_bufferSel]
#define SVC_CMD_BUFFER     svc_Cmd_bufferArray[svc_Cmd_bufferSel]

#define SVC_CMD_BUFFER_ADD_CHAR(_ch)                                    \
{                                                                       \
    svc_Cmd_bufferArray[svc_Cmd_bufferSel][SVC_CMD_BUFFER_IDX] = (_ch); \
    SVC_CMD_BUFFER_IDX++;                                               \
    svc_Cmd_bufferArray[svc_Cmd_bufferSel][SVC_CMD_BUFFER_IDX] = 0;     \
}

#define SVC_CMD_BUFFER_REMOVE_CHAR()                                \
{                                                                   \
    SVC_CMD_BUFFER_IDX--;                                           \
    svc_Cmd_bufferArray[svc_Cmd_bufferSel][SVC_CMD_BUFFER_IDX] = 0; \
}

#define SVC_CMD_BUFFER_RESET()                                      \
{                                                                   \
    SVC_CMD_BUFFER_IDX = 0;                                         \
    svc_Cmd_bufferArray[svc_Cmd_bufferSel][SVC_CMD_BUFFER_IDX] = 0; \
}

/*==============================================================================
 *                                 Globals
 *============================================================================*/

// Total stack needed for the console thread
uint32_t svc_Cmd_stack[SVC_CMD_STACK_SIZE_32];

/*============================================================================*/
osapi_Semaphore_t sem;

/*============================================================================*/
uint16_t svc_Cmd_bufferIdxArray[SVC_CMD_BUFFER_HISTORY];
uint8_t  svc_Cmd_bufferArray[SVC_CMD_BUFFER_HISTORY][SVC_CMD_BUFFER_SIZE];
uint8_t  svc_Cmd_bufferSel;

/*==============================================================================
 *                            Local Functions
 *============================================================================*/
/*============================================================================*/
static void
svc_Cmd_processBinary( uint8_t* buf, uint16_t len )
{
    uint16_t i;
    SVC_LOG_INFO( "Binary Command: len:%d"NL, len );

    for( i=0; i<len; i++ )
    {
        SVC_LOG_INFO( "%02X ", buf[i] );
    }

    SVC_LOG_INFO(NL);

    svc_Cmd_handlerBinary( buf, len );
    return;
}


/*============================================================================*/
static void
svc_Cmd_processText( uint8_t* buf, uint8_t len )
{
    char* argv[SVC_CMD_ARGC_MAX];
    int argc = 0;
    char* ptr = (char*)&buf[0];

    // Tokenize command string on whitespaces
    while( ptr < (char*)&buf[len] )
    {
        while( isspace(((int)(*ptr))) && (ptr < (char*)&buf[len]) ) { *ptr++ = '\0'; } // NULL out whitespaces
        if( ptr < (char*)&buf[len] ) { argv[argc++] = ptr++; }
        while( !isspace(((int)(*ptr))) && (ptr < (char*)&buf[len]) ) { ptr++; } // Skip non-whitespaces
    }

    svc_Cmd_handlerText( argc, argv );
    return;
}


/*============================================================================*/
static void
svc_Cmd_bufferInit( void )
{
    svc_Cmd_bufferSel = 0;
    memset(svc_Cmd_bufferIdxArray, 0, sizeof(svc_Cmd_bufferIdxArray));
    memset(svc_Cmd_bufferArray, 0, sizeof(svc_Cmd_bufferArray));
    return;
}


/*============================================================================*/
static void
svc_Cmd_bufferAddChar( char c )
{
    static bool binary_mode = false;
    static int16_t binary_len = 0;
    static uint8_t esc_buf[3];
    static uint8_t esc_cnt = 0;
    bool cmd_done = false;

    // If a command starts with a BIN_START then the rest of the command buffer
    // is filled with a binary command and ends
    if( (SVC_CMD_BUFFER_IDX == 0) && (esc_cnt == 0) )
    {
        binary_mode = (c == SVC_CMD_BIN_START);
        binary_len = -1;
        esc_cnt = 0;

        // If running in binary mode, Forget history and make use of all
        // buffers as a single binary command buffer
        if( binary_mode )
        {
            svc_Cmd_bufferSel = 0;
            SVC_CMD_BUFFER_RESET();
        }
    }

    // Establish the length of the binary command
    if( (SVC_CMD_BUFFER_IDX == 1) && binary_mode )
    {
        binary_len = (c + SVC_CMD_BIN_HDR_LEN); // add in "$X" where X is the length byte
        binary_len = (binary_len > SVC_CMD_BUFFER_SIZE) ? SVC_CMD_BUFFER_SIZE : binary_len;
    }


    // Continue adding to the buffer so long as there's space.
    // Text commands end with NL or end of buffer with NULL
    // Binary commands end after expected length or end of buffer
    //   Binary length is number of bytes after the length byte
    if( binary_mode == true )
    {
        SVC_CMD_BUFFER_ADD_CHAR( c );
        cmd_done = ((binary_len > 0) && (SVC_CMD_BUFFER_IDX >= binary_len));
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
                if( memcmp( esc_buf, SVC_CMD_TXT_UP_ARROW, sizeof(esc_buf) ) == 0 ) // Up Arrow
                {
                    if( SVC_CMD_BUFFER_IDX > 0 )
                    {
                        // using carriage return will put the cursor at the prompt
                        putc( '\r', stdout );
                        printf( "%*c", (SVC_CMD_BUFFER_IDX + 1), ' ' );
                        putc( '\r', stdout );

                        // Restore the prompt
                        putc( SVC_CMD_TXT_PROMPT, stdout );
                    }
                    svc_Cmd_bufferSel = ((svc_Cmd_bufferSel + 1) % SVC_CMD_BUFFER_HISTORY);

                    // reconstruct the command string by replacing null characters with spaces
                    if( SVC_CMD_BUFFER_IDX > 0 )
                    {
                        for( uint8_t i=0; i<SVC_CMD_BUFFER_IDX; i++ )
                        {
                            if( SVC_CMD_BUFFER[i] == 0 )
                            {
                                SVC_CMD_BUFFER[i] = ' ';
                            }
                            putc( SVC_CMD_BUFFER[i], stdout );
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
                if( SVC_CMD_BUFFER_IDX >= 1 )
                {
                    printf( "\b \b" );
                    SVC_CMD_BUFFER_IDX--;
                }
            }
            else if( isprint(c) )
            {
                SVC_CMD_BUFFER_ADD_CHAR( c );
                putc(c, stdout);
            }
            else if ( c == SVC_CMD_TXT_END )
            {
                putc(c, stdout);
            }
            cmd_done = ((c == SVC_CMD_TXT_END) || (SVC_CMD_BUFFER_IDX >= (SVC_CMD_BUFFER_SIZE - 1)));
        }
    }

    // Process complete commands
    if( cmd_done == true )
    {
        if( binary_mode == true )
        {
            svc_Cmd_processBinary( SVC_CMD_BUFFER, (uint16_t)SVC_CMD_BUFFER_IDX );
            SVC_CMD_BUFFER_RESET();
        }
        else
        {
            svc_Cmd_processText( SVC_CMD_BUFFER, (uint8_t)SVC_CMD_BUFFER_IDX );
            putc( SVC_CMD_TXT_PROMPT, stdout );
        }
        svc_Cmd_bufferSel = ((svc_Cmd_bufferSel + 1) % SVC_CMD_BUFFER_HISTORY);
        SVC_CMD_BUFFER_RESET();
    }
    return;
}


/*============================================================================*/
static void
svc_Cmd_ioCallback( void )
{
    osapi_Semaphore_give( sem );
    return;
}


/*============================================================================*/
static void
svc_Cmd_threadMain( osapi_ThreadArg_t arg )
{
    int fd;
    int c = EOF;

    svc_Cmd_bufferInit();

    // Grab the file descriptor for the serial device and register a callback
    // to be informed when data is received on stdin
    fd = bsp_Io_fileToDfd( stdin );

    sem = osapi_Semaphore_create();

#if (PLATFORM_IO == uart)
    bsp_UartIo_registerDataAvailableCallback( fd, svc_Cmd_ioCallback );
#elif (PLATFORM_IO == usb)
    bsp_UsbIo_registerDataAvailableCallback( fd, svc_Cmd_ioCallback );
#endif

    // Print out the command console header and first prompt
    printf( SVC_CMD_TXT_HDR, SVC_CMD_TXT_PROMPT );

    for(;;)
    {
        osapi_Semaphore_take( sem, OSAPI_TIMEOUT_WAIT_FOREVER );
        clearerr( stdin );
        c = getc( stdin );
        while( c != EOF )
        {
            svc_Cmd_bufferAddChar( (char)c );
            c = getc( stdin );
        }
    }
}

/*============================================================================*/
const osapi_ThreadInitInfo_t BSP_ATTR_USED BSP_ATTR_SECTION(".tinit") svc_Cmd_threadInitInfo =
{
  .name        = "Cmd",
  .handler     = svc_Cmd_threadMain,
  .arg         = NULL,
  .priority    = 2,
  .stackSize32 = SVC_CMD_STACK_SIZE_32,
  .stackPtr    = &svc_Cmd_stack[0]
};
BSP_PRAGMA_DATA_REQUIRED(svc_Cmd_threadInitInfo)
