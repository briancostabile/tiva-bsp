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
 * @file bsp_Dma.h
 * @brief Contains types and prototypes to access the DMA engine
 */
#pragma once

#include "bsp_Platform.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
/* Channel 0 */
#define BSP_DMA_CH0_USB0_EP1_RX 0
#define BSP_DMA_CH0_UART2_RX    1
#define BSP_DMA_CH0_SW2         2
#define BSP_DMA_CH0_GPTIMER4A   3
#define BSP_DMA_CH0_SW4         4

/* Channel 1 */
#define BSP_DMA_CH1_USB0_EP1_TX 0
#define BSP_DMA_CH1_UART2_TX    1
#define BSP_DMA_CH1_SW2         2
#define BSP_DMA_CH1_GPTIMER4B   3
#define BSP_DMA_CH1_SW4         4

/* Channel 2 */
#define BSP_DMA_CH2_USB0_EP2_RX 0
#define BSP_DMA_CH2_GPTIMER3A   1
#define BSP_DMA_CH2_SW2         2
#define BSP_DMA_CH2_SW3         3
#define BSP_DMA_CH2_SW4         4

/* Channel 3 */
#define BSP_DMA_CH3_USB0_EP2_TX 0
#define BSP_DMA_CH3_GPTIMER3B   1
#define BSP_DMA_CH3_SW2         2
#define BSP_DMA_CH3_SW3         3
#define BSP_DMA_CH3_SW4         4

/* Channel 4 */
#define BSP_DMA_CH4_USB0_EP3_RX 0
#define BSP_DMA_CH4_GPTIMER2A   1
#define BSP_DMA_CH4_SW2         2
#define BSP_DMA_CH4_GPIOA       3
#define BSP_DMA_CH4_SW4         4

/* Channel 5 */
#define BSP_DMA_CH5_USB0_EP3_TX 0
#define BSP_DMA_CH5_GPTIMER2B   1
#define BSP_DMA_CH5_SW2         2
#define BSP_DMA_CH5_GPIOB       3
#define BSP_DMA_CH5_SW4         4

/* Channel 6 */
#define BSP_DMA_CH6_SW0         0
#define BSP_DMA_CH6_GPTIMER2A_6 1
#define BSP_DMA_CH6_UART5_RX    2
#define BSP_DMA_CH6_GPIOC       3
#define BSP_DMA_CH6_SW4         4

/* Channel 7 */
#define BSP_DMA_CH7_SW0         0
#define BSP_DMA_CH7_GPTIMER2B_7 1
#define BSP_DMA_CH7_UART5_TX    2
#define BSP_DMA_CH7_GPIOD       3
#define BSP_DMA_CH7_SW4         4

/* Channel 8 */
#define BSP_DMA_CH8_UART0_RX  0
#define BSP_DMA_CH8_UART1_RX  1
#define BSP_DMA_CH8_SW3       2
#define BSP_DMA_CH8_GPTIMER5A 3
#define BSP_DMA_CH8_SW4       4

/* Channel 9 */
#define BSP_DMA_CH9_UART0_TX  0
#define BSP_DMA_CH9_UART1_TX  1
#define BSP_DMA_CH9_SW3       2
#define BSP_DMA_CH9_GPTIMER5B 3
#define BSP_DMA_CH9_SW4       4

/* Channel 10 */
#define BSP_DMA_CH10_SSI0_RX    0
#define BSP_DMA_CH10_SSI1_RX    1
#define BSP_DMA_CH10_UART6_RX   2
#define BSP_DMA_CH10_GPWTIMER0A 3
#define BSP_DMA_CH10_SW4        4

/* Channel 11 */
#define BSP_DMA_CH11_SSI0_TX    0
#define BSP_DMA_CH11_SSI1_TX    1
#define BSP_DMA_CH11_UART6_TX   2
#define BSP_DMA_CH11_GPWTIMER0B 3
#define BSP_DMA_CH11_SW4        4

/* Channel 12 */
#define BSP_DMA_CH12_SW0         0
#define BSP_DMA_CH12_UART2_RX_12 1
#define BSP_DMA_CH12_SSI2_RX     2
#define BSP_DMA_CH12_GPWTIMER1A  3
#define BSP_DMA_CH12_SW4         4

/* Channel 13 */
#define BSP_DMA_CH13_SW0         0
#define BSP_DMA_CH13_UART2_TX_13 1
#define BSP_DMA_CH13_SSI2_TX     2
#define BSP_DMA_CH13_GPWTIMER1B  3
#define BSP_DMA_CH13_SW4         4

/* Channel 14 */
#define BSP_DMA_CH14_ADC0_SS0  0
#define BSP_DMA_CH14_GPTIMER2A 1
#define BSP_DMA_CH14_SSI3_RX   2
#define BSP_DMA_CH14_GPIOE     3
#define BSP_DMA_CH14_SW4       4

/* Channel 15 */
#define BSP_DMA_CH15_ADC0_SS1  0
#define BSP_DMA_CH15_GPTIMER2B 1
#define BSP_DMA_CH15_SSI3_TX   2
#define BSP_DMA_CH15_GPIOF     3
#define BSP_DMA_CH15_SW4       4

/* Channel 16 */
#define BSP_DMA_CH16_ADC0_SS2   0
#define BSP_DMA_CH16_SW1        1
#define BSP_DMA_CH16_UART3_RX   2
#define BSP_DMA_CH16_GPWTIMER2A 3
#define BSP_DMA_CH16_SW4        4

/* Channel 17 */
#define BSP_DMA_CH17_ADC0_SS3   0
#define BSP_DMA_CH17_SW1        1
#define BSP_DMA_CH17_UART3_TX   2
#define BSP_DMA_CH17_GPWTIMER2B 3
#define BSP_DMA_CH17_SW4        4

/* Channel 18 */
#define BSP_DMA_CH18_GPTIMER0A 0
#define BSP_DMA_CH18_GPTIMER1A 1
#define BSP_DMA_CH18_UART4_RX  2
#define BSP_DMA_CH18_GPIOB     3
#define BSP_DMA_CH18_SW4       4

/* Channel 19 */
#define BSP_DMA_CH19_GPTIMER0B 0
#define BSP_DMA_CH19_GPTIMER1B 1
#define BSP_DMA_CH19_UART4_TX  2
#define BSP_DMA_CH19_SW3       3
#define BSP_DMA_CH19_SW4       4

/* Channel 20 */
#define BSP_DMA_CH20_GPTIMER1A 0
#define BSP_DMA_CH20_SW1       1
#define BSP_DMA_CH20_UART7_RX  2
#define BSP_DMA_CH20_SW3       3
#define BSP_DMA_CH20_SW4       4

/* Channel 21 */
#define BSP_DMA_CH21_GPTIMER1B 0
#define BSP_DMA_CH21_SW1       1
#define BSP_DMA_CH21_UART7_TX  2
#define BSP_DMA_CH21_SW3       3
#define BSP_DMA_CH21_SW4       4

/* Channel 22 */
#define BSP_DMA_CH22_UART1_RX 0
#define BSP_DMA_CH22_SW1      1
#define BSP_DMA_CH22_SW2      2
#define BSP_DMA_CH22_SW3      3
#define BSP_DMA_CH22_SW4      4

/* Channel 23 */
#define BSP_DMA_CH23_UART1_TX 0
#define BSP_DMA_CH23_SW1      1
#define BSP_DMA_CH23_SW2      2
#define BSP_DMA_CH23_SW3      3
#define BSP_DMA_CH23_SW4      4

/* Channel 24 */
#define BSP_DMA_CH24_SSI1_RX    0
#define BSP_DMA_CH24_ADC1_SS0   1
#define BSP_DMA_CH24_SW2        2
#define BSP_DMA_CH24_GPWTIMER3A 3
#define BSP_DMA_CH24_SW4        4

/* Channel 25 */
#define BSP_DMA_CH25_SSI1_TX    0
#define BSP_DMA_CH25_ADC1_SS1   1
#define BSP_DMA_CH25_SW2        2
#define BSP_DMA_CH25_GPWTIMER3B 3
#define BSP_DMA_CH25_SW4        4

/* Channel 26 */
#define BSP_DMA_CH26_SW0        0
#define BSP_DMA_CH26_ADC1_SS2   1
#define BSP_DMA_CH26_SW2        2
#define BSP_DMA_CH26_GPWTIMER4A 3
#define BSP_DMA_CH26_SW4        4

/* Channel 27 */
#define BSP_DMA_CH27_SW0        0
#define BSP_DMA_CH27_ADC1_SS3   1
#define BSP_DMA_CH27_SW2        2
#define BSP_DMA_CH27_GPWTIMER4B 3
#define BSP_DMA_CH27_SW4        4

/* Channel 28 */
#define BSP_DMA_CH28_SW0        0
#define BSP_DMA_CH28_SW1        1
#define BSP_DMA_CH28_SW2        2
#define BSP_DMA_CH28_GPWTIMER5A 3
#define BSP_DMA_CH28_SW4        4

/* Channel 29 */
#define BSP_DMA_CH29_SW0        0
#define BSP_DMA_CH29_SW1        1
#define BSP_DMA_CH29_SW2        2
#define BSP_DMA_CH29_GPWTIMER5B 3
#define BSP_DMA_CH29_SW4        4

/* Channel 30 */
#define BSP_DMA_CH30_SW0 0
#define BSP_DMA_CH30_SW1 1
#define BSP_DMA_CH30_SW2 2
#define BSP_DMA_CH30_SW3 3
#define BSP_DMA_CH30_SW4 4

/*============================================================================*/
#define BSP_DMA_INCREMENT_8    ((bsp_Dma_Increment_t)0)
#define BSP_DMA_INCREMENT_16   ((bsp_Dma_Increment_t)1)
#define BSP_DMA_INCREMENT_32   ((bsp_Dma_Increment_t)2)
#define BSP_DMA_INCREMENT_NONE ((bsp_Dma_Increment_t)3)
typedef uint8_t bsp_Dma_Increment_t;

/*============================================================================*/
typedef uint16_t bsp_Dma_XferSize_t;

/*============================================================================*/
#define BSP_DMA_ARB_SIZE_1    ((bsp_Dma_ArbSize_t)0)
#define BSP_DMA_ARB_SIZE_2    ((bsp_Dma_ArbSize_t)1)
#define BSP_DMA_ARB_SIZE_4    ((bsp_Dma_ArbSize_t)2)
#define BSP_DMA_ARB_SIZE_8    ((bsp_Dma_ArbSize_t)3)
#define BSP_DMA_ARB_SIZE_16   ((bsp_Dma_ArbSize_t)4)
#define BSP_DMA_ARB_SIZE_32   ((bsp_Dma_ArbSize_t)5)
#define BSP_DMA_ARB_SIZE_64   ((bsp_Dma_ArbSize_t)6)
#define BSP_DMA_ARB_SIZE_128  ((bsp_Dma_ArbSize_t)7)
#define BSP_DMA_ARB_SIZE_256  ((bsp_Dma_ArbSize_t)8)
#define BSP_DMA_ARB_SIZE_512  ((bsp_Dma_ArbSize_t)9)
#define BSP_DMA_ARB_SIZE_1024 ((bsp_Dma_ArbSize_t)10)
typedef uint16_t bsp_Dma_ArbSize_t;

/*============================================================================*/
#define BSP_DMA_XFER_MODE_STOP                      ((bsp_Dma_XferMode_t)0)
#define BSP_DMA_XFER_MODE_BASIC                     ((bsp_Dma_XferMode_t)1)
#define BSP_DMA_XFER_MODE_AUTO_REQUEST              ((bsp_Dma_XferMode_t)2)
#define BSP_DMA_XFER_MODE_PING_PONG                 ((bsp_Dma_XferMode_t)3)
#define BSP_DMA_XFER_MODE_MEM_SCATTER_GATHER        ((bsp_Dma_XferMode_t)4)
#define BSP_DMA_XFER_MODE_ALT_MEM_SCATTER_GATHER    ((bsp_Dma_XferMode_t)5)
#define BSP_DMA_XFER_MODE_PERIPH_SCATTER_GATHER     ((bsp_Dma_XferMode_t)6)
#define BSP_DMA_XFER_MODE_ALT_PERIPH_SCATTER_GATHER ((bsp_Dma_XferMode_t)7)
typedef uint8_t bsp_Dma_XferMode_t;

/*============================================================================*/
#define BSP_DMA_PROT_NOT_PRIVILEGED ((bsp_Dma_Protect_t)0)
#define BSP_DMA_PROT_PRIVILEGED     ((bsp_Dma_Protect_t)1)
typedef uint8_t bsp_Dma_Protect_t;

/*============================================================================*/
typedef uint8_t bsp_Dma_ChId_t;

/*============================================================================*/
typedef void (*bsp_Dma_XferDoneCallback_t)(void *userData);

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Dma_init(void);

/*============================================================================*/
void bsp_Dma_chConfig(
    bsp_Dma_ChId_t      chId,
    bsp_Dma_XferMode_t  mode,
    bsp_Dma_Increment_t srcInc,
    bsp_Dma_Increment_t srcSize,
    void *              srcPtr,
    bsp_Dma_Increment_t dstInc,
    bsp_Dma_Increment_t dstSize,
    void *              dstPtr,
    size_t              size);

/*============================================================================*/
void bsp_Dma_chStart(bsp_Dma_ChId_t chId, bsp_Dma_XferDoneCallback_t callback);
