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
 * @file bsp_Interrupt.h
 * @brief Contains Macros and defines for the interrupt controller.
 */
#ifndef BSP_INTERRUPT_H
#define BSP_INTERRUPT_H

#include BUILD_INCLUDE_STRING( bsp_Interrupt_, PLATFORM_CORE )

/*==============================================================================
 *                                Macros
 *============================================================================*/
/*==============================================================================
 * Define a macro to build the Priority Group/subgroup bits based on the platform
 * specific compile time selected GROUP_MODE
 */
#if (BSP_INTERRUPT_PRIORITY_GROUP_MODE == BSP_INTERRUPT_PRIORITY_MODE_GRP8_SUB1)
#define BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( _groupName, _subGroupName ) BSP_INTERRUPT_PRIORITY_GROUP_##_groupName
#elif (BSP_INTERRUPT_PRIORITY_GROUP_MODE == BSP_INTERRUPT_PRIORITY_MODE_GRP4_SUB2)
#define BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( _groupName, _subGroupName ) (((BSP_INTERRUPT_PRIORITY_GROUP_##_groupName) << 1) | (BSP_INTERRUPT_PRIORITY_SUBGROUP_##_subGroupName))
#elif (BSP_INTERRUPT_PRIORITY_GROUP_MODE == BSP_INTERRUPT_PRIORITY_MODE_GRP2_SUB4)
#define BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( _groupName, _subGroupName ) (((BSP_INTERRUPT_PRIORITY_GROUP_##_groupName) << 2) | (BSP_INTERRUPT_PRIORITY_SUBGROUP_##_subGroupName))
#elif (BSP_INTERRUPT_PRIORITY_GROUP_MODE == BSP_INTERRUPT_PRIORITY_MODE_GRP1_SUB8)
#define BSP_INTERRUPT_PRIORITY_GROUP_SUBGROUP( _groupName, _subGroupName ) BSP_INTERRUPT_PRIORITY_SUBGROUP_##_subGroupName
#endif


/*==============================================================================
 * Type for the vector handler
 */
#define BSP_INTERRUPT_BUILD_PRI_REG( _a, _b, _c, _d ) \
(                                                     \
    (uint32_t)(((uint32_t)(_d) << 29) |               \
               ((uint32_t)(_c) << 21) |               \
               ((uint32_t)(_b) << 13) |               \
               ((uint32_t)(_a) << 5))                 \
)


/*==============================================================================
 *                                Types
 *============================================================================*/
/*==============================================================================
 * Type for the vector handler
 */
typedef void (*bsp_Interrupt_VectorHandler_t)( void );

/*==============================================================================
 * Type for the interrupt priority mode
 */
#define BSP_INTERRUPT_PRIORITY_MODE_GRP8_SUB1 0x00
#define BSP_INTERRUPT_PRIORITY_MODE_GRP4_SUB2 0x05
#define BSP_INTERRUPT_PRIORITY_MODE_GRP2_SUB4 0x06
#define BSP_INTERRUPT_PRIORITY_MODE_GRP1_SUB8 0x07
typedef uint8_t bsp_Interrupt_PriorityMode_t;

/*==============================================================================
 * Type for the interrupt priority group
 */
#define BSP_INTERRUPT_PRIORITY_GROUP_0 ((bsp_Interrupt_PriorityGroup_t)0x00)
#define BSP_INTERRUPT_PRIORITY_GROUP_1 ((bsp_Interrupt_PriorityGroup_t)0x01)
#define BSP_INTERRUPT_PRIORITY_GROUP_2 ((bsp_Interrupt_PriorityGroup_t)0x02)
#define BSP_INTERRUPT_PRIORITY_GROUP_3 ((bsp_Interrupt_PriorityGroup_t)0x03)
#define BSP_INTERRUPT_PRIORITY_GROUP_4 ((bsp_Interrupt_PriorityGroup_t)0x04)
#define BSP_INTERRUPT_PRIORITY_GROUP_5 ((bsp_Interrupt_PriorityGroup_t)0x05)
#define BSP_INTERRUPT_PRIORITY_GROUP_6 ((bsp_Interrupt_PriorityGroup_t)0x06)
#define BSP_INTERRUPT_PRIORITY_GROUP_7 ((bsp_Interrupt_PriorityGroup_t)0x07)
typedef uint8_t bsp_Interrupt_PriorityGroup_t;

typedef uint8_t bsp_Interrupt_Id_t;

/*==============================================================================
 *                                Types
 *============================================================================*/
/*==============================================================================
 * Initialize the interrupt driver
 */
void
bsp_Interrupt_init( void );

/*==============================================================================
 * Enable an interrupt in the NVIC
 */
bool_t
bsp_Interrupt_enable( bsp_Interrupt_Id_t intId );

/*==============================================================================
 * Disable an interrupt in the NVIC
 */
bool_t
bsp_Interrupt_disable( bsp_Interrupt_Id_t intId );

/*==============================================================================
 * Clear a pending interrupt in the NVIC
 */
void
bsp_Interrupt_clearPending( bsp_Interrupt_Id_t intId );


#endif
