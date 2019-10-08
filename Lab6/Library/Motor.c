// Motor.c

// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board and connect VCCMD to 3.3V.
// This makes P3.7 and P3.6 low power disables for motor drivers.  0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
// This separates P3.7 and P3.6 allowing for independent control
//
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Motor.h"
#include "Encoder.h"


/* Timer_A PWM Configuration Parameter */
/*
 * Configure a timer to provide a PWM signal to each motor.
 * Use Timer_A along with two compare registers to generate two different PWM signals
 *
 * SMCLK = 12Mhz
 * Divide by 12 to get a 1Mhz timer clock source
 * Set divider to 1000 to get a 1000Hz pwm rate
 * Start off at 0 * 1/64 for the high time
 * Range will be 0-1000
 */
Timer_A_PWMConfig right_motor_pwm_config =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_12,
        1000,
        TIMER_A_CAPTURECOMPARE_REGISTER_3,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};
Timer_A_PWMConfig left_motor_pwm_config =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_12,
        1000,
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

void motor_init(void){
    /*
    * Configuring GPIO2.6 as peripheral output for PWM of Right Motor
    * Configure GPIO1.6 as peripheral output for DIR of Right Motor
    */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6,
           GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN5,
                  GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN5);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);

    /*
    * Configuring GPIO2.7 as peripheral output for PWM of Left Motor
    * Configure GPIO1.7 as peripheral output for DIR of Left Motor
    */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
           GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN4,
                  GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);

    /*
    * Configuring Timer_A to have a period of approximately 500ms and
    * an initial duty cycle of 10% of that (3200 ticks)
    */
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &right_motor_pwm_config);
  
    set_left_motor_pwm(0);
    set_right_motor_pwm(0);
}

/*
 *  Set left motor power (pwm - pulse width modulation)
 *
 *  Power is given in a range of 0.0 - 1.0
 */
void set_left_motor_pwm(float pwm_normal)
{
    int pwm;

    pwm = 1000 * pwm_normal;
    if (pwm>1000) pwm=1000;
    if (pwm<0) pwm=0;
    left_motor_pwm_config.dutyCycle = pwm;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &left_motor_pwm_config);
}

/*
 *  Set right motor power (pwm - pulse width modulation)
 *
 *  Power is given in a range of 0.0 - 1.0
 */
void set_right_motor_pwm(float pwm_normal)
{
    int pwm;

    pwm = 1000 * pwm_normal;

    if (pwm>1000) pwm=1000;
    if (pwm<0) pwm=0;
    right_motor_pwm_config.dutyCycle = pwm;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &right_motor_pwm_config);
}

/*
 *  Set left motor direction.
 *
 *  True: forward
 *  False: reverse
 */
void set_left_motor_direction(bool dir)
{
    if (dir)
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    else
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);
}

/*
 *  Set right motor direction.
 *
 *  True: forward
 *  False: reverse
 */
void set_right_motor_direction(bool dir)
{
    if (dir)
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);
    else
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
}

/*
 *  Rotate both left and right motors by a given encoder count.
 *
 *  This routine is called in two parts, first call is with mode=INITIAL to set up some initial static variables,
 *  second call is with mode=CONTINUOUS.  This call will turn on the motors in the proper direction and check if the target count threshold
 *  has been reached.  It should be called continuously until a value of TRUE is returned.
 */
bool rotate_motors_by_counts(motor_mode_t mode, float speed_factor, int left_count, int right_count)
{
    static int left_target;
    static int right_target;
    int left_error;
    int right_error;
    bool r = false;

    switch (mode) {
    case INITIAL:
        // save the target degrees in a static variable for use later
        left_target = get_left_motor_count() + left_count;
        right_target = get_right_motor_count() + right_count;

        // set motor direction based on if degrees is positive or negative
        set_left_motor_direction(left_count>=0);
        set_right_motor_direction(right_count>=0);

        r = false;
    break;

    case CONTINUOUS:

        left_error = left_target - get_left_motor_count();
        right_error = right_target - get_right_motor_count();

        // set motor direction based on if speed is positive or negative
        set_left_motor_direction(left_error>=0);
        set_right_motor_direction(right_error>=0);

        // Stop individual motor if we are within the threshold
        if (abs(left_error)>8)
            set_left_motor_pwm(speed_factor);
        else
            set_left_motor_pwm(0);

        if (abs(right_error)>8)
            set_right_motor_pwm(speed_factor);
        else
            set_right_motor_pwm(0);

        // if both motors are within the threshold then return true to signal "all done"
        if ((abs(left_error) <= 8) && (abs(right_error) <= 8))
            r = true;
    break;
    }

    return r;

}

/*
 *  Rotate both left and right motors by a given encoder count using a PID algorithm.
 *
 *  This routine is called in two parts, first call is with mode=INITIAL to set up some initial static variables,
 *  second call is with mode=CONTINUOUS.  This call will process the PID loop and return after one iteration.  It
 *  should be called until the return value is TRUE, signifying that the motors have reached the threshold around the target count.
 */
bool rotate_motors_by_counts_pid(motor_mode_t mode, float speed_factor, int left_count, int right_count)
{
    static int left_target;
    static int right_target;
    bool r = false;
    int left_error;
    int right_error;
    static int left_error_sum;
    static int right_error_sum;
    float P = 0.3;
    float I = 0.001;
    float left_motor_speed;
    float right_motor_speed;

    switch (mode) {
    case INITIAL:
        left_target = get_left_motor_count() + left_count;    // save the target degrees in a static variable for use later
        right_target = get_right_motor_count() + right_count;

        set_left_motor_direction(left_count>=0);               // set motor direction based on if degrees is positive or negative
        set_right_motor_direction(right_count>=0);

        left_error_sum = 0;
        right_error_sum = 0;
        r = false;
    break;

    case CONTINUOUS:

        left_error = left_target - get_left_motor_count();
        right_error = right_target - get_right_motor_count();

        // Calculate integral of error
        left_error_sum = left_error_sum + left_error;
        right_error_sum = right_error_sum + right_error;

        // Clamp integral, stop windup
        if (left_error_sum > 1.0/I) left_error_sum = 1.0/I;
        if (left_error_sum < -1.0/I) left_error_sum = -1.0/I;
        if (right_error_sum > 1.0/I) right_error_sum = 1.0/I;
        if (right_error_sum < -1.0/I) right_error_sum = -1.0/I;

        // Calculate motor speed using PI
        left_motor_speed = P * (float)left_error + I * (float)left_error_sum;
        right_motor_speed = P * (float)right_error + I * (float)right_error_sum;

        // Clamp motor speed between -1.0 - 1.0
        if (left_motor_speed < -1.0) left_motor_speed = -1.0;
        else if (left_motor_speed > 1.0) left_motor_speed = 1.0;
        if (right_motor_speed < -1.0) right_motor_speed = -1.0;
        else if (right_motor_speed > 1.0) right_motor_speed = 1.0;

        // Set motor direction based on sign of motor speed
        set_left_motor_direction(left_motor_speed>=0);               // set motor direction based on if speed is positive or negative
        set_right_motor_direction(right_motor_speed>=0);

        // Set motors to run at calculated speed times a speed factor
        set_left_motor_pwm(fabs(left_motor_speed * speed_factor));
        set_right_motor_pwm(fabs(right_motor_speed * speed_factor));

        // Stop if within a treshold
        if ((abs(left_error) < 2) && (abs(right_error) < 2))
            r = true;
    break;
    }

    return r;

}

