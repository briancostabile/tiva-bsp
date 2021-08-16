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
 * @file bsp_Gpio_tm4c123.h
 * @brief Contains the tm4c123 core GPIO defines. Note: Not all of these IOs
 *        may be available
 */
#pragma once

#include "bsp_Types.h"
#include "bsp_Interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_Gpio.h"
#include "driverlib/pin_map.h"

/* Port A */
#define BSP_GPIO_PORT_ID_PA0           0
#define BSP_GPIO_BASE_ADDR_PA0         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA0        0
#define BSP_GPIO_BIT_MASK_PA0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PA0)

#define BSP_GPIO_PORT_ID_PA1           0
#define BSP_GPIO_BASE_ADDR_PA1         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA1        1
#define BSP_GPIO_BIT_MASK_PA1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PA1)

#define BSP_GPIO_PORT_ID_PA2           0
#define BSP_GPIO_BASE_ADDR_PA2         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA2        2
#define BSP_GPIO_BIT_MASK_PA2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PA2)

#define BSP_GPIO_PORT_ID_PA3           0
#define BSP_GPIO_BASE_ADDR_PA3         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA3        3
#define BSP_GPIO_BIT_MASK_PA3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PA3)

#define BSP_GPIO_PORT_ID_PA4           0
#define BSP_GPIO_BASE_ADDR_PA4         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA4        4
#define BSP_GPIO_BIT_MASK_PA4          (0x00000001 << BSP_GPIO_BIT_OFFSET_TPA4)

#define BSP_GPIO_PORT_ID_PA5           0
#define BSP_GPIO_BASE_ADDR_PA5         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA5        5
#define BSP_GPIO_BIT_MASK_PA5          (0x00000001 << BSP_GPIO_BIT_OFFSET_TPA5)

#define BSP_GPIO_PORT_ID_PA6           0
#define BSP_GPIO_BASE_ADDR_PA6         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA6        6
#define BSP_GPIO_BIT_MASK_PA6          (0x00000001 << BSP_GPIO_BIT_OFFSET_TPA6)

#define BSP_GPIO_PORT_ID_PA7           0
#define BSP_GPIO_BASE_ADDR_PA7         GPIO_PORTA_BASE
#define BSP_GPIO_BIT_OFFSET_PA7        7
#define BSP_GPIO_BIT_MASK_PA7          (0x00000001 << BSP_GPIO_BIT_OFFSET_TPA7)


/* Port B */
#define BSP_GPIO_PORT_ID_PB0           1
#define BSP_GPIO_BASE_ADDR_PB0         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB0        0
#define BSP_GPIO_BIT_MASK_PB0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB0)

#define BSP_GPIO_PORT_ID_PB1           1
#define BSP_GPIO_BASE_ADDR_PB1         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB1        1
#define BSP_GPIO_BIT_MASK_PB1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB1)

#define BSP_GPIO_PORT_ID_PB2           1
#define BSP_GPIO_BASE_ADDR_PB2         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB2        2
#define BSP_GPIO_BIT_MASK_PB2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB2)

#define BSP_GPIO_PORT_ID_PB3           1
#define BSP_GPIO_BASE_ADDR_PB3         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB3        3
#define BSP_GPIO_BIT_MASK_PB3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB3)

#define BSP_GPIO_PORT_ID_PB4           1
#define BSP_GPIO_BASE_ADDR_PB4         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB4        4
#define BSP_GPIO_BIT_MASK_PB4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB4)
#define BSP_GPIO_ALT_FUNC_PB4_SSI2CLK  2
#define BSP_GPIO_ALT_FUNC_PB4_M0PWM2   4

#define BSP_GPIO_PORT_ID_PB5           1
#define BSP_GPIO_BASE_ADDR_PB5         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB5        5
#define BSP_GPIO_BIT_MASK_PB5          (0x00000001 << BSP_GPIO_BIT_OFFSET_TPB5)

#define BSP_GPIO_PORT_ID_PB6           1
#define BSP_GPIO_BASE_ADDR_PB6         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB6        6
#define BSP_GPIO_BIT_MASK_PB6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB6)

#define BSP_GPIO_PORT_ID_PB7           1
#define BSP_GPIO_BASE_ADDR_PB7         GPIO_PORTB_BASE
#define BSP_GPIO_BIT_OFFSET_PB7        7
#define BSP_GPIO_BIT_MASK_PB7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PB7)


/* Port C */
#define BSP_GPIO_PORT_ID_PC0            2
#define BSP_GPIO_BASE_ADDR_PC0          GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC0         0
#define BSP_GPIO_BIT_MASK_PC0           (0x00000001 << BSP_GPIO_BIT_OFFSET_PC0)

#define BSP_GPIO_PORT_ID_PC1            2
#define BSP_GPIO_BASE_ADDR_PC1          GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC1         1
#define BSP_GPIO_BIT_MASK_PC1           (0x00000001 << BSP_GPIO_BIT_OFFSET_PC1)

#define BSP_GPIO_PORT_ID_PC2          2
#define BSP_GPIO_BASE_ADDR_PC2        GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC2       2
#define BSP_GPIO_BIT_MASK_PC2         (0x00000001 << BSP_GPIO_BIT_OFFSET_PC2)

#define BSP_GPIO_PORT_ID_PC3          2
#define BSP_GPIO_BASE_ADDR_PC3        GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC3       3
#define BSP_GPIO_BIT_MASK_PC3         (0x00000001 << BSP_GPIO_BIT_OFFSET_PC3)

#define BSP_GPIO_PORT_ID_PC4          2
#define BSP_GPIO_BASE_ADDR_PC4        GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC4       4
#define BSP_GPIO_BIT_MASK_PC4         (0x00000001 << BSP_GPIO_BIT_OFFSET_PC4)

#define BSP_GPIO_PORT_ID_PC5          2
#define BSP_GPIO_BASE_ADDR_PC5        GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC5       5
#define BSP_GPIO_BIT_MASK_PC5         (0x00000001 << BSP_GPIO_BIT_OFFSET_PC5)

#define BSP_GPIO_PORT_ID_PC6           2
#define BSP_GPIO_BASE_ADDR_PC6         GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC6        6
#define BSP_GPIO_BIT_MASK_PC6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PC6)

#define BSP_GPIO_PORT_ID_PC7           2
#define BSP_GPIO_BASE_ADDR_PC7         GPIO_PORTC_BASE
#define BSP_GPIO_BIT_OFFSET_PC7        7
#define BSP_GPIO_BIT_MASK_PC7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PC7)


/* Port D */
#define BSP_GPIO_PORT_ID_PD0           3
#define BSP_GPIO_BASE_ADDR_PD0         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD0        0
#define BSP_GPIO_BIT_MASK_PD0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD0)

#define BSP_GPIO_PORT_ID_PD1           3
#define BSP_GPIO_BASE_ADDR_PD1         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD1        1
#define BSP_GPIO_BIT_MASK_PD1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD1)

#define BSP_GPIO_PORT_ID_PD2           3
#define BSP_GPIO_BASE_ADDR_PD2         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD2        2
#define BSP_GPIO_BIT_MASK_PD2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD2)

#define BSP_GPIO_PORT_ID_PD3           3
#define BSP_GPIO_BASE_ADDR_PD3         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD3        3
#define BSP_GPIO_BIT_MASK_PD3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD3)

#define BSP_GPIO_PORT_ID_PD4           3
#define BSP_GPIO_BASE_ADDR_PD4         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD4        4
#define BSP_GPIO_BIT_MASK_PD4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD4)

#define BSP_GPIO_PORT_ID_PD5           3
#define BSP_GPIO_BASE_ADDR_PD5         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD5        5
#define BSP_GPIO_BIT_MASK_PD5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD5)

#define BSP_GPIO_PORT_ID_PD6           3
#define BSP_GPIO_BASE_ADDR_PD6         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD6        6
#define BSP_GPIO_BIT_MASK_PD6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD6)

#define BSP_GPIO_PORT_ID_PD7           3
#define BSP_GPIO_BASE_ADDR_PD7         GPIO_PORTD_BASE
#define BSP_GPIO_BIT_OFFSET_PD7        7
#define BSP_GPIO_BIT_MASK_PD7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PD7)


/* Port E */
#define BSP_GPIO_PORT_ID_PE0           4
#define BSP_GPIO_BASE_ADDR_PE0         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE0        0
#define BSP_GPIO_BIT_MASK_PE0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE0)

#define BSP_GPIO_PORT_ID_PE1           4
#define BSP_GPIO_BASE_ADDR_PE1         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE1        1
#define BSP_GPIO_BIT_MASK_PE1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE1)

#define BSP_GPIO_PORT_ID_PE2           4
#define BSP_GPIO_BASE_ADDR_PE2         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE2        2
#define BSP_GPIO_BIT_MASK_PE2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE2)

#define BSP_GPIO_PORT_ID_PE3           4
#define BSP_GPIO_BASE_ADDR_PE3         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE3        3
#define BSP_GPIO_BIT_MASK_PE3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE3)

#define BSP_GPIO_PORT_ID_PE4           4
#define BSP_GPIO_BASE_ADDR_PE4         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE4        4
#define BSP_GPIO_BIT_MASK_PE4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE4)

#define BSP_GPIO_PORT_ID_PE5           4
#define BSP_GPIO_BASE_ADDR_PE5         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE5        5
#define BSP_GPIO_BIT_MASK_PE5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE5)

#define BSP_GPIO_PORT_ID_PE6           4
#define BSP_GPIO_BASE_ADDR_PE6         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE6        6
#define BSP_GPIO_BIT_MASK_PE6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE6)

#define BSP_GPIO_PORT_ID_PE7           4
#define BSP_GPIO_BASE_ADDR_PE7         GPIO_PORTE_BASE
#define BSP_GPIO_BIT_OFFSET_PE7        5
#define BSP_GPIO_BIT_MASK_PE7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PE7)


/* Port F */
#define BSP_GPIO_PORT_ID_PF0           5
#define BSP_GPIO_BASE_ADDR_PF0         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF0        0
#define BSP_GPIO_BIT_MASK_PF0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF0)

#define BSP_GPIO_PORT_ID_PF1           5
#define BSP_GPIO_BASE_ADDR_PF1         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF1        1
#define BSP_GPIO_BIT_MASK_PF1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF1)

#define BSP_GPIO_PORT_ID_PF2           5
#define BSP_GPIO_BASE_ADDR_PF2         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF2        2
#define BSP_GPIO_BIT_MASK_PF2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF2)

#define BSP_GPIO_PORT_ID_PF3           5
#define BSP_GPIO_BASE_ADDR_PF3         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF3        3
#define BSP_GPIO_BIT_MASK_PF3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF3)

#define BSP_GPIO_PORT_ID_PF4           5
#define BSP_GPIO_BASE_ADDR_PF4         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF4        4
#define BSP_GPIO_BIT_MASK_PF4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF4)

#define BSP_GPIO_PORT_ID_PF5           5
#define BSP_GPIO_BASE_ADDR_PF5         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF5        5
#define BSP_GPIO_BIT_MASK_PF5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF5)

#define BSP_GPIO_PORT_ID_PF6           5
#define BSP_GPIO_BASE_ADDR_PF6         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF6        6
#define BSP_GPIO_BIT_MASK_PF6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF6)

#define BSP_GPIO_PORT_ID_PF7           5
#define BSP_GPIO_BASE_ADDR_PF7         GPIO_PORTF_BASE
#define BSP_GPIO_BIT_OFFSET_PF7        7
#define BSP_GPIO_BIT_MASK_PF7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PF7)

/* Port G */
#define BSP_GPIO_PORT_ID_PG0           6
#define BSP_GPIO_BASE_ADDR_PG0         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG0        0
#define BSP_GPIO_BIT_MASK_PG0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG0)

#define BSP_GPIO_PORT_ID_PG1           6
#define BSP_GPIO_BASE_ADDR_PG1         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG1        1
#define BSP_GPIO_BIT_MASK_PG1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG1)

#define BSP_GPIO_PORT_ID_PG2           6
#define BSP_GPIO_BASE_ADDR_PG2         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG2        2
#define BSP_GPIO_BIT_MASK_PG2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG2)

#define BSP_GPIO_PORT_ID_PG3           6
#define BSP_GPIO_BASE_ADDR_PG3         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG3        3
#define BSP_GPIO_BIT_MASK_PG3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG3)

#define BSP_GPIO_PORT_ID_PG4           6
#define BSP_GPIO_BASE_ADDR_PG4         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG4        4
#define BSP_GPIO_BIT_MASK_PG4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG4)
#define BSP_GPIO_ALT_FUNC_PG4_U2RX     1

#define BSP_GPIO_PORT_ID_PG5           6
#define BSP_GPIO_BASE_ADDR_PG5         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG5        5
#define BSP_GPIO_BIT_MASK_PG5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG5)
#define BSP_GPIO_ALT_FUNC_PG5_U2TX     1

#define BSP_GPIO_PORT_ID_PG6           6
#define BSP_GPIO_BASE_ADDR_PG6         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG6        6
#define BSP_GPIO_BIT_MASK_PG6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG6)

#define BSP_GPIO_PORT_ID_PG7           6
#define BSP_GPIO_BASE_ADDR_PG7         GPIO_PORTG_BASE
#define BSP_GPIO_BIT_OFFSET_PG7        7
#define BSP_GPIO_BIT_MASK_PG7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PG7)


/* Port H */
#define BSP_GPIO_PORT_ID_PH0           7
#define BSP_GPIO_BASE_ADDR_PH0         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH0        0
#define BSP_GPIO_BIT_MASK_PH0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH0)

#define BSP_GPIO_PORT_ID_PH1           7
#define BSP_GPIO_BASE_ADDR_PH1         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH1        1
#define BSP_GPIO_BIT_MASK_PH1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH1)

#define BSP_GPIO_PORT_ID_PH2           7
#define BSP_GPIO_BASE_ADDR_PH2         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH2        2
#define BSP_GPIO_BIT_MASK_PH2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH2)

#define BSP_GPIO_PORT_ID_PH3           7
#define BSP_GPIO_BASE_ADDR_PH3         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH3        3
#define BSP_GPIO_BIT_MASK_PH3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH3)

#define BSP_GPIO_PORT_ID_PH4           7
#define BSP_GPIO_BASE_ADDR_PH4         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH4        4
#define BSP_GPIO_BIT_MASK_PH4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH4)

#define BSP_GPIO_PORT_ID_PH5           7
#define BSP_GPIO_BASE_ADDR_PH5         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH5        5
#define BSP_GPIO_BIT_MASK_PH5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH5)

#define BSP_GPIO_PORT_ID_PH6           7
#define BSP_GPIO_BASE_ADDR_PH6         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH6        6
#define BSP_GPIO_BIT_MASK_PH6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH6)

#define BSP_GPIO_PORT_ID_PH7           7
#define BSP_GPIO_BASE_ADDR_PH7         GPIO_PORTH_BASE
#define BSP_GPIO_BIT_OFFSET_PH7        7
#define BSP_GPIO_BIT_MASK_PH7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PH7)


/* Port J */
#define BSP_GPIO_PORT_ID_PJ0           8
#define BSP_GPIO_BASE_ADDR_PJ0         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ0        0
#define BSP_GPIO_BIT_MASK_PJ0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ0)
#define BSP_GPIO_ALT_FUNC_PJ0_U4RX     1

#define BSP_GPIO_PORT_ID_PJ1           8
#define BSP_GPIO_BASE_ADDR_PJ1         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ1        1
#define BSP_GPIO_BIT_MASK_PJ1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ1)
#define BSP_GPIO_ALT_FUNC_PJ1_U4TX     1

#define BSP_GPIO_PORT_ID_PJ2           8
#define BSP_GPIO_BASE_ADDR_PJ2         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ2        2
#define BSP_GPIO_BIT_MASK_PJ2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ2)
#define BSP_GPIO_ALT_FUNC_PJ2_U5RX     1

#define BSP_GPIO_PORT_ID_PJ3           8
#define BSP_GPIO_BASE_ADDR_PJ3         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ3        3
#define BSP_GPIO_BIT_MASK_PJ3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ3)
#define BSP_GPIO_ALT_FUNC_PJ3_U5TX     1

#define BSP_GPIO_PORT_ID_PJ4           8
#define BSP_GPIO_BASE_ADDR_PJ4         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ4        4
#define BSP_GPIO_BIT_MASK_PJ4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ4)
#define BSP_GPIO_ALT_FUNC_PJ4_U6RX     1

#define BSP_GPIO_PORT_ID_PJ5           8
#define BSP_GPIO_BASE_ADDR_PJ5         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ5        5
#define BSP_GPIO_BIT_MASK_PJ5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ5)
#define BSP_GPIO_ALT_FUNC_PJ5_U6TX     1

#define BSP_GPIO_PORT_ID_PJ6           8
#define BSP_GPIO_BASE_ADDR_PJ6         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ6        6
#define BSP_GPIO_BIT_MASK_PJ6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ6)

#define BSP_GPIO_PORT_ID_PJ7           8
#define BSP_GPIO_BASE_ADDR_PJ7         GPIO_PORTJ_BASE
#define BSP_GPIO_BIT_OFFSET_PJ7        7
#define BSP_GPIO_BIT_MASK_PJ7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PJ7)


/* Port K */
#define BSP_GPIO_PORT_ID_PK0           9
#define BSP_GPIO_BASE_ADDR_PK0         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK0        0
#define BSP_GPIO_BIT_MASK_PK0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK0)

#define BSP_GPIO_PORT_ID_PK1           9
#define BSP_GPIO_BASE_ADDR_PK1         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK1        1
#define BSP_GPIO_BIT_MASK_PK1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK1)

#define BSP_GPIO_PORT_ID_PK2           9
#define BSP_GPIO_BASE_ADDR_PK2         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK2        2
#define BSP_GPIO_BIT_MASK_PK2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK2)

#define BSP_GPIO_PORT_ID_PK3           9
#define BSP_GPIO_BASE_ADDR_PK3         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK3        3
#define BSP_GPIO_BIT_MASK_PK3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK3)

#define BSP_GPIO_PORT_ID_PK4           9
#define BSP_GPIO_BASE_ADDR_PK4         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK4        4
#define BSP_GPIO_BIT_MASK_PK4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK4)
#define BSP_GPIO_ALT_FUNC_PK4_U7RX     1

#define BSP_GPIO_PORT_ID_PK5           9
#define BSP_GPIO_BASE_ADDR_PK5         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK5        5
#define BSP_GPIO_BIT_MASK_PK5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK5)
#define BSP_GPIO_ALT_FUNC_PK5_U7TX     1

#define BSP_GPIO_PORT_ID_PK6           9
#define BSP_GPIO_BASE_ADDR_PK6         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK6        6
#define BSP_GPIO_BIT_MASK_PK6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK6)

#define BSP_GPIO_PORT_ID_PK7           9
#define BSP_GPIO_BASE_ADDR_PK7         GPIO_PORTK_BASE
#define BSP_GPIO_BIT_OFFSET_PK7        7
#define BSP_GPIO_BIT_MASK_PK7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PK7)


/* Port L */
#define BSP_GPIO_PORT_ID_PL0           10
#define BSP_GPIO_BASE_ADDR_PL0         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL0        0
#define BSP_GPIO_BIT_MASK_PL0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL0)

#define BSP_GPIO_PORT_ID_PL1           10
#define BSP_GPIO_BASE_ADDR_PL1         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL1        1
#define BSP_GPIO_BIT_MASK_PL1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL1)

#define BSP_GPIO_PORT_ID_PL2           10
#define BSP_GPIO_BASE_ADDR_PL2         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL2        2
#define BSP_GPIO_BIT_MASK_PL2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL2)

#define BSP_GPIO_PORT_ID_PL3           10
#define BSP_GPIO_BASE_ADDR_PL3         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL3        3
#define BSP_GPIO_BIT_MASK_PL3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL3)

#define BSP_GPIO_PORT_ID_PL4           10
#define BSP_GPIO_BASE_ADDR_PL4         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL4        4
#define BSP_GPIO_BIT_MASK_PL4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL4)

#define BSP_GPIO_PORT_ID_PL5           10
#define BSP_GPIO_BASE_ADDR_PL5         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL5        5
#define BSP_GPIO_BIT_MASK_PL5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL5)

#define BSP_GPIO_PORT_ID_PL6           10
#define BSP_GPIO_BASE_ADDR_PL6         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL6        6
#define BSP_GPIO_BIT_MASK_PL6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL6)

#define BSP_GPIO_PORT_ID_PL7           10
#define BSP_GPIO_BASE_ADDR_PL7         GPIO_PORTL_BASE
#define BSP_GPIO_BIT_OFFSET_PL7        7
#define BSP_GPIO_BIT_MASK_PL7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PL7)


/* Port M */
#define BSP_GPIO_PORT_ID_PM0           11
#define BSP_GPIO_BASE_ADDR_PM0         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM0        0
#define BSP_GPIO_BIT_MASK_PM0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM0)

#define BSP_GPIO_PORT_ID_PM1           11
#define BSP_GPIO_BASE_ADDR_PM1         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM1        1
#define BSP_GPIO_BIT_MASK_PM1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM1)

#define BSP_GPIO_PORT_ID_PM2           11
#define BSP_GPIO_BASE_ADDR_PM2         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM2        2
#define BSP_GPIO_BIT_MASK_PM2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM2)

#define BSP_GPIO_PORT_ID_PM3           11
#define BSP_GPIO_BASE_ADDR_PM3         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM3        3
#define BSP_GPIO_BIT_MASK_PM3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM3)

#define BSP_GPIO_PORT_ID_PM4           11
#define BSP_GPIO_BASE_ADDR_PM4         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM4        4
#define BSP_GPIO_BIT_MASK_PM4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM4)

#define BSP_GPIO_PORT_ID_PM5           11
#define BSP_GPIO_BASE_ADDR_PM5         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM5        5
#define BSP_GPIO_BIT_MASK_PM5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM5)

#define BSP_GPIO_PORT_ID_PM6           11
#define BSP_GPIO_BASE_ADDR_PM6         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM6        6
#define BSP_GPIO_BIT_MASK_PM6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM6)

#define BSP_GPIO_PORT_ID_PM7           11
#define BSP_GPIO_BASE_ADDR_PM7         GPIO_PORTM_BASE
#define BSP_GPIO_BIT_OFFSET_PM7        7
#define BSP_GPIO_BIT_MASK_PM7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PM7)


/* Port N */
#define BSP_GPIO_PORT_ID_PN0           12
#define BSP_GPIO_BASE_ADDR_PN0         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN0        0
#define BSP_GPIO_BIT_MASK_PN0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN0)

#define BSP_GPIO_PORT_ID_PN1           12
#define BSP_GPIO_BASE_ADDR_PN1         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN1        1
#define BSP_GPIO_BIT_MASK_PN1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN1)

#define BSP_GPIO_PORT_ID_PN2           12
#define BSP_GPIO_BASE_ADDR_PN2         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN2        2
#define BSP_GPIO_BIT_MASK_PN2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN2)

#define BSP_GPIO_PORT_ID_PN3           12
#define BSP_GPIO_BASE_ADDR_PN3         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN3        3
#define BSP_GPIO_BIT_MASK_PN3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN3)

#define BSP_GPIO_PORT_ID_PN4           12
#define BSP_GPIO_BASE_ADDR_PN4         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN4        4
#define BSP_GPIO_BIT_MASK_PN4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN4)

#define BSP_GPIO_PORT_ID_PN5           12
#define BSP_GPIO_BASE_ADDR_PN5         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN5        5
#define BSP_GPIO_BIT_MASK_PN5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN5)

#define BSP_GPIO_PORT_ID_PN6           12
#define BSP_GPIO_BASE_ADDR_PN6         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN6        6
#define BSP_GPIO_BIT_MASK_PN6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN6)

#define BSP_GPIO_PORT_ID_PN7           12
#define BSP_GPIO_BASE_ADDR_PN7         GPIO_PORTN_BASE
#define BSP_GPIO_BIT_OFFSET_PN7        7
#define BSP_GPIO_BIT_MASK_PN7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PN7)


/* Port P */
#define BSP_GPIO_PORT_ID_PP0           13
#define BSP_GPIO_BASE_ADDR_PP0         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP0        0
#define BSP_GPIO_BIT_MASK_PP0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP0)

#define BSP_GPIO_PORT_ID_PP1           13
#define BSP_GPIO_BASE_ADDR_PP1         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP1        1
#define BSP_GPIO_BIT_MASK_PP1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP1)

#define BSP_GPIO_PORT_ID_PP2           13
#define BSP_GPIO_BASE_ADDR_PP2         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP2        2
#define BSP_GPIO_BIT_MASK_PP2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP2)

#define BSP_GPIO_PORT_ID_PP3           13
#define BSP_GPIO_BASE_ADDR_PP3         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP3        3
#define BSP_GPIO_BIT_MASK_PP3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP3)

#define BSP_GPIO_PORT_ID_PP4           13
#define BSP_GPIO_BASE_ADDR_PP4         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP4        4
#define BSP_GPIO_BIT_MASK_PP4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP4)

#define BSP_GPIO_PORT_ID_PP5           13
#define BSP_GPIO_BASE_ADDR_PP5         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP5        5
#define BSP_GPIO_BIT_MASK_PP5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP5)

#define BSP_GPIO_PORT_ID_PP6           13
#define BSP_GPIO_BASE_ADDR_PP6         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP6        6
#define BSP_GPIO_BIT_MASK_PP6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP6)

#define BSP_GPIO_PORT_ID_PP7           13
#define BSP_GPIO_BASE_ADDR_PP7         GPIO_PORTP_BASE
#define BSP_GPIO_BIT_OFFSET_PP7        7
#define BSP_GPIO_BIT_MASK_PP7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PP7)


/* Port Q */
#define BSP_GPIO_PORT_ID_PQ0           14
#define BSP_GPIO_BASE_ADDR_PQ0         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ0        0
#define BSP_GPIO_BIT_MASK_PQ0          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ0)

#define BSP_GPIO_PORT_ID_PQ1           14
#define BSP_GPIO_BASE_ADDR_PQ1         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ1        1
#define BSP_GPIO_BIT_MASK_PQ1          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ1)

#define BSP_GPIO_PORT_ID_PQ2           14
#define BSP_GPIO_BASE_ADDR_PQ2         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ2        2
#define BSP_GPIO_BIT_MASK_PQ2          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ2)

#define BSP_GPIO_PORT_ID_PQ3           14
#define BSP_GPIO_BASE_ADDR_PQ3         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ3        3
#define BSP_GPIO_BIT_MASK_PQ3          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ3)

#define BSP_GPIO_PORT_ID_PQ4           14
#define BSP_GPIO_BASE_ADDR_PQ4         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ4        4
#define BSP_GPIO_BIT_MASK_PQ4          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ4)

#define BSP_GPIO_PORT_ID_PQ5           14
#define BSP_GPIO_BASE_ADDR_PQ5         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ5        5
#define BSP_GPIO_BIT_MASK_PQ5          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ5)

#define BSP_GPIO_PORT_ID_PQ6           14
#define BSP_GPIO_BASE_ADDR_PQ6         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ6        6
#define BSP_GPIO_BIT_MASK_PQ6          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ6)

#define BSP_GPIO_PORT_ID_PQ7           14
#define BSP_GPIO_BASE_ADDR_PQ7         GPIO_PORTQ_BASE
#define BSP_GPIO_BIT_OFFSET_PQ7        7
#define BSP_GPIO_BIT_MASK_PQ7          (0x00000001 << BSP_GPIO_BIT_OFFSET_PQ7)




/*============================================================================*/
#define BSP_GPIO_PORT_ID_A 0
#define BSP_GPIO_PORT_ID_B 1
#define BSP_GPIO_PORT_ID_C 2
#define BSP_GPIO_PORT_ID_D 3
#define BSP_GPIO_PORT_ID_E 4
#define BSP_GPIO_PORT_ID_F 5
#define BSP_GPIO_PORT_ID_G 6
#define BSP_GPIO_PORT_ID_H 7
#define BSP_GPIO_PORT_ID_J 8
#define BSP_GPIO_PORT_ID_K 9
#define BSP_GPIO_PORT_ID_L 10
#define BSP_GPIO_PORT_ID_M 11
#define BSP_GPIO_PORT_ID_N 12
#define BSP_GPIO_PORT_ID_P 13
#define BSP_GPIO_PORT_ID_Q 14
typedef uint8_t bsp_Gpio_PortId_t;

/*============================================================================*/
#define BSP_GPIO_PORT_ID_NUM_PORTS 6 // Depends on Chip version

/*============================================================================*/
#define BSP_GPIO_PIN_OFFSET_NUM_PINS_PER_PORT 8
typedef uint8_t bsp_Gpio_PinOffset_t;

/*============================================================================*/
typedef void (* bsp_Gpio_InputHandler_t)( bsp_Gpio_PortId_t    portId,
                                          bsp_Gpio_PinOffset_t pinOffset );

/*============================================================================*/
typedef struct {
    uint32_t                 baseAddr;
    uint32_t                 sysCtrlAddr;
    bsp_Interrupt_Id_t       intId;
    bool_t                   bitInterruptable;
    bsp_Gpio_InputHandler_t* handlerTable;
}bsp_Gpio_PlatformPortInfo_t;


/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Gpio_interruptHandlerPortA( void );
void bsp_Gpio_interruptHandlerPortB( void );
void bsp_Gpio_interruptHandlerPortC( void );
void bsp_Gpio_interruptHandlerPortD( void );
void bsp_Gpio_interruptHandlerPortE( void );
void bsp_Gpio_interruptHandlerPortF( void );
void bsp_Gpio_interruptHandlerPortG( void );
void bsp_Gpio_interruptHandlerPortH( void );
void bsp_Gpio_interruptHandlerPortJ( void );
void bsp_Gpio_interruptHandlerPortK( void );
void bsp_Gpio_interruptHandlerPortL( void );
void bsp_Gpio_interruptHandlerPortM( void );
void bsp_Gpio_interruptHandlerPortN( void );
void bsp_Gpio_interruptHandlerPortP0( void );
void bsp_Gpio_interruptHandlerPortP1( void );
void bsp_Gpio_interruptHandlerPortP2( void );
void bsp_Gpio_interruptHandlerPortP3( void );
void bsp_Gpio_interruptHandlerPortP4( void );
void bsp_Gpio_interruptHandlerPortP5( void );
void bsp_Gpio_interruptHandlerPortP6( void );
void bsp_Gpio_interruptHandlerPortP7( void );
void bsp_Gpio_interruptHandlerPortQ0( void );
void bsp_Gpio_interruptHandlerPortQ1( void );
void bsp_Gpio_interruptHandlerPortQ2( void );
void bsp_Gpio_interruptHandlerPortQ3( void );
void bsp_Gpio_interruptHandlerPortQ4( void );
void bsp_Gpio_interruptHandlerPortQ5( void );
void bsp_Gpio_interruptHandlerPortQ6( void );
void bsp_Gpio_interruptHandlerPortQ7( void );


/*==============================================================================
 *                                Globals
 *============================================================================*/

/*============================================================================*/
extern const bsp_Gpio_PlatformPortInfo_t bsp_Gpio_platformPortInfoTable[BSP_GPIO_PORT_ID_NUM_PORTS];
