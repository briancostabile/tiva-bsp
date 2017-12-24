/*============================================================================*/
/**
 * @file bsp_Mcu.h
 * @brief Contains Macros and defines for MCU specific operations.
 */

#include "bsp_Types.h"
#include "bsp_Mcu.h"
#include "bsp_Trace.h"

/*==============================================================================
 *                               Functions
 *============================================================================*/
/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intEnable( void )
{
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n");

    /* To prevent compiler error, actual return is in assembly */
    return(0);
}


/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intDisable( void )
{
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n");

    /* To prevent compiler error, actual return is in assembly */
    return(0);
}


/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intStateGet( void )
{
    __asm("    mrs     r0, PRIMASK\n"
          "    bx      lr\n");

    /* To prevent compiler error, actual return is in assembly */
    return(0);
}

/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intStateSet( bsp_Mcu_IntState_t state )
{
    __asm("    cbnz    r0, _disable\n"
          "    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n"
          "_disable:\n"
          "    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n");

    /* To prevent compiler error, actual return is in assembly */
    return(0);
}

/*============================================================================*/
void
bsp_Mcu_waitForInt( void )
{
    __asm("    wfi\n");

    return;
}

/*============================================================================*/
bsp_Mcu_BasePriority_t
bsp_Mcu_basePriorityGet( void )
{
    __asm("    mrs     r0, BASEPRI\n"
          "    bx      lr\n");

    /* To prevent compiler error, actual return is in assembly */
    return(0);
}


/*============================================================================*/
void
bsp_Mcu_basePrioritySet( bsp_Mcu_BasePriority_t priority )
{
    __asm("    msr     BASEPRI, r0\n");

    return;
}
