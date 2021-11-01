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
//*****************************************************************************


#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_Reset.h"
#include "bsp_Interrupt.h"
#include "bsp_Clk.h"
#include "bsp_Led.h"
#include "bsp_Uart.h"
#include "bsp_UartIo.h"
#include "bsp_Usb.h"
#include "bsp_UsbIo.h"
#include "bsp_UsbBulk.h"
#include "bsp_Gpio.h"
#include "bsp_Trace.h"
#include "bsp_Button.h"
#include "bsp_TimerGp.h"
#include "bsp_I2c.h"
#include "bsp_Io.h"
#include "bsp_Dma.h"
#include "bsp_Crc.h"
#include "bsp_Eeprom.h"

#include "osapi.h"

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include <string.h>
//#include <_lock.h>


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
int
main(void)
{
    extern int remove_device( char* devName );

    bsp_Clk_init();
    bsp_Reset_init();
    bsp_Interrupt_init();
    bsp_Dma_init();
    bsp_Gpio_init();
    bsp_Trace_init();
    bsp_Led_init();
    bsp_Button_init();
    bsp_TimerGp_init();
    bsp_I2c_init();
    bsp_Uart_init();
    bsp_Usb_init();
    bsp_Io_init();
    bsp_UsbIo_init();
    bsp_UartIo_init();
    bsp_Crc_init();
    bsp_Eeprom_init();

    // Initialize the OS
    osapi_init();

    MAP_IntMasterEnable();

    // Run the Scheduler, No Return
    osapi_Scheduler_run();
}
