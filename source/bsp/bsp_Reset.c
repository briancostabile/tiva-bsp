/*===========================================================================*/
/**
 * @file bsp_Reset.c
 * @brief Contains Macros and defines for the HW reset reason.
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Reset.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"


/*=============================================================================
 *                                   Globals
 *===========================================================================*/
static bsp_Reset_HwReason_t bsp_Reset_hwReason;
static bsp_Reset_SwReason_t bsp_Reset_swReasonStored;
static bsp_Reset_SwReason_t bsp_Reset_swReason;
static bsp_Reset_Reason_t bsp_Reset_lastReason;
uint16_t bsp_Reset_lastAssertLine;
const char* bsp_Reset_lastAssertFunction;


/*
 * The following 3 globals are placed in fixed locations in RAM so they will
 * be in the same location even after a FW upgrade.
 */
//BSP_PRAGMA_DATA_LOCATION( bsp_Reset_count, "RAM_RESET_COUNT_XDATA" )
BSP_PRAGMA_NO_INIT uint32_t bsp_Reset_count;
//BSP_PRAGMA_DATA_LOCATION( bsp_Reset_swReason1, "RAM_RESET_SWREASON1_XDATA" )
BSP_PRAGMA_NO_INIT bsp_Reset_SwReason_t bsp_Reset_swReason1;
//BSP_PRAGMA_DATA_LOCATION( bsp_Reset_swReason2, "RAM_RESET_SWREASON2_XDATA" )
BSP_PRAGMA_NO_INIT bsp_Reset_SwReason_t bsp_Reset_swReason2;

BSP_PRAGMA_NO_INIT volatile uint16_t bsp_Reset_assertLine;
BSP_PRAGMA_NO_INIT const char* bsp_Reset_assertFunction;

/*=============================================================================
 *                                   Functions
 *===========================================================================*/
/*===========================================================================*/
void
bsp_Reset_init( void )
{
    uint32_t hwResetCause;
    bsp_Reset_count++;

    /* And out the 2 bits from the register */
    hwResetCause = MAP_SysCtlResetCauseGet();
    MAP_SysCtlResetCauseClear( hwResetCause );

    bsp_Reset_hwReason = ( (hwResetCause & 0x0000003F) |
                           (hwResetCause >> 10) );

    bsp_Reset_assertLine     = 0;
    bsp_Reset_assertFunction = "";

    if( bsp_Reset_swReason1 == bsp_Reset_swReason2 )
    {
        bsp_Reset_swReason = bsp_Reset_swReason1;
    }
    else
    {
        bsp_Reset_swReason = BSP_RESET_SWREASON_USE_STORED;
    }

    bsp_Reset_swReason1 = BSP_RESET_SWREASON_USE_STORED;
    bsp_Reset_swReason2 = BSP_RESET_SWREASON_USE_STORED;

    bsp_Reset_lastReason = ( ((uint32_t)bsp_Reset_hwReason << 16) |
                                ((uint32_t)bsp_Reset_swReason) );

    return;
}


/*===========================================================================*/
bsp_Reset_HwReason_t
bsp_Reset_getHwReason( void )
{
    return( bsp_Reset_hwReason );
}


/*===========================================================================*/
bsp_Reset_SwReason_t
bsp_Reset_getSwReason( void )
{
    return( bsp_Reset_swReason );
}


/*===========================================================================*/
bsp_Reset_Reason_t
bsp_Reset_getLastReason( void )
{
    return( bsp_Reset_lastReason );
}


/*===========================================================================*/
void
bsp_Reset_systemReset( bsp_Reset_SwReason_t reason )
{
    BSP_MCU_INT_DISABLE();

    if( reason == BSP_RESET_SWREASON_USE_STORED )
    {
        reason = bsp_Reset_swReasonStored;
    }

    bsp_Reset_swReason1 = reason;
    bsp_Reset_swReason2 = reason;

    /* No return from this */
    MAP_SysCtlReset();
}


/*===========================================================================*/
void
bsp_Reset_storeReason( bsp_Reset_SwReason_t reason )
{
    bsp_Reset_swReasonStored = reason;
    return;
}
