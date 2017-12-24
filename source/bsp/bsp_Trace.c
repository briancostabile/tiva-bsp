/*============================================================================*/
/**
 * @file bsp_Trace.c
 * @brief Contains IO port initialization function to configure all Test point
 *        IO pins to outputs
 */

#include "bsp_Types.h"
#include "bsp_Trace.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Trace_init( void )
{
    uint8_t i;

    for( i=0; i<DIM(bsp_Trace_ioInfoTable); i++ )
    {
        /* For each port configure the test points as output 2ma drive
         * and set each output to low
         */
        bsp_Gpio_configOutput( bsp_Trace_ioInfoTable[i].portId,
                               bsp_Trace_ioInfoTable[i].mask,
                               FALSE, BSP_GPIO_DRIVE_2MA );

        bsp_Gpio_write( bsp_Trace_ioInfoTable[i].portId,
                        bsp_Trace_ioInfoTable[i].mask,
                        0 );
    }

    return;
}
