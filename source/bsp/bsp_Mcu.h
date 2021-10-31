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
 * @file bsp_Mcu.h
 * @brief Contains Macros and defines for MCU specific operations.
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Pragma.h"
#include "bsp_Trace.h"
#include "hw_sysctl.h"

/*==============================================================================
 *                                   Macros
 *============================================================================*/
/*============================================================================*/
/**
 * @brief This macro enables global interrupts.
 */
#define BSP_MCU_INT_ENABLE() ( bsp_Mcu_intEnable() )


/*============================================================================*/
/**
 * @brief This macro disables global interrupts.
 */
#define BSP_MCU_INT_DISABLE() ( bsp_Mcu_intDisable() )


/*============================================================================*/
/**
 * @brief This macro returns the current state of the global interrupts.
 *        0 = disabled, 1 = enabled
 */
#define BSP_MCU_INT_STATE_GET() ( bsp_Mcu_intStateGet() )


/*============================================================================*/
/**
 * @brief This macro sets the state of global interrupts to the passed in value.
 *        0 = disabled, 1 = enabled
 */
#define BSP_MCU_INT_STATE_SET(_state) {bsp_Mcu_intStateSet((_state));}


/*============================================================================*/
/**
 * @brief These macros must be used together to wrap critical blocks of code.
 *        ENTER and EXIT must be used at the same level of scope.
 */
#define BSP_MCU_CRITICAL_SECTION_ENTER() { bsp_Mcu_IntState_t _intState = BSP_MCU_INT_DISABLE(); BSP_TRACE_INT_LOCK_ENTER();
#define BSP_MCU_CRITICAL_SECTION_EXIT()    BSP_TRACE_INT_LOCK_EXIT(); BSP_MCU_INT_STATE_SET(_intState); }


/*==============================================================================
 *                                   Types
 *============================================================================*/
/*============================================================================*/
/**
 * @brief Type enumerating the enable/disable state of interrupts.
 */
#define BSP_MCU_INT_STATE_DISABLED ((bsp_Mcu_IntState_t)0)
#define BSP_MCU_INT_STATE_ENABLED  ((bsp_Mcu_IntState_t)1)

typedef uint32_t bsp_Mcu_IntState_t;

/*============================================================================*/
#define BSP_MCU_BASE_PRIORITY_0 ((bsp_Mcu_BasePriority_t)(0x00000000 << 4))
#define BSP_MCU_BASE_PRIORITY_1 ((bsp_Mcu_BasePriority_t)(0x00000001 << 4))
#define BSP_MCU_BASE_PRIORITY_2 ((bsp_Mcu_BasePriority_t)(0x00000002 << 4))
#define BSP_MCU_BASE_PRIORITY_3 ((bsp_Mcu_BasePriority_t)(0x00000003 << 4))
#define BSP_MCU_BASE_PRIORITY_4 ((bsp_Mcu_BasePriority_t)(0x00000004 << 4))
#define BSP_MCU_BASE_PRIORITY_5 ((bsp_Mcu_BasePriority_t)(0x00000005 << 4))
#define BSP_MCU_BASE_PRIORITY_6 ((bsp_Mcu_BasePriority_t)(0x00000006 << 4))
#define BSP_MCU_BASE_PRIORITY_7 ((bsp_Mcu_BasePriority_t)(0x00000007 << 4))
typedef uint32_t bsp_Mcu_BasePriority_t;

/*==============================================================================
 *                                Prototypes
 *============================================================================*/
/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intEnable( void );

/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intDisable( void );

/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intStateGet( void );

/*============================================================================*/
bsp_Mcu_IntState_t
bsp_Mcu_intStateSet( bsp_Mcu_IntState_t state );

/*============================================================================*/
void
bsp_Mcu_waitForInt( void );

/*============================================================================*/
bsp_Mcu_BasePriority_t
bsp_Mcu_basePriorityGet( void );

/*============================================================================*/
void
bsp_Mcu_basePrioritySet( bsp_Mcu_BasePriority_t priority );
