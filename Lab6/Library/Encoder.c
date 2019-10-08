 /*
  *
  * Pololu #3542 Romi Encoder connected to Pololu #3543 Motor Driver and Power Distribution Board
  * This connects motor, power, encoders, and grounds.  Kit includes this hardware.  See images.
  * Sever VPU = VREG jumper on Motor Driver and Power Distribution Board and connect VPU to 3.3V.
  * This is necessary because MSP432 inputs are not 5V tolerant.
  *
  * Left Encoder A connected to P6.4 (J1)                     .
  * Left Encoder B connected to P6.5 (J1)
  * Right Encoder A connected to P5.4 (J3)
  * Right Encoder B connected to P5.5 (J3)
  *
  * Pololu encoder has 12 counts per revolution (counting all 4 edges).
  * The motor has a gearbox with a 120:1 ratio.
  * This gives 12*120 = 1440 counts per revolution of the wheel.
  * Since we are only counting 1 edge of the encoder we need to divide by 4 for a total of 360 counts per revolution.
 */



#include <stdint.h>
#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Encoder.h"

// Initialize Encoder inputs

int left_motor_count = 0;
int right_motor_count = 0;

void encoder_init(void){
    /* era:10.4 erb: 5.0 ela:10.5 elb: 5.2 */
    /* Configuring P6.4 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN2);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN2, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN2);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN2);
    MAP_Interrupt_enableInterrupt(INT_PORT5);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P10, GPIO_PIN5);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN0);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN0);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN0);
    MAP_Interrupt_enableInterrupt(INT_PORT5);

    MAP_GPIO_setAsInputPin(GPIO_PORT_P10, GPIO_PIN4);
}

int get_left_motor_count() {
    return left_motor_count;
}

int get_right_motor_count() {
    return right_motor_count;
}

/* GPIO ISR */
void PORT5_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    /* Toggling the output on the LED */
    if(status & GPIO_PIN2)
    {
        if (MAP_GPIO_getInputPinValue(GPIO_PORT_P10, GPIO_PIN5))
            left_motor_count--;
        else
            left_motor_count++;

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }

    /* Toggling the output on the LED */
    if(status & GPIO_PIN0)
    {
        if (MAP_GPIO_getInputPinValue(GPIO_PORT_P10, GPIO_PIN4))
            right_motor_count--;
        else
            right_motor_count++;

        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }

}

