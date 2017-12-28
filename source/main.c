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
#include "driverlib/interrupt.h"

#include <string.h>
#include <_lock.h>


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
                    1 );

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
