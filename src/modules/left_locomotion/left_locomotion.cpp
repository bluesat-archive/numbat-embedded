// Module for left locomotion of the NUMBAT rover
// Author: Elliott Smith
// Date: 2018-04-02
// Editor: William Miles
// Updated: 2019-09-10
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

#define FRONT_LEFT_DRIVE_PIN PWM0
#define BACK_LEFT_DRIVE_PIN PWM1
#define FRONT_LEFT_ROTATE_PIN PWM2
#define BACK_LEFT_ROTATE_PIN PWM3

#define SERVO_ANGLE_CONVERSION_FACTOR 7.85 // 2826 deg. / 360 deg.
#define PWM_PERIOD (10.0)
#define NEUTRAL_DUTY (15.0)
#define MAX_DUTY_AMPLITUDE (-5.0)
#define SPEED_MAX (1.0)

#define SYSTICKS_PER_SECOND     100

static duty_pct speed_to_duty_pct(double speed);
static double wheel_to_servo_angle(double wheel_angle);
void frontLeftDriveCallback(const std_msgs::Float64 & msg);
void frontLeftRotateCallback(const std_msgs::Float64 & msg);
void backLeftDriveCallback(const std_msgs::Float64 & msg);
void backLeftRotateCallback(const std_msgs::Float64 & msg);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

extern "C" void task_left_locomotion_fn(void) {
    ros_echronos::ROS_INFO("Entered left locomotion task. Initializing...\n");

    ros_echronos::NodeHandle nh;
    nh.init("left_locomotion_fn", "left_locomotion_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    ros_echronos::ROS_INFO("Done init\n");

    ros_echronos::ROS_INFO("Initalising left locomotion subscribers\n");

    std_msgs::Float64 front_left_drive_buffer_in[5];
    std_msgs::Float64 front_left_rotate_buffer_in[5];
    std_msgs::Float64 back_left_drive_buffer_in[5];
    std_msgs::Float64 back_left_rotate_buffer_in[5];

    ros_echronos::Subscriber<std_msgs::Float64> frontLeftDriveSub("/e/fld", front_left_drive_buffer_in, 5, frontLeftDriveCallback);
    ros_echronos::Subscriber<std_msgs::Float64> frontLeftRotateSub("/e/fls", front_left_rotate_buffer_in, 5, frontLeftRotateCallback);
    ros_echronos::Subscriber<std_msgs::Float64> backLeftDriveSub("/e/bld", back_left_drive_buffer_in, 5, backLeftDriveCallback);
    ros_echronos::Subscriber<std_msgs::Float64> backLeftRotateSub("/e/bls", back_left_rotate_buffer_in, 5, backLeftRotateCallback);

    frontLeftDriveSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    frontLeftRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    backLeftDriveSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    backLeftRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    servo_init(HS_785HB, FRONT_LEFT_ROTATE_PIN);
    servo_init(HS_785HB, BACK_LEFT_ROTATE_PIN);

    pwm_init(FRONT_LEFT_DRIVE_PIN);
    pwm_init(BACK_LEFT_DRIVE_PIN);

    pwm_set_period(PWM_PAIR0, PWM_PERIOD);

    pwm_set_duty(FRONT_LEFT_DRIVE_PIN, NEUTRAL_DUTY);
    pwm_set_duty(BACK_LEFT_DRIVE_PIN, NEUTRAL_DUTY);

    pwm_enable(FRONT_LEFT_DRIVE_PIN);
    pwm_enable(BACK_LEFT_DRIVE_PIN);
    
    ros_echronos::ROS_INFO("Starting the main loop.\n");

    for (;;) {
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

static duty_pct speed_to_duty_pct(double speed) {
    return NEUTRAL_DUTY + ((speed / SPEED_MAX) * MAX_DUTY_AMPLITUDE);
}

static double wheel_to_servo_angle(double wheel_angle) {
    return wheel_angle * SERVO_ANGLE_CONVERSION_FACTOR;
}

void frontLeftDriveCallback(const std_msgs::Float64 & msg) {
    UARTprintf("Front left drive received.\n");
    pwm_set_duty(FRONT_LEFT_DRIVE_PIN, speed_to_duty_pct(msg.data));
}
  
void frontLeftRotateCallback(const std_msgs::Float64 & msg) {
    UARTprintf("Front left swerve received.\n");
    servo_write_rads(HS_785HB, FRONT_LEFT_ROTATE_PIN, wheel_to_servo_angle(msg.data));
}
    
    
void backLeftDriveCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(BACK_LEFT_DRIVE_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Back left drive received.\n");
}
    
void backLeftRotateCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(HS_785HB, BACK_LEFT_ROTATE_PIN, wheel_to_servo_angle(msg.data));
    UARTprintf("Back left swerve received.\n");
}
