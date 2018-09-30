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
 * @file bsp_Clk.c
 * @brief Contains Functions for configuring and accessing the System Clock
 */
#include "bsp_Gpio.h"
#include "bsp_Platform.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#if (BSP_PLATFORM_OSC_FREQ_HZ == 16000000)
#define BSP_CLK_SYSCTL_XTAL SYSCTL_XTAL_16MHZ
#endif

#if (BSP_PLATFORM_SYSTEM_CLK_FREQ_HZ == 80000000)
#define BSP_CLK_SYSCTL_SYSDIV SYSCTL_SYSDIV_2_5
#define BSP_CLK_TICKS_PER_NS( _ns ) (((_ns) * 2) / 25)
#endif

#define BSP_CLK_DELAY_NS( _ns ) { MAP_SysCtlDelay( (BSP_CLK_TICKS_PER_NS( _ns ) / 3) ); }

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Clk_init( void )
{
	MAP_SysCtlIntEnable( SYSCTL_INT_PLL_LOCK | SYSCTL_INT_MOSC_FAIL );

	MAP_SysCtlMOSCConfigSet( (SYSCTL_MOSC_VALIDATE | SYSCTL_MOSC_INTERRUPT) );

	/* Set System Clock to 80MHz */
	MAP_SysCtlClockSet( (BSP_CLK_SYSCTL_SYSDIV | SYSCTL_USE_PLL | BSP_CLK_SYSCTL_XTAL | SYSCTL_OSC_MAIN) );

	/* Wait for PLL to lock */
	while( SYSCTL_PLLSTAT_R == 0 );

	return;
}

/*============================================================================*/
void
bsp_Clk_delayNs( uint32_t ns )
{
	BSP_CLK_DELAY_NS( ns );

	return;
}

/*============================================================================*/
void
bsp_Clk_delayUs( uint32_t us )
{
	uint32_t i;

	for(i=0; i<us; i++)
	{
		BSP_CLK_DELAY_NS( 1000 );
	}

    return;
}

/*============================================================================*/
void
bsp_Clk_delayMs( uint32_t ms )
{
	uint32_t i;

	for(i=0; i<ms; i++)
	{
		BSP_CLK_DELAY_NS( 1000000 );
	}

    return;
}
