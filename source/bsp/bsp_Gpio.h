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
 * @file bsp_Gpio.h
 * @brief Contains types and prototypes to access the GPIO
 */
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "bsp_Platform.h"

#include BUILD_INCLUDE_STRING( bsp_Gpio_, PLATFORM )

/*==============================================================================
 *                             Defines
 *============================================================================*/

/*============================================================================*/
#define BSP_GPIO_DIR_INPUT  0x00
#define BSP_GPIO_DIR_OUTPUT 0x01
typedef uint8_t bsp_Gpio_Dir_t;

/*============================================================================*/
#define BSP_GPIO_INT_SENSE_EDGE  0x00
#define BSP_GPIO_INT_SENSE_LEVEL 0x01
typedef uint8_t bsp_Gpio_IntSense_t;

/*============================================================================*/
#define BSP_GPIO_INT_EDGE_SINGLE 0x00
#define BSP_GPIO_INT_EDGE_DUAL   0x01
typedef uint8_t bsp_Gpio_IntEdge_t;

/*============================================================================*/
#define BSP_GPIO_INT_EVENT_DIR_FALLING 0x00
#define BSP_GPIO_INT_EVENT_DIR_RISING  0x01
typedef uint8_t bsp_Gpio_IntEventDir_t;

/*============================================================================*/
#define BSP_GPIO_INT_TYPE_EDGE_RISING  0x00
#define BSP_GPIO_INT_TYPE_EDGE_FALLING 0x01
#define BSP_GPIO_INT_TYPE_EDGE_BOTH    0x02
#define BSP_GPIO_INT_TYPE_LEVEL_LOW    0x03
#define BSP_GPIO_INT_TYPE_LEVEL_HIGH   0x04
typedef uint8_t bsp_Gpio_IntType_t;

/*============================================================================*/
#define BSP_GPIO_INT_CONTROL_DISABLE 0x00
#define BSP_GPIO_INT_CONTROL_ENABLE  0x01
typedef uint8_t bsp_Gpio_IntControl_t;

/*============================================================================*/
#define BSP_GPIO_PULL_NONE 0x00
#define BSP_GPIO_PULL_UP   0x01
#define BSP_GPIO_PULL_DOWN 0x02
typedef uint8_t bsp_Gpio_Pull_t;

/*============================================================================*/
#define BSP_GPIO_DRIVE_2MA    0x00
#define BSP_GPIO_DRIVE_4MA    0x01
#define BSP_GPIO_DRIVE_8MA    0x02
#define BSP_GPIO_DRIVE_8MA_SC 0x03
#define BSP_GPIO_DRIVE_6MA    0x04
#define BSP_GPIO_DRIVE_10MA   0x05
#define BSP_GPIO_DRIVE_12MA   0x06
typedef uint8_t bsp_Gpio_Drive_t;

/*==============================================================================
 *                             Macros
 *============================================================================*/
/*============================================================================*/
/**
 * To write to the pin the address of the data register to write to depends on
 * the bit offset
 */
#define BSP_GPIO_DATA_REG_FROM_NAME( _ioName ) *(((volatile uint32_t *)(BSP_GPIO_BASE_ADDR_##_ioName)) + BSP_GPIO_BIT_MASK_##_ioName)

/*============================================================================*/
/**
 * Macros to get the platform port and mask from the name
 */
#define BSP_GPIO_PORT_ID( _name ) (BSP_GPIO_PORT_ID_##_name)
#define BSP_GPIO_PORT_BASE_ADDR( _name ) (BSP_GPIO_BASE_ADDR_##_name)
#define BSP_GPIO_ALT_FUNC( _name ) (GPIO_##_name)
#define BSP_GPIO_MASK( _name ) (bsp_Gpio_BitMask_t)(BSP_GPIO_BIT_MASK_##_name)


/*============================================================================*/
/**
 * Macros to set/clear/toggle a single IO based on the defined name
 */
#define BSP_GPIO_OUT_SET_HIGH( _name )                                 \
{                                                                      \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) |= BSP_GPIO_BIT_MASK_##_name; \
}

#define BSP_GPIO_OUT_SET_LOW( _name )                                   \
{                                                                       \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) &= ~BSP_GPIO_BIT_MASK_##_name; \
}

#define BSP_GPIO_TOGGLE( _name )                                       \
{                                                                      \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) ^= BSP_GPIO_BIT_MASK_##_name; \
}

/*============================================================================*/
/**
 * Macro to Read a single IO based on the defined name
 */
#define BSP_GPIO_GET( _name )                                           \
(                                                                       \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) >> BSP_GPIO_BIT_OFFSET_##_name \
)


/*==============================================================================
 *                               Types
 *============================================================================*/
/*============================================================================*/
typedef uint8_t bsp_Gpio_BitMask_t;

/*============================================================================*/
typedef uint32_t bsp_Gpio_AltFuncId_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Gpio_init( void );

/*============================================================================*/
void
bsp_Gpio_initPlatform( void );

/*============================================================================*/
void
bsp_Gpio_write( bsp_Gpio_PortId_t  portId,
		        bsp_Gpio_BitMask_t mask,
		        bsp_Gpio_BitMask_t val );

/*============================================================================*/
bsp_Gpio_BitMask_t
bsp_Gpio_read( bsp_Gpio_PortId_t  portId,
		       bsp_Gpio_BitMask_t mask );

/*============================================================================*/
void
bsp_Gpio_configAnalog( bsp_Gpio_PortId_t    portId,
                       bsp_Gpio_BitMask_t   mask );

/*============================================================================*/
void
bsp_Gpio_configAltFunction( bsp_Gpio_PortId_t    portId,
                            bsp_Gpio_BitMask_t   mask,
                            bsp_Gpio_AltFuncId_t altFuncId );

/*============================================================================*/
void
bsp_Gpio_configOutput( bsp_Gpio_PortId_t  portId,
                       bsp_Gpio_BitMask_t mask,
                       bool_t             openDrain,
                       bsp_Gpio_Drive_t   drive );

/*============================================================================*/
void
bsp_Gpio_configInput( bsp_Gpio_PortId_t  portId,
                      bsp_Gpio_BitMask_t mask,
                      bool_t             openDrain,
                      bsp_Gpio_Pull_t    pull );

/*============================================================================*/
void
bsp_Gpio_intControl( bsp_Gpio_PortId_t     portId,
                     bsp_Gpio_BitMask_t    mask,
                     bsp_Gpio_IntControl_t control );

/*============================================================================*/
void
bsp_Gpio_intConfig( bsp_Gpio_PortId_t       portId,
                    bsp_Gpio_BitMask_t      mask,
                    bool_t                  dmaTriggerEnable,
                    bool_t                  adcTriggerEnable,
                    bsp_Gpio_IntType_t      type,
                    bsp_Gpio_InputHandler_t callback );

#endif
