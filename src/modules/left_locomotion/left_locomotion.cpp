// Module for left locomotion of the NUMBAT rover
// Author: Elliott Smith
// Date: 2/4/18
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

#define DRIVE_PWM_PERIOD 10.0

#define SERVO_ANGLE_CONVERSION_FACTOR 7.85 // 2826 deg. / 360 deg.

#define SYSTICKS_PER_SECOND     100

static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
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

bool sent_message;

static uint32_t error_flag;

extern "C" void task_left_locomotion_fn(void) {
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");

    ros_echronos::NodeHandle nh;
    nh.init("left_locomotion_fn", "left_locomotion_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    ros_echronos::ROS_INFO("Done init\n");

    ros_echronos::ROS_INFO("Initalising left locomotion subscribers\n");

    std_msgs::Float64 front_left_drive_buffer_in[5];
    std_msgs::Float64 front_left_rotate_buffer_in[5];
    std_msgs::Float64 back_left_drive_buffer_in[5];
    std_msgs::Float64 back_left_rotate_buffer_in[5];

    ros_echronos::Subscriber<std_msgs::Float64> frontLeftDriveSub("/fld", front_left_drive_buffer_in, 5, frontLeftDriveCallback);
    ros_echronos::Subscriber<std_msgs::Float64> frontLeftRotateSub("/fls", front_left_rotate_buffer_in, 5, frontLeftRotateCallback);
    ros_echronos::Subscriber<std_msgs::Float64> backLeftDriveSub("/bld", back_left_drive_buffer_in, 5, backLeftDriveCallback);
    ros_echronos::Subscriber<std_msgs::Float64> backLeftRotateSub("/bls", back_left_rotate_buffer_in, 5, backLeftRotateCallback);

    frontLeftDriveSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    frontLeftRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    backLeftDriveSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    backLeftRotateSub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    servo_init(HS_785HB, FRONT_LEFT_ROTATE_PIN);
    servo_init(HS_785HB, BACK_LEFT_ROTATE_PIN);

    pwm_init(FRONT_LEFT_DRIVE_PIN);
    pwm_init(BACK_LEFT_DRIVE_PIN);

    pwm_set_period(PWM_PAIR0, DRIVE_PWM_PERIOD);

    pwm_set_duty(FRONT_LEFT_DRIVE_PIN,15.0);
    pwm_set_duty(BACK_LEFT_DRIVE_PIN,15.0);

    pwm_enable(FRONT_LEFT_DRIVE_PIN);
    pwm_enable(BACK_LEFT_DRIVE_PIN);
    
    ros_echronos::ROS_INFO("starting the main loop\n");
    int i = 0;
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

static duty_pct speed_to_duty_pct(double speed) {
    duty_pct duty = 15.0 + (speed / 3.0 * 5.0);

    return duty;
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
