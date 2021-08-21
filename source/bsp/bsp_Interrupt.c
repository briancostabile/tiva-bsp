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
/*============================================================================*/
/**
 * @file bsp_Interrupt.c
 * @brief Contains stub functions for unhandled interrupts.
 */
#include "bsp_Types.h"
#include "bsp_Interrupt.h"
#include "bsp_Mcu.h"
#include "bsp_Pragma.h"
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"

/*==============================================================================
 *                                 Defines
 *============================================================================*/
#define BSP_INTERRUPT_REG_PTR_ENABLE_SET    ((volatile uint32_t *)0xE000E100)
#define BSP_INTERRUPT_REG_PTR_ENABLE_CLEAR  ((volatile uint32_t *)0xE000E180)
#define BSP_INTERRUPT_REG_PTR_PENDING_SET   ((volatile uint32_t *)0xE000E200)
#define BSP_INTERRUPT_REG_PTR_PENDING_CLEAR ((volatile uint32_t *)0xE000E280)
#define BSP_INTERRUPT_REG_PTR_ACTIVE        ((volatile uint32_t *)0xE000E300)
#define BSP_INTERRUPT_REG_PTR_PRI           ((volatile uint32_t *)0xE000E400)

#define BSP_INTERRUPT_REG_PTR_APINT         ((volatile uint32_t *)0xE000ED0C)
#define BSP_INTERRUPT_REG_PTR_SYSPRI        ((volatile uint32_t *)0xE000ED18)

#define BSP_INTERRUPT_SYS_PRIORITY_REG_CNT 3
#define BSP_INTERRUPT_INT_PRIORITY_REG_CNT 35

/*==============================================================================
 *                                Local Functions
 *============================================================================*/
void
bsp_Interrupt_init( void )
{
    uint8_t i;
    volatile uint32_t *regPtr;

    /* Setup priority mode */
    regPtr = BSP_INTERRUPT_REG_PTR_APINT;
    *regPtr = 0x05FA0000 | ((*regPtr) & 0x00008007) | (BSP_INTERRUPT_PRIORITY_GROUP_MODE << 8);

    /* Setup priorities for exceptions */
    regPtr = BSP_INTERRUPT_REG_PTR_SYSPRI;
    for( i=0; i<BSP_INTERRUPT_SYS_PRIORITY_REG_CNT; i++ )
    {
        *regPtr = bsp_Interrupt_groupPriorityTableExceptions[i];
        regPtr++;
    }

    /* Setup priorities for interrupts */
    regPtr = BSP_INTERRUPT_REG_PTR_PRI;
    for( i=0; i<BSP_INTERRUPT_INT_PRIORITY_REG_CNT; i++ )
    {
        *regPtr = bsp_Interrupt_groupPriorityTableInterrupts[i];
        regPtr++;
    }

    return;
}

/*============================================================================*/
bool_t
bsp_Interrupt_enable( bsp_Interrupt_Id_t intId )
{
    bool_t enabled;

    BSP_MCU_CRITICAL_SECTION_ENTER();
    enabled = (bool_t)(MAP_IntIsEnabled( intId ));
    MAP_IntEnable( intId );
    BSP_MCU_CRITICAL_SECTION_EXIT();

    return( enabled );
}

/*============================================================================*/
bool_t
bsp_Interrupt_disable( bsp_Interrupt_Id_t intId )
{
    bool_t enabled;

    BSP_MCU_CRITICAL_SECTION_ENTER();
    enabled = (bool_t)(MAP_IntIsEnabled( intId ));
    MAP_IntDisable( intId );
    BSP_MCU_CRITICAL_SECTION_EXIT();

    return( enabled );
}

/*============================================================================*/
void
bsp_Interrupt_clearPending( bsp_Interrupt_Id_t intId )
{
    MAP_IntPendClear( intId );
    return;
}


/*============================================================================*/
bsp_Interrupt_Id_t
bsp_Interrupt_activeId( void )
{
    return( ADDR_TO_REG( NVIC_INT_CTRL ) & NVIC_INT_CTRL_VEC_ACT_M );
}
