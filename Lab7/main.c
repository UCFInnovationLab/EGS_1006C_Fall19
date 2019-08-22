/*
 * Main.c
 *
 * Main file containing the main state machine.
 */
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "Library/Reflectance.h"
#include "Library/Clock.h"
#include "Library/Bump.h"
#include "Library/Motor.h"
#include "Library/Encoder.h"
#include "Library/Button.h"

#define TARGET_TICKS 180

void Initialize_System();

uint8_t light_data; // QTR-8RC
uint8_t light_data0;
int32_t Position; // 332 is right, and -332 is left of center
uint8_t bump_data;
uint8_t bump_data0;
uint8_t bump_data2;

int tick=0;

int mytime[20];
int i=0;

typedef enum
{
    START = 0,
    WAIT,
    GO,
    GO2,
    BUMMPED1a,
    BUMMPED1b,
    DRIVE,
    STOP
} my_state_t;

my_state_t state = START;

int main(void)
{
    bool left_done, right_done;

    int left_encoder_pos, right_encoder_pos;

    Initialize_System();

    set_left_motor_pwm(0);
    set_right_motor_pwm(0);

    while (1)
    {
        // Read Reflectance data into a byte.
        // Each bit corresponds to a sensor on the light bar
        light_data = Reflectance_Read(1000);
        light_data0 = LIGHT_BAR(light_data,0);

        // Convert light_data into a Position using a weighted sum
        Position = Reflectance_Position(light_data);

        // Read Bump data into a byte
        // Lower six bits correspond to the six bump sensors
        // put into individual variables so we can view it in GC
        bump_data = Bump_Read();
        bump_data0 = BUMP_SWITCH(bump_data,0);
        bump_data2 = BUMP_SWITCH(bump_data,2);

        // Emergency stop switch S2
        // Switch to state "STOP" if pressed
        if (button_S2_pressed()) 
        {
            state = STOP;
        }

        //-----------------------------------
        //        Main State Machine
        //-----------------------------------
        switch (state) 
        {

        case START:
                state = WAIT;
        break;

        case WAIT:
            if (button_S1_pressed()) 
            {
                state = GO;
            }
        break;

        case GO:
            left_encoder_pos = get_left_motor_count();
            right_encoder_pos = get_right_motor_count();

            set_left_motor_direction(true);
            set_right_motor_direction(false);

            left_done = false;
            right_done = false;

            state = DRIVE;
        break;

        // case GO2:
        //     if (bump_data0) state = BUMMPED1a;

        //     done = rotate_motors_by_counts(CONTINUOUS, .25, 360, 0);  // Continue to rotate until done

        //     if (done) state = STOP;
        // break;

        // case BUMMPED1a:
        //     set_left_motor_pwm(0);          // Stop all motors
        //     set_right_motor_pwm(0);

        //     rotate_motors_by_counts(INITIAL, .25, -100, 0);
        //     state = BUMMPED1b;
        // break;

        // case BUMMPED1b:
        //    done = rotate_motors_by_counts(CONTINUOUS, .25, -100, 0);

        //    if (done) state = DRIVE;
        // break;

        case DRIVE:
            if (!left_done)
            {
                set_left_motor_pwm(.1);
                left_done = (get_left_motor_count() - left_encoder_pos) > TARGET_TICKS;
            }
            else
            {
                set_left_motor_pwm(0);
            }
            
            if(!right_done)
            {
                set_right_motor_pwm(.1);
                right_done = (get_right_motor_count() - right_encoder_pos) < -TARGET_TICKS;
            }
            else
            {
                set_right_motor_pwm(0);
            }
            
            if (left_done && right_done) 
                state = STOP;
        break;

        case STOP:
            set_left_motor_pwm(0);          // Stop all motors
            set_right_motor_pwm(0);

            state = WAIT;
        break;
        }

        Clock_Delay1ms(10);
    }
}

void Initialize_System()
{
    /*
     * Initialize main clock
     *
     * SMCLK = 12Mhz
     */
    Clock_Init48MHz();

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring GPIO LED1 as an output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configure GPIO LED Red, LED Green, LED Blue */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);

    Reflectance_Init();

    Bump_Init();

    motor_init();

    encoder_init();

    button_init();

    MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);

    /*
     * Configuring SysTick to trigger at .001 sec (MCLK is 48Mhz)
     */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(48000);
    MAP_SysTick_enableInterrupt();

    MAP_Interrupt_enableMaster();
}


/*
 * Handle the SysTick Interrupt.  Currently interrupting at 1/10 second.
 *
 * Increment the tick counter "tick"
 * Blink the red led
 */
void SysTick_Handler(void)
{
    tick++;
    // if ((tick%1000)==0) MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);        // Toggle RED LED each time through loop
}



