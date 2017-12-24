/*============================================================================*/
/**
 * @file bsp_Assert.c
 */

#include "bsp_Assert.h"
#include "bsp_Mcu.h"


/*============================================================================*/
/**
 * @brief Halt the processor and flash LEDs
 *
 * This function is designed to reduce the code size of the ASSERT() macro
 */
void
bsp_Assert_halt( const char *funcName, uint16_t lineNum )
{
    BSP_MCU_INT_DISABLE();
    bsp_Reset_assertFunction = funcName;
    bsp_Reset_assertLine     = lineNum;
    while(1)
    {
        //Pet a watchdog
    }
}


/*============================================================================*/
/**
 * @brief Reset the processor
 *
 * This function is designed to reduce the code size of the ASSERT() macro
 */
void
bsp_Assert_reset( const char *funcName, uint16_t lineNum )
{
    BSP_MCU_INT_DISABLE();
    bsp_Reset_assertFunction = funcName;
    bsp_Reset_assertLine     = lineNum;
    bsp_Reset_systemReset( BSP_RESET_SWREASON_ASSERT );

    return;
}

