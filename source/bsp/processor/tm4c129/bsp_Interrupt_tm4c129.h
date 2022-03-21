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
 * @file bsp_Interrupt_tm4c129.h
 * @brief Contains Interrupt defines common to the tm4c129 core.
 */
#pragma once

#include "bsp_Platform.h"

/*==============================================================================
 * Remap interrupt names to BSP names
 */
#define BSP_INTERRUPT_ID_GPIOA      INT_GPIOA
#define BSP_INTERRUPT_ID_GPIOB      INT_GPIOB
#define BSP_INTERRUPT_ID_GPIOC      INT_GPIOC
#define BSP_INTERRUPT_ID_GPIOD      INT_GPIOD
#define BSP_INTERRUPT_ID_GPIOE      INT_GPIOE
#define BSP_INTERRUPT_ID_UART0      INT_UART0
#define BSP_INTERRUPT_ID_UART1      INT_UART1
#define BSP_INTERRUPT_ID_SSI0       INT_SSI0
#define BSP_INTERRUPT_ID_I2C0       INT_I2C0
#define BSP_INTERRUPT_ID_PWM0_FAULT INT_PWM0_FAULT
#define BSP_INTERRUPT_ID_PWM0_0     INT_PWM0_0
#define BSP_INTERRUPT_ID_PWM0_1     INT_PWM0_1
#define BSP_INTERRUPT_ID_PWM0_2     INT_PWM0_2
#define BSP_INTERRUPT_ID_QEI0       INT_QEI0
#define BSP_INTERRUPT_ID_ADC0SS0    INT_ADC0SS0
#define BSP_INTERRUPT_ID_ADC0SS1    INT_ADC0SS1
#define BSP_INTERRUPT_ID_ADC0SS2    INT_ADC0SS2
#define BSP_INTERRUPT_ID_ADC0SS3    INT_ADC0SS3
#define BSP_INTERRUPT_ID_WATCHDOG   INT_WATCHDOG
#define BSP_INTERRUPT_ID_TIMER0A    INT_TIMER0A
#define BSP_INTERRUPT_ID_TIMER0B    INT_TIMER0B
#define BSP_INTERRUPT_ID_TIMER1A    INT_TIMER1A
#define BSP_INTERRUPT_ID_TIMER1B    INT_TIMER1B
#define BSP_INTERRUPT_ID_TIMER2A    INT_TIMER2A
#define BSP_INTERRUPT_ID_TIMER2B    INT_TIMER2B
#define BSP_INTERRUPT_ID_COMP0      INT_COMP0
#define BSP_INTERRUPT_ID_COMP1      INT_COMP1
#define BSP_INTERRUPT_ID_COMP2      INT_COMP2
#define BSP_INTERRUPT_ID_SYSCTL     INT_SYSCTL
#define BSP_INTERRUPT_ID_FLASH      INT_FLASH
#define BSP_INTERRUPT_ID_GPIOF      INT_GPIOF
#define BSP_INTERRUPT_ID_GPIOG      INT_GPIOG
#define BSP_INTERRUPT_ID_GPIOH      INT_GPIOH
#define BSP_INTERRUPT_ID_UART2      INT_UART2
#define BSP_INTERRUPT_ID_SSI1       INT_SSI1
#define BSP_INTERRUPT_ID_TIMER3A    INT_TIMER3A
#define BSP_INTERRUPT_ID_TIMER3B    INT_TIMER3B
#define BSP_INTERRUPT_ID_I2C1       INT_I2C1
#define BSP_INTERRUPT_ID_CAN0       INT_CAN0
#define BSP_INTERRUPT_ID_CAN1       INT_CAN1
#define BSP_INTERRUPT_ID_EMAC0      INT_EMAC0
#define BSP_INTERRUPT_ID_HIBERNATE  INT_HIBERNATE
#define BSP_INTERRUPT_ID_USB0       INT_USB0
#define BSP_INTERRUPT_ID_PWM0_3     INT_PWM0_3
#define BSP_INTERRUPT_ID_UDMA       INT_UDMA
#define BSP_INTERRUPT_ID_UDMAERR    INT_UDMAERR
#define BSP_INTERRUPT_ID_ADC1SS0    INT_ADC1SS0
#define BSP_INTERRUPT_ID_ADC1SS1    INT_ADC1SS1
#define BSP_INTERRUPT_ID_ADC1SS2    INT_ADC1SS2
#define BSP_INTERRUPT_ID_ADC1SS3    INT_ADC1SS3
#define BSP_INTERRUPT_ID_EPI0       INT_EPI0
#define BSP_INTERRUPT_ID_GPIOJ      INT_GPIOJ
#define BSP_INTERRUPT_ID_GPIOK      INT_GPIOK
#define BSP_INTERRUPT_ID_GPIOL      INT_GPIOL
#define BSP_INTERRUPT_ID_SSI2       INT_SSI2
#define BSP_INTERRUPT_ID_SSI3       INT_SSI3
#define BSP_INTERRUPT_ID_UART3      INT_UART3
#define BSP_INTERRUPT_ID_UART4      INT_UART4
#define BSP_INTERRUPT_ID_UART5      INT_UART5
#define BSP_INTERRUPT_ID_UART6      INT_UART6
#define BSP_INTERRUPT_ID_UART7      INT_UART7
#define BSP_INTERRUPT_ID_I2C2       INT_I2C2
#define BSP_INTERRUPT_ID_I2C3       INT_I2C3
#define BSP_INTERRUPT_ID_TIMER4A    INT_TIMER4A
#define BSP_INTERRUPT_ID_TIMER4B    INT_TIMER4B
#define BSP_INTERRUPT_ID_TIMER5A    INT_TIMER5A
#define BSP_INTERRUPT_ID_TIMER5B    INT_TIMER5B
#define BSP_INTERRUPT_ID_SYSEXC     INT_SYSEXC
#define BSP_INTERRUPT_ID_I2C4       INT_I2C4
#define BSP_INTERRUPT_ID_I2C5       INT_I2C5
#define BSP_INTERRUPT_ID_GPIOM      INT_GPIOM
#define BSP_INTERRUPT_ID_GPION      INT_GPION
#define BSP_INTERRUPT_ID_TAMPER0    INT_TAMPER0
#define BSP_INTERRUPT_ID_GPIOP      INT_GPIOP0
#define BSP_INTERRUPT_ID_GPIOP0     INT_GPIOP0
#define BSP_INTERRUPT_ID_GPIOP1     INT_GPIOP1
#define BSP_INTERRUPT_ID_GPIOP2     INT_GPIOP2
#define BSP_INTERRUPT_ID_GPIOP3     INT_GPIOP3
#define BSP_INTERRUPT_ID_GPIOP4     INT_GPIOP4
#define BSP_INTERRUPT_ID_GPIOP5     INT_GPIOP5
#define BSP_INTERRUPT_ID_GPIOP6     INT_GPIOP6
#define BSP_INTERRUPT_ID_GPIOP7     INT_GPIOP7
#define BSP_INTERRUPT_ID_GPIOQ      INT_GPIOQ0
#define BSP_INTERRUPT_ID_GPIOQ0     INT_GPIOQ0
#define BSP_INTERRUPT_ID_GPIOQ1     INT_GPIOQ1
#define BSP_INTERRUPT_ID_GPIOQ2     INT_GPIOQ2
#define BSP_INTERRUPT_ID_GPIOQ3     INT_GPIOQ3
#define BSP_INTERRUPT_ID_GPIOQ4     INT_GPIOQ4
#define BSP_INTERRUPT_ID_GPIOQ5     INT_GPIOQ5
#define BSP_INTERRUPT_ID_GPIOQ6     INT_GPIOQ6
#define BSP_INTERRUPT_ID_GPIOQ7     INT_GPIOQ7
#if defined(INT_GPIOR)
#define BSP_INTERRUPT_ID_GPIOR INT_GPIOR
#endif
#if defined(INT_GPIOS)
#define BSP_INTERRUPT_ID_GPIOS INT_GPIOS
#endif
#if defined(INT_LDC0)
#define BSP_INTERRUPT_ID_LCD0 INT_LCD0
#endif
#define BSP_INTERRUPT_ID_TIMER6A INT_TIMER6A
#define BSP_INTERRUPT_ID_TIMER6B INT_TIMER6B
#define BSP_INTERRUPT_ID_TIMER7A INT_TIMER7A
#define BSP_INTERRUPT_ID_TIMER7B INT_TIMER7B
#define BSP_INTERRUPT_ID_I2C6    INT_I2C6
#define BSP_INTERRUPT_ID_I2C7    INT_I2C7
#define BSP_INTERRUPT_ID_I2C8    INT_I2C8
#define BSP_INTERRUPT_ID_I2C9    INT_I2C9
#if defined(INT_GPIOT)
#define BSP_INTERRUPT_ID_GPIOT INT_GPIOT
#endif

/*==============================================================================
 * Select the group/sub-group mode. This determines how many of the 3 priority
 * bits are used to define the group vs. sub-group.
 */
#define BSP_INTERRUPT_PRIORITY_GROUP_MODE BSP_INTERRUPT_PRIORITY_MODE_GRP8_SUB1

/*==============================================================================
 * Globals defining the platform specific mappings
 */
extern const uint32_t bsp_Interrupt_groupPriorityTableExceptions[];
extern const uint32_t bsp_Interrupt_groupPriorityTableInterrupts[];
