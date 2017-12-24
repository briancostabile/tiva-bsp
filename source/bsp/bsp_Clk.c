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
