/*============================================================================*/
/**
 * @file bsp_Reset.h
 * @brief Contains Macros and defines for the reset reason.
 */
#ifndef BSP_RESET_H
#define BSP_RESET_H

#include "bsp_Types.h"

/*==============================================================================
 *                                   Types
 *============================================================================*/
typedef uint32_t bsp_Reset_Reason_t;

enum
{
    BSP_RESET_HWREASON_EXT  = 0x0001,
    BSP_RESET_HWREASON_POR  = 0x0002,
    BSP_RESET_HWREASON_BOR  = 0x0004,
    BSP_RESET_HWREASON_WDT0 = 0x0008,
    BSP_RESET_HWREASON_SW   = 0x0010,
    BSP_RESET_HWREASON_WDT1 = 0x0020,
    BSP_RESET_HWREASON_MOSC = 0x0040
};
typedef uint8_t bsp_Reset_HwReason_t;

enum
{
    BSP_RESET_SWREASON_USE_STORED    = 0x0000,
    BSP_RESET_SWREASON_ASSERT        = 0x0001,
    BSP_RESET_SWREASON_UNHANDLED_INT = 0x0002,
    BSP_RESET_SWREASON_UNKNOWN       = 0x0003
};
typedef uint8_t bsp_Reset_SwReason_t;

/*==============================================================================
 *                                   Globals
 *============================================================================*/
/*============================================================================*/
/**
 * @brief global counter for the number of resets
 */
extern uint32_t bsp_Reset_count;


/*******************************
 * The following globals are copied at boot to "last" globals and then they are
 * cleared and maintained throughout the operation. One every reset they are
 * latched into the corresponding "last" variable.
 ******************************/
/*============================================================================*/
/**
 * @brief global to keep track of the function that the last assert occurred in
 */
extern const char* bsp_Reset_assertFunction;
extern const char* bsp_Reset_lastAssertFunction;


/*============================================================================*/
/**
 * @brief global to keep track of the line number within the file that the last
 *        assert occurred in
 */
extern volatile uint16_t bsp_Reset_assertLine;
extern uint16_t bsp_Reset_lastAssertLine;

/*==============================================================================
 *                                   Prototypes
 *============================================================================*/
void
bsp_Reset_init( void );

bsp_Reset_HwReason_t
bsp_Reset_getHwReason( void );

bsp_Reset_SwReason_t
bsp_Reset_getSwReason( void );

bsp_Reset_Reason_t
bsp_Reset_getLastReason( void );

void
bsp_Reset_systemReset( bsp_Reset_SwReason_t reason );

void
bsp_Reset_storeReason( bsp_Reset_SwReason_t reason );

#endif
