/*
 * Button.c
 */

#include <stdint.h>
#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Button.h"

// Global button flags
bool button_S1_pressed_flag = false;
bool button_S2_pressed_flag = false;

/*
 *
 */
void button_init(void){
    /*
     * Configure P1.1 for button interrupt
     */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    /*
     * Configure P1.4 for button interrupt
     */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    /* Enable interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT1);
}

/*
 *
 */
bool button_S1_pressed(void)
{
    if (button_S1_pressed_flag)
    {
        button_S1_pressed_flag = false;
        return true;
    }

    return false;
}

bool button_S2_pressed(void)
{
    if (button_S2_pressed_flag)
    {
        button_S2_pressed_flag = false;
        return true;
    }

    return false;
}
/* Port1 ISR - This Interrupt Handler will service the Port1 interrupts.
 * Port1 pin 1 & 4 are connected to the two side buttons.
 */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Button S1
    if (status & GPIO_PIN1)
    {
        button_S1_pressed_flag = true;
    }

    // Button S2
    if (status & GPIO_PIN4)
    {
        button_S2_pressed_flag = true;
    }
}
