// Module for the setup for the BlueTongue arm onto NUMBAT
// Author: Anita Smirnov
// Date: 21/07/18
#include <rtos-kochab.h>
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "pwm.h"
#include "std_msgs/Float64.hpp"
#include "servo.h"

#define ARM_ROTATE_PIN PWM0
#define ARM_TOP_PIN PWM1
#define ARM_BOTTOM_PIN PWM2
#define CLAW_ROTATE_PIN PWM3
#define CLAW_GRIP_PIN PWM4

#define PWM_PERIOD 10.0
#define MAX 2.0 // ms 20%
#define MIN 1.0 // ms 10%
#define DUTY_RANGE (100*(MAX - MIN)/PWM_PERIOD) // 10%
#define DUTY_NEUTRAL ((1.5 * 100) / PWM_PERIOD) // 15%
//#define MAX_SPEED 1 // m/s
#define MAP_SPEED_TO_DUTY_PCT ((MAX*100)/PWM_PERIOD - DUTY_NEUTRAL) // (20-15)% = 5%

#define SERVO_ANGLE_CONVERSION_FACTOR 0.5 // 180 deg. / 360 deg.

#define SYSTICKS_PER_SECOND 100

#define CLAW_ROTATION_MID 45
#define CLAW_ROTATION_MAX 90
#define CLAW_ROTATION_MIN 0

void armRotateCallback(const std_msgs::Float64 & msgs);
void armTopCallback(const std_msgs::Float64 & msgs);
void armBottomCallback(const std_msgs::Float64 & msg);
void clawRotateCallback(const std_msgs::Float64 & msg);
void clawGripCallback(const std_msgs::Float64 & msg);
static duty_pct speed_to_duty_pct(double speed);
static float claw_grip_convert (int clawGripPos);
static double wheel_to_servo_angle(double wheel_angle);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

extern "C" void task_blue_arm_fn(void) {
    ros_echronos::ROS_INFO("Entered ARM task. Initializing...\n");

    ros_echronos::NodeHandle nh;
    nh.init("blue_arm_fn", "blue_arm_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    ros_echronos::ROS_INFO("Done init\n");

    ros_echronos::ROS_INFO("Initalising BlueTongue arm subscribers\n");
    std_msgs::Float64 arm_rotate_buffer_in[5];
    std_msgs::Float64 arm_top_buffer_in[5];
    std_msgs::Float64 arm_bottom_buffer_in[5];
    std_msgs::Float64 claw_rotate_buffer_in[5];
    std_msgs::Float64 claw_grip_buffer_in[5];

    ros_echronos::Subscriber<std_msgs::Float64> armRotateSub("/e/abr", arm_rotate_buffer_in, 5, armRotateCallback);
    ros_echronos::Subscriber<std_msgs::Float64> armTopSub("/e/atl", arm_top_buffer_in, 5, armTopCallback);
    ros_echronos::Subscriber<std_msgs::Float64> armBottomSub("/e/abl", arm_bottom_buffer_in, 5, armBottomCallback);
    ros_echronos::Subscriber<std_msgs::Float64> clawRotateSub("/e/acr", claw_rotate_buffer_in, 5, clawRotateCallback);
    ros_echronos::Subscriber<std_msgs::Float64> clawGripSub("/e/acg", claw_grip_buffer_in, 5, clawGripCallback);

    armRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    armTopSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    armBottomSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    clawRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    clawGripSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    servo_init(GENERIC, CLAW_ROTATE_PIN);
    servo_init(GENERIC, CLAW_GRIP_PIN);
 
    pwm_init(ARM_ROTATE_PIN);
    pwm_init(ARM_TOP_PIN);
    pwm_init(ARM_BOTTOM_PIN);

    //pwm_set_prescaler(DIV8);

    pwm_set_period(PWM_PAIR0, PWM_PERIOD); //not 100% about this one
    pwm_set_period(PWM_PAIR1, PWM_PERIOD); //not 100% about this one
    //pwm_set_period(PWM_PAIR2, PWM_PERIOD); //not 100% about this one
    pwm_set_duty(ARM_TOP_PIN,15.0); //or these... is 15 a standard #?
    pwm_set_duty(ARM_BOTTOM_PIN,15.0);
    //pwm_set_duty(CLAW_GRIP_PIN,15.0); 
    pwm_set_duty(ARM_ROTATE_PIN, 15.0);

    pwm_enable(ARM_TOP_PIN);
    pwm_enable(ARM_BOTTOM_PIN);
    //pwm_enable(CLAW_GRIP_PIN);
    pwm_enable(ARM_ROTATE_PIN);

    ros_echronos::ROS_INFO("Starting the main loop.\n");
    while(true) {
        nh.spin();
    }
}

int main(void) {

    // Initialize the floating-point unit.
    InitializeFPU();

    // Set the clocking to run from the PLL at 50 MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Set up the systick interrupt used by the RTOS
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    init_can_common();

    alloc::init_mm(RTOS_MUTEX_ID_ALLOC);

    ros_echronos::write_mutex = RTOS_MUTEX_ID_PRINT;
    ros_echronos::write_mutex_set = true;

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}

// finds the duty cycle of the motors in terms of a percentage
static duty_pct speed_to_duty_pct(double speed) {
    return DUTY_NEUTRAL + (speed * MAP_SPEED_TO_DUTY_PCT);
}

static double wheel_to_servo_angle(double wheel_angle) {
    return wheel_angle * SERVO_ANGLE_CONVERSION_FACTOR;
}

// valid claw grip values range 1200 - 1700, 1500 is default/neutral
static float claw_grip_convert (int clawGripPos) {
    float clawGrip = (float)clawGripPos/(float)CLAW_ROTATION_MAX*1000.0 + 1000;
}

void armRotateCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(ARM_ROTATE_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Arm rotate received.\n");
}

void armTopCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(ARM_TOP_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Arm top received.\n");
}

void armBottomCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(ARM_BOTTOM_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Arm bottom received.\n");
}

void clawRotateCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(GENERIC, CLAW_ROTATE_PIN, msg.data);
    UARTprintf("Claw rotate received.\n");
}

void clawGripCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(GENERIC, CLAW_GRIP_PIN, msg.data);
    UARTprintf("Claw grip received.\n");
}
