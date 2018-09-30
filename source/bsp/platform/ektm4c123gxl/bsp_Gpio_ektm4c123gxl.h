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
 * @file bsp_Gpio_ektm4c123gxl.h
 * @brief Contains Macros and defines for the GPIO block specific
 *        to the extm4c123gxl platform.
 */
#ifndef BSP_GPIO_EKTM4C123GXL_H
#define BSP_GPIO_EKTM4C123GXL_H

#include "inc/hw_memmap.h"
#include "inc/hw_Gpio.h"
#include "bsp_Platform.h"

#include BUILD_INCLUDE_STRING( bsp_Gpio_, PLATFORM_CORE )


#define BSP_GPIO_PORT_ID_NUM_PORTS 6

/**
 * PA0 - VCP-RX
 * PA1 - VCP-TX
 * PA2 - TPA2
 * PA3 - TPA3
 * PA4 - TPA4
 * PA5 - TPA5
 * PA6 - TPA6
 * PA7 - TPA7
 */
#define BSP_GPIO_PORT_ID_VCP_RX    BSP_GPIO_PORT_ID_PA0
#define BSP_GPIO_BASE_ADDR_VCP_RX  BSP_GPIO_BASE_ADDR_PA0
#define BSP_GPIO_BIT_OFFSET_VCP_RX BSP_GPIO_BIT_OFFSET_PA0
#define BSP_GPIO_BIT_MASK_VCP_RX   BSP_GPIO_BIT_MASK_PA0

#define BSP_GPIO_PORT_ID_VCP_TX     BSP_GPIO_PORT_ID_PA1
#define BSP_GPIO_BASE_ADDR_VCP_TX   BSP_GPIO_BASE_ADDR_PA1
#define BSP_GPIO_BIT_OFFSET_VCP_TX  BSP_GPIO_BIT_OFFSET_PA1
#define BSP_GPIO_BIT_MASK_VCP_TX    BSP_GPIO_BIT_MASK_PA1

#define BSP_GPIO_PORT_ID_TPA2       BSP_GPIO_PORT_ID_PA2
#define BSP_GPIO_BASE_ADDR_TPA2     BSP_GPIO_BASE_ADDR_PA2
#define BSP_GPIO_BIT_OFFSET_TPA2    BSP_GPIO_BIT_OFFSET_PA2
#define BSP_GPIO_BIT_MASK_TPA2      BSP_GPIO_BIT_MASK_PA2

#define BSP_GPIO_PORT_ID_TPA3       BSP_GPIO_PORT_ID_PA3
#define BSP_GPIO_BASE_ADDR_TPA3     BSP_GPIO_BASE_ADDR_PA3
#define BSP_GPIO_BIT_OFFSET_TPA3    BSP_GPIO_BIT_OFFSET_PA3
#define BSP_GPIO_BIT_MASK_TPA3      BSP_GPIO_BIT_MASK_PA3

#define BSP_GPIO_PORT_ID_TPA4       BSP_GPIO_PORT_ID_PA4
#define BSP_GPIO_BASE_ADDR_TPA4     BSP_GPIO_BASE_ADDR_PA4
#define BSP_GPIO_BIT_OFFSET_TPA4    BSP_GPIO_BIT_OFFSET_PA4
#define BSP_GPIO_BIT_MASK_TPA4      BSP_GPIO_BIT_MASK_PA4

#define BSP_GPIO_PORT_ID_TPA5       BSP_GPIO_PORT_ID_PA5
#define BSP_GPIO_BASE_ADDR_TPA5     BSP_GPIO_BASE_ADDR_PA5
#define BSP_GPIO_BIT_OFFSET_TPA5    BSP_GPIO_BIT_OFFSET_PA5
#define BSP_GPIO_BIT_MASK_TPA5      BSP_GPIO_BIT_MASK_PA5

#define BSP_GPIO_PORT_ID_TPA6       BSP_GPIO_PORT_ID_PA6
#define BSP_GPIO_BASE_ADDR_TPA6     BSP_GPIO_BASE_ADDR_PA6
#define BSP_GPIO_BIT_OFFSET_TPA6    BSP_GPIO_BIT_OFFSET_PA6
#define BSP_GPIO_BIT_MASK_TPA6      BSP_GPIO_BIT_MASK_PA6

#define BSP_GPIO_PORT_ID_TPA7       BSP_GPIO_PORT_ID_PA7
#define BSP_GPIO_BASE_ADDR_TPA7     BSP_GPIO_BASE_ADDR_PA7
#define BSP_GPIO_BIT_OFFSET_TPA7    BSP_GPIO_BIT_OFFSET_PA7
#define BSP_GPIO_BIT_MASK_TPA7      BSP_GPIO_BIT_MASK_PA7


/**
 * PB0 - USB_ID
 * PB1 - USB_VB
 * PB2 - TPB2
 * PB3 - TPB3
 * PB4 - TPB4
 * PB5 - TPB5
 * PB6 - NCPB6 (externally connected to PD0)
 * PB7 - NCPB7 (externally connected to PD1)
 */
#define BSP_GPIO_PORT_ID_USB_ID    BSP_GPIO_PORT_ID_PB0
#define BSP_GPIO_BASE_ADDR_USB_ID  BSP_GPIO_BASE_ADDR_PB0
#define BSP_GPIO_BIT_OFFSET_USB_ID BSP_GPIO_BIT_OFFSET_PB0
#define BSP_GPIO_BIT_MASK_USB_ID   BSP_GPIO_BIT_MASK_PB0

#define BSP_GPIO_PORT_ID_USB_VB    BSP_GPIO_PORT_ID_PB1
#define BSP_GPIO_BASE_ADDR_USB_VB  BSP_GPIO_BASE_ADDR_PB1
#define BSP_GPIO_BIT_OFFSET_USB_VB BSP_GPIO_BIT_OFFSET_PB1
#define BSP_GPIO_BIT_MASK_USB_VB   BSP_GPIO_BIT_MASK_PB1

#define BSP_GPIO_PORT_ID_TPB2      BSP_GPIO_PORT_ID_PB2
#define BSP_GPIO_BASE_ADDR_TPB2    BSP_GPIO_BASE_ADDR_PB2
#define BSP_GPIO_BIT_OFFSET_TPB2   BSP_GPIO_BIT_OFFSET_PB2
#define BSP_GPIO_BIT_MASK_TPB2     BSP_GPIO_BIT_MASK_PB2

#define BSP_GPIO_PORT_ID_TPB3      BSP_GPIO_PORT_ID_PB3
#define BSP_GPIO_BASE_ADDR_TPB3    BSP_GPIO_BASE_ADDR_PB3
#define BSP_GPIO_BIT_OFFSET_TPB3   BSP_GPIO_BIT_OFFSET_PB3
#define BSP_GPIO_BIT_MASK_TPB3     BSP_GPIO_BIT_MASK_PB3

#define BSP_GPIO_PORT_ID_TPB4      BSP_GPIO_PORT_ID_PB4
#define BSP_GPIO_BASE_ADDR_TPB4    BSP_GPIO_BASE_ADDR_PB4
#define BSP_GPIO_BIT_OFFSET_TPB4   BSP_GPIO_BIT_OFFSET_PB4
#define BSP_GPIO_BIT_MASK_TPB4     BSP_GPIO_BIT_MASK_PB4

#define BSP_GPIO_PORT_ID_TPB5      BSP_GPIO_PORT_ID_PB5
#define BSP_GPIO_BASE_ADDR_TPB5    BSP_GPIO_BASE_ADDR_PB5
#define BSP_GPIO_BIT_OFFSET_TPB5   BSP_GPIO_BIT_OFFSET_PB5
#define BSP_GPIO_BIT_MASK_TPB5     BSP_GPIO_BIT_MASK_PB5

#define BSP_GPIO_PORT_ID_NCPB6     BSP_GPIO_PORT_ID_PB6
#define BSP_GPIO_BASE_ADDR_NCPB6   BSP_GPIO_BASE_ADDR_PB6
#define BSP_GPIO_BIT_OFFSET_NCPB6  BSP_GPIO_BIT_OFFSET_PB6
#define BSP_GPIO_BIT_MASK_NCPB6    BSP_GPIO_BIT_MASK_PB6

#define BSP_GPIO_PORT_ID_NCPB7     BSP_GPIO_PORT_ID_PB7
#define BSP_GPIO_BASE_ADDR_NCPB7   BSP_GPIO_BASE_ADDR_PB7
#define BSP_GPIO_BIT_OFFSET_NCPB7  BSP_GPIO_BIT_OFFSET_PB7
#define BSP_GPIO_BIT_MASK_NCPB7    BSP_GPIO_BIT_MASK_PB7


/**
 * PC0 - JTAG-TCK
 * PC1 - JTAG-TMS
 * PC2 - JTAG-TDI
 * PC3 - JTAG-TDO
 * PC4 - TPC4
 * PC5 - TPC5
 * PC6 - TPC6
 * PC7 - TPC7
 */
#define BSP_GPIO_PORT_ID_JTAG_TCK     BSP_GPIO_PORT_ID_PC0
#define BSP_GPIO_BASE_ADDR_JTAG_TCK   BSP_GPIO_BASE_ADDR_PC0
#define BSP_GPIO_BIT_OFFSET_JTAG_TCK  BSP_GPIO_BIT_OFFSET_PC0
#define BSP_GPIO_BIT_MASK_JTAG_TCK    BSP_GPIO_BIT_MASK_PC0

#define BSP_GPIO_PORT_ID_JTAG_TMS     BSP_GPIO_PORT_ID_PC1
#define BSP_GPIO_BASE_ADDR_JTAG_TMS   BSP_GPIO_BASE_ADDR_PC1
#define BSP_GPIO_BIT_OFFSET_JTAG_TMS  BSP_GPIO_BIT_OFFSET_PC1
#define BSP_GPIO_BIT_MASK_JTAG_TMS    BSP_GPIO_BIT_MASK_PC1

#define BSP_GPIO_PORT_ID_JTAG_TDI     BSP_GPIO_PORT_ID_PC2
#define BSP_GPIO_BASE_ADDR_JTAG_TDI   BSP_GPIO_BASE_ADDR_PC2
#define BSP_GPIO_BIT_OFFSET_JTAG_TDI  BSP_GPIO_BIT_OFFSET_PC2
#define BSP_GPIO_BIT_MASK_JTAG_TDI    BSP_GPIO_BIT_MASK_PC2

#define BSP_GPIO_PORT_ID_JTAG_TDO     BSP_GPIO_PORT_ID_PC3
#define BSP_GPIO_BASE_ADDR_JTAG_TDO   BSP_GPIO_BASE_ADDR_PC3
#define BSP_GPIO_BIT_OFFSET_JTAG_TDO  BSP_GPIO_BIT_OFFSET_PC3
#define BSP_GPIO_BIT_MASK_JTAG_TDO    BSP_GPIO_BIT_MASK_PC3

#define BSP_GPIO_PORT_ID_TPC4         BSP_GPIO_PORT_ID_PC4
#define BSP_GPIO_BASE_ADDR_TPC4       BSP_GPIO_BASE_ADDR_PC4
#define BSP_GPIO_BIT_OFFSET_TPC4      BSP_GPIO_BIT_OFFSET_PC4
#define BSP_GPIO_BIT_MASK_TPC4        BSP_GPIO_BIT_MASK_PC4

#define BSP_GPIO_PORT_ID_TPC5         BSP_GPIO_PORT_ID_PC5
#define BSP_GPIO_BASE_ADDR_TPC5       BSP_GPIO_BASE_ADDR_PC5
#define BSP_GPIO_BIT_OFFSET_TPC5      BSP_GPIO_BIT_OFFSET_PC5
#define BSP_GPIO_BIT_MASK_TPC5        BSP_GPIO_BIT_MASK_PC5

#define BSP_GPIO_PORT_ID_TPC6         BSP_GPIO_PORT_ID_PC6
#define BSP_GPIO_BASE_ADDR_TPC6       BSP_GPIO_BASE_ADDR_PC6
#define BSP_GPIO_BIT_OFFSET_TPC6      BSP_GPIO_BIT_OFFSET_PC6
#define BSP_GPIO_BIT_MASK_TPC6        BSP_GPIO_BIT_MASK_PC6

#define BSP_GPIO_PORT_ID_TPC7         BSP_GPIO_PORT_ID_PC7
#define BSP_GPIO_BASE_ADDR_TPC7       BSP_GPIO_BASE_ADDR_PC7
#define BSP_GPIO_BIT_OFFSET_TPC7      BSP_GPIO_BIT_OFFSET_PC7
#define BSP_GPIO_BIT_MASK_TPC7        BSP_GPIO_BIT_MASK_PC7


/**
 * PD0 - I2C_SCL
 * PD1 - I2C_SDA
 * PD2 - TPD2
 * PD3 - TPD3
 * PD4 - USB_DM
 * PD5 - USB_DP
 * PD6 - TPD6
 * PD7 - TPD7
 */
#define BSP_GPIO_PORT_ID_I2C_SCL     BSP_GPIO_PORT_ID_PD0
#define BSP_GPIO_BASE_ADDR_I2C_SCL   BSP_GPIO_BASE_ADDR_PD0
#define BSP_GPIO_BIT_OFFSET_I2C_SCL  BSP_GPIO_BIT_OFFSET_PD0
#define BSP_GPIO_BIT_MASK_I2C_SCL    BSP_GPIO_BIT_MASK_PD0

#define BSP_GPIO_PORT_ID_I2C_SDA     BSP_GPIO_PORT_ID_PD1
#define BSP_GPIO_BASE_ADDR_I2C_SDA   BSP_GPIO_BASE_ADDR_PD1
#define BSP_GPIO_BIT_OFFSET_I2C_SDA  BSP_GPIO_BIT_OFFSET_PD1
#define BSP_GPIO_BIT_MASK_I2C_SDA    BSP_GPIO_BIT_MASK_PD1

#define BSP_GPIO_PORT_ID_TPD2        BSP_GPIO_PORT_ID_PD2
#define BSP_GPIO_BASE_ADDR_TPD2      BSP_GPIO_BASE_ADDR_PD2
#define BSP_GPIO_BIT_OFFSET_TPD2     BSP_GPIO_BIT_OFFSET_PD2
#define BSP_GPIO_BIT_MASK_TPD2       BSP_GPIO_BIT_MASK_PD2

#define BSP_GPIO_PORT_ID_TPD3        BSP_GPIO_PORT_ID_PD3
#define BSP_GPIO_BASE_ADDR_TPD3      BSP_GPIO_BASE_ADDR_PD3
#define BSP_GPIO_BIT_OFFSET_TPD3     BSP_GPIO_BIT_OFFSET_PD3
#define BSP_GPIO_BIT_MASK_TPD3       BSP_GPIO_BIT_MASK_PD3

#define BSP_GPIO_PORT_ID_USB_DM      BSP_GPIO_PORT_ID_PD4
#define BSP_GPIO_BASE_ADDR_USB_DM    BSP_GPIO_BASE_ADDR_PD4
#define BSP_GPIO_BIT_OFFSET_USB_DM   BSP_GPIO_BIT_OFFSET_PD4
#define BSP_GPIO_BIT_MASK_USB_DM     BSP_GPIO_BIT_MASK_PD4

#define BSP_GPIO_PORT_ID_USB_DP      BSP_GPIO_PORT_ID_PD5
#define BSP_GPIO_BASE_ADDR_USB_DP    BSP_GPIO_BASE_ADDR_PD5
#define BSP_GPIO_BIT_OFFSET_USB_DP   BSP_GPIO_BIT_OFFSET_PD5
#define BSP_GPIO_BIT_MASK_USB_DP     BSP_GPIO_BIT_MASK_PD5

#define BSP_GPIO_PORT_ID_TPD6        BSP_GPIO_PORT_ID_PD6
#define BSP_GPIO_BASE_ADDR_TPD6      BSP_GPIO_BASE_ADDR_PD6
#define BSP_GPIO_BIT_OFFSET_TPD6     BSP_GPIO_BIT_OFFSET_PD6
#define BSP_GPIO_BIT_MASK_TPD6       BSP_GPIO_BIT_MASK_PD6

#define BSP_GPIO_PORT_ID_TPD7        BSP_GPIO_PORT_ID_PD7
#define BSP_GPIO_BASE_ADDR_TPD7      BSP_GPIO_BASE_ADDR_PD7
#define BSP_GPIO_BIT_OFFSET_TPD7     BSP_GPIO_BIT_OFFSET_PD7
#define BSP_GPIO_BIT_MASK_TPD7       BSP_GPIO_BIT_MASK_PD7


/**
 * PE0 - TPE0
 * PE1 - TPE1
 * PE2 - TPE2
 * PE3 - TPE3
 * PE4 - TPE4
 * PE5 - TPE5
 */
#define BSP_GPIO_PORT_ID_TPE0    BSP_GPIO_PORT_ID_PE0
#define BSP_GPIO_BASE_ADDR_TPE0  BSP_GPIO_BASE_ADDR_PE0
#define BSP_GPIO_BIT_OFFSET_TPE0 BSP_GPIO_BIT_OFFSET_PE0
#define BSP_GPIO_BIT_MASK_TPE0   BSP_GPIO_BIT_MASK_PE0

#define BSP_GPIO_PORT_ID_TPE1    BSP_GPIO_PORT_ID_PE1
#define BSP_GPIO_BASE_ADDR_TPE1  BSP_GPIO_BASE_ADDR_PE1
#define BSP_GPIO_BIT_OFFSET_TPE1 BSP_GPIO_BIT_OFFSET_PE1
#define BSP_GPIO_BIT_MASK_TPE1   BSP_GPIO_BIT_MASK_PE1

#define BSP_GPIO_PORT_ID_TPE2    BSP_GPIO_PORT_ID_PE2
#define BSP_GPIO_BASE_ADDR_TPE2  BSP_GPIO_BASE_ADDR_PE2
#define BSP_GPIO_BIT_OFFSET_TPE2 BSP_GPIO_BIT_OFFSET_PE2
#define BSP_GPIO_BIT_MASK_TPE2   BSP_GPIO_BIT_MASK_PE2

#define BSP_GPIO_PORT_ID_TPE3    BSP_GPIO_PORT_ID_PE3
#define BSP_GPIO_BASE_ADDR_TPE3  BSP_GPIO_BASE_ADDR_PE3
#define BSP_GPIO_BIT_OFFSET_TPE3 BSP_GPIO_BIT_OFFSET_PE3
#define BSP_GPIO_BIT_MASK_TPE3   BSP_GPIO_BIT_MASK_PE3

#define BSP_GPIO_PORT_ID_TPE4    BSP_GPIO_PORT_ID_PE4
#define BSP_GPIO_BASE_ADDR_TPE4  BSP_GPIO_BASE_ADDR_PE4
#define BSP_GPIO_BIT_OFFSET_TPE4 BSP_GPIO_BIT_OFFSET_PE4
#define BSP_GPIO_BIT_MASK_TPE4   BSP_GPIO_BIT_MASK_PE4

#define BSP_GPIO_PORT_ID_TPE5    BSP_GPIO_PORT_ID_PE5
#define BSP_GPIO_BASE_ADDR_TPE5  BSP_GPIO_BASE_ADDR_PE5
#define BSP_GPIO_BIT_OFFSET_TPE5 BSP_GPIO_BIT_OFFSET_PE5
#define BSP_GPIO_BIT_MASK_TPE5   BSP_GPIO_BIT_MASK_PE5


/**
 * PF0 - USR-SW2
 * PF1 - LED-R
 * PF2 - LED-B
 * PF3 - LED-G
 * PF4 - USR-SW1
 */
#define BSP_GPIO_PORT_ID_USR_SW2       BSP_GPIO_PORT_ID_PF0
#define BSP_GPIO_BASE_ADDR_USR_SW2     BSP_GPIO_BASE_ADDR_PF0
#define BSP_GPIO_BIT_OFFSET_USR_SW2    BSP_GPIO_BIT_OFFSET_PF0
#define BSP_GPIO_BIT_MASK_USR_SW2      BSP_GPIO_BIT_MASK_PF0

#define BSP_GPIO_PORT_ID_LED_R         BSP_GPIO_PORT_ID_PF1
#define BSP_GPIO_BASE_ADDR_LED_R       BSP_GPIO_BASE_ADDR_PF1
#define BSP_GPIO_BIT_OFFSET_LED_R      BSP_GPIO_BIT_OFFSET_PF1
#define BSP_GPIO_BIT_MASK_LED_R        BSP_GPIO_BIT_MASK_PF1

#define BSP_GPIO_PORT_ID_LED_B         BSP_GPIO_PORT_ID_PF2
#define BSP_GPIO_BASE_ADDR_LED_B       BSP_GPIO_BASE_ADDR_PF2
#define BSP_GPIO_BIT_OFFSET_LED_B      BSP_GPIO_BIT_OFFSET_PF2
#define BSP_GPIO_BIT_MASK_LED_B        BSP_GPIO_BIT_MASK_PF2

#define BSP_GPIO_PORT_ID_LED_G         BSP_GPIO_PORT_ID_PF3
#define BSP_GPIO_BASE_ADDR_LED_G       BSP_GPIO_BASE_ADDR_PF3
#define BSP_GPIO_BIT_OFFSET_LED_G      BSP_GPIO_BIT_OFFSET_PF3
#define BSP_GPIO_BIT_MASK_LED_G        BSP_GPIO_BIT_MASK_PF3

#define BSP_GPIO_PORT_ID_USR_SW1       BSP_GPIO_PORT_ID_PF4
#define BSP_GPIO_BASE_ADDR_USR_SW1     BSP_GPIO_BASE_ADDR_PF4
#define BSP_GPIO_BIT_OFFSET_USR_SW1    BSP_GPIO_BIT_OFFSET_PF4
#define BSP_GPIO_BIT_MASK_USR_SW1      BSP_GPIO_BIT_MASK_PF4



/*==============================================================================
 *                                Globals
 *============================================================================*/
/*============================================================================*/
extern const bsp_Gpio_PlatformPortInfo_t bsp_Gpio_platformPortInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS];



#endif