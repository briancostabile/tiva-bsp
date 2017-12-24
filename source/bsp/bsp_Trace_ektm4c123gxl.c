/*============================================================================*/
/**
 * @file bsp_Trace_ektm4c123gxl.c
 * @brief Contains table of available test point IOs
 */

#include "bsp_Types.h"
#include "bsp_Trace.h"
#include "bsp_Gpio.h"

/*==============================================================================
 *                               Globals
 *============================================================================*/
/*============================================================================*/
/* One entry for each IO port. */
const bsp_Trace_IoInfo_t bsp_Trace_ioInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS]=
{
    { BSP_GPIO_PORT_ID(PA0), (BSP_GPIO_MASK(TPA2) | BSP_GPIO_MASK(TPA3) | BSP_GPIO_MASK(TPA4) | BSP_GPIO_MASK(TPA5) | BSP_GPIO_MASK(TPA6) | BSP_GPIO_MASK(TPA7)) },
    { BSP_GPIO_PORT_ID(PB0), (BSP_GPIO_MASK(TPB2) | BSP_GPIO_MASK(TPB3) | BSP_GPIO_MASK(TPB4) | BSP_GPIO_MASK(TPB5)) },
    { BSP_GPIO_PORT_ID(PC0), (BSP_GPIO_MASK(TPC4) | BSP_GPIO_MASK(TPC5) | BSP_GPIO_MASK(TPC6) | BSP_GPIO_MASK(TPC7)) },
    { BSP_GPIO_PORT_ID(PD0), (BSP_GPIO_MASK(TPD2) | BSP_GPIO_MASK(TPD3) | BSP_GPIO_MASK(TPD6) | BSP_GPIO_MASK(TPD7)) },
    { BSP_GPIO_PORT_ID(PE0), (BSP_GPIO_MASK(TPE0) | BSP_GPIO_MASK(TPE1) | BSP_GPIO_MASK(TPE2) | BSP_GPIO_MASK(TPE3) | BSP_GPIO_MASK(TPE4) | BSP_GPIO_MASK(TPE5)) },
    { BSP_GPIO_PORT_ID(PF0), 0x00000000 }
};
