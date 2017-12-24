//*****************************************************************************
//
// project.c - Simple project to use as a starting point for more complex
//             projects.
//
// Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 1.0 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_Reset.h"
#include "bsp_Interrupt.h"
#include "bsp_Clk.h"
#include "bsp_Uart.h"
#include "bsp_UartIo.h"
#include "bsp_UsbIo.h"
#include "bsp_Gpio.h"
#include "bsp_Trace.h"
#include "bsp_I2CMaster.h"


#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include <string.h>
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Simple Project (project)</h1>
//!
//! A very simple example that can be used as a starting point for more complex
//! projects.  Most notably, this project is fully TI BSD licensed, so any and
//! all of the code (including the startup code) can be used as allowed by that
//! license.
//!
//! The provided code simply toggles a GPIO using the Tiva Peripheral Driver
//! Library.
//
//*****************************************************************************


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Toggle a GPIO.
//
//*****************************************************************************
int
main(void)
{
    int i;
    extern int remove_device( char* devName );

	bsp_Clk_init();
	bsp_Reset_init();
	bsp_Interrupt_init();
	bsp_Gpio_init();
    bsp_Trace_init();
    bsp_I2cMaster_init();
	bsp_Uart_init();
    bsp_UsbIo_init();
    bsp_UartIo_init();

    bsp_Gpio_configOutput( BSP_GPIO_PORT_ID(LED_R),
                           (BSP_GPIO_MASK(LED_R) | BSP_GPIO_MASK(LED_G) | BSP_GPIO_MASK(LED_B)),
                           FALSE, BSP_GPIO_DRIVE_8MA );

    bsp_Gpio_write( BSP_GPIO_PORT_ID(LED_R),
                    (BSP_GPIO_MASK(LED_R) | BSP_GPIO_MASK(LED_G) | BSP_GPIO_MASK(LED_B)),
                    0 );

    MAP_IntMasterEnable();

    /* Redirect stdout,stderr,stdin to/from usb file-io */
    freopen( BSP_PLATFORM_STDOUT_MAPPING":"BSP_PLATFORM_STDOUT_MAPPING, "w", stdout ); // redirect stdout to usb
    freopen( BSP_PLATFORM_STDERR_MAPPING":"BSP_PLATFORM_STDERR_MAPPING, "w", stderr ); // redirect stderr to usb
    freopen( BSP_PLATFORM_STDIN_MAPPING":"BSP_PLATFORM_STDIN_MAPPING, "r", stdin );  // redirect stdin from usb
    setvbuf( stdout, NULL, _IONBF, 0); // turn off buffering for stdout
    setvbuf( stderr, NULL, _IONBF, 0); // turn off buffering for stderr
    setvbuf( stdin, NULL, _IONBF, 0);  // turn off buffering for stdin

    //
    // Loop forever.
    //
    i = 1;
    int ch;
    while(1)
    {
        ch = getchar();
        if( ch != EOF )
        {
            printf( "stdout: received %c %d!\r\n", (char)ch, i++ );
            fprintf( stderr, "stderr: received %c %d!\r\n", (char)ch, i++ );
        }
    }
}
