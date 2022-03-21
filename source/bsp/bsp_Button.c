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
 * @file bsp_Button.c
 * @brief Simple low-level interface to switch-style push buttons.
 */

#include "bsp_Types.h"
#include "bsp_Assert.h"
#include "bsp_Mcu.h"
#include "bsp_Button.h"
#include "bsp_Gpio.h"

#if (BSP_PLATFORM_IO_BUTTON_NUM > 0)

/*==============================================================================
 *                                Types
 *============================================================================*/
/*============================================================================*/
typedef struct bsp_Button_Info_s {
    bsp_Button_Control_t      control;
    bsp_Button_PressHandler_t handler;
} bsp_Button_Info_t;

/*==============================================================================
 *                              Global Data
 *============================================================================*/
bsp_Button_Info_t bsp_Button_infoTable[BSP_PLATFORM_IO_BUTTON_NUM];

/*==============================================================================
 *                             Local Functions
 *============================================================================*/
/*============================================================================*/
static void bsp_Button_ioHandler(bsp_Gpio_PortId_t portId, bsp_Gpio_PinOffset_t pinOffset)
{
    bsp_Gpio_BitMask_t mask = (1 << pinOffset);

    for (uint8_t i = 0; i < DIM(bsp_Button_ioInfoTable); i++) {
        if ((bsp_Button_ioInfoTable[i].portId == portId) &&
            (bsp_Button_ioInfoTable[i].mask == mask) &&
            (bsp_Button_infoTable[i].control == BSP_BUTTON_CONTROL_ENABLE) &&
            (bsp_Button_infoTable[i].handler != NULL)) {
            /* Buttons auto-disable on the interrupt event to prevent bouncing
             * from triggering a bunch of disruptive interrupts. The client must
             * poll the state of the button after a short time to determine if
             * the press is "real" and then continue polling on a longer period
             * to determine a long press and release
             */
            bsp_Button_infoTable[i].control = BSP_BUTTON_CONTROL_DISABLE;
            bsp_Gpio_intControl(
                bsp_Button_ioInfoTable[i].portId,
                bsp_Button_ioInfoTable[i].mask,
                BSP_GPIO_INT_CONTROL_DISABLE);

            bsp_Button_infoTable[i].handler(i);
            break;
        }
    }
    return;
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void bsp_Button_init(void)
{
    uint8_t i;

    for (i = 0; i < DIM(bsp_Button_ioInfoTable); i++) {
        /* Disable the GPIO interrupt while configuring */
        bsp_Gpio_intControl(
            bsp_Button_ioInfoTable[i].portId,
            bsp_Button_ioInfoTable[i].mask,
            BSP_GPIO_INT_CONTROL_DISABLE);

        bsp_Button_infoTable[i].control = BSP_BUTTON_CONTROL_DISABLE;
        bsp_Button_infoTable[i].handler = NULL;

        /* Configure as input */
        bsp_Gpio_configInput(
            bsp_Button_ioInfoTable[i].portId,
            bsp_Button_ioInfoTable[i].mask,
            FALSE,
            (bsp_Button_ioInfoTable[i].pressPolarity == BSP_BUTTON_PRESS_POLARITY_LOW)
                ? BSP_GPIO_PULL_UP
                : BSP_GPIO_PULL_DOWN);
    }

    return;
}

/*============================================================================*/
void bsp_Button_control(bsp_Button_Id_t id, bsp_Button_Control_t control)
{
    BSP_ASSERT(id < BSP_PLATFORM_IO_BUTTON_NUM);

    BSP_MCU_CRITICAL_SECTION_ENTER();
    if ((control == BSP_BUTTON_CONTROL_DISABLE) &&
        (bsp_Button_infoTable[id].control != BSP_BUTTON_CONTROL_DISABLE)) {
        bsp_Button_infoTable[id].control = control;
        bsp_Gpio_intControl(
            bsp_Button_ioInfoTable[id].portId,
            bsp_Button_ioInfoTable[id].mask,
            BSP_GPIO_INT_CONTROL_DISABLE);
    }
    else if (
        (control == BSP_BUTTON_CONTROL_ENABLE) &&
        (bsp_Button_infoTable[id].control != BSP_BUTTON_CONTROL_ENABLE) &&
        (bsp_Button_infoTable[id].handler != NULL)) {
        bsp_Button_infoTable[id].control = control;
        bsp_Gpio_intControl(
            bsp_Button_ioInfoTable[id].portId,
            bsp_Button_ioInfoTable[id].mask,
            BSP_GPIO_INT_CONTROL_ENABLE);
    }
    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
void bsp_Button_registerHandler(bsp_Button_Id_t id, bsp_Button_PressHandler_t handler)
{
    BSP_ASSERT(id < BSP_PLATFORM_IO_BUTTON_NUM);

    BSP_MCU_CRITICAL_SECTION_ENTER();

    bsp_Button_infoTable[id].handler = handler;

    bsp_Gpio_intConfig(
        bsp_Button_ioInfoTable[id].portId,
        bsp_Button_ioInfoTable[id].mask,
        FALSE,
        FALSE,
        (bsp_Button_ioInfoTable[id].pressPolarity == BSP_BUTTON_PRESS_POLARITY_LOW)
            ? BSP_GPIO_INT_TYPE_EDGE_FALLING
            : BSP_GPIO_INT_TYPE_EDGE_RISING,
        bsp_Button_ioHandler);

    BSP_MCU_CRITICAL_SECTION_EXIT();

    return;
}

/*============================================================================*/
bsp_Button_State_t bsp_Button_state(bsp_Button_Id_t id)
{
    bsp_Gpio_BitMask_t mask;
    BSP_ASSERT(id < BSP_PLATFORM_IO_BUTTON_NUM);

    mask = bsp_Gpio_read(bsp_Button_ioInfoTable[id].portId, bsp_Button_ioInfoTable[id].mask);

    return (
        ((bsp_Button_ioInfoTable[id].pressPolarity == BSP_BUTTON_PRESS_POLARITY_LOW) && (mask == 0))
            ? BSP_BUTTON_STATE_PRESSED
            : BSP_BUTTON_STATE_NOT_PRESSED);
}
#else
/*============================================================================*/
void bsp_Button_init(void)
{
    return;
}
#endif
