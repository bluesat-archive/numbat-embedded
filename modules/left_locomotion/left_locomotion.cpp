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

ros_echronos::NodeHandle * volatile nh_ptr = NULL;

#define DRIVE_PWM_PERIOD 10.0
#define DRIVE_DUTY_MAX 20.0

#define SERVO_ANGLE_CONVERSION_FACTOR 7.85 // 2826 deg. / 360 deg.

#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

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
    nh.init("left_locomotion_fn", "left_locomotion_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    ros_echronos::ROS_INFO("Done init\n");
    nh_ptr = &nh;

    ros_echronos::ROS_INFO("Initalising left locomotion subscribers\n");
    // Create the subscribers
    std_msgs::Float64 front_left_drive_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> frontLeftDriveSub("front_left_wheel_axel_controller/command", front_left_drive_buffer_in, 5, frontLeftDriveCallback);
    frontLeftDriveSub.set_topic_id(0);
    std_msgs::Float64 front_left_rotate_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> frontLeftRotateSub("front_left_swerve_controller/command", front_left_rotate_buffer_in, 5, frontLeftRotateCallback);
    frontLeftRotateSub.set_topic_id(4);
    std_msgs::Float64 back_left_drive_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> backLeftDriveSub("back_left_wheel_axel_controller/command", back_left_drive_buffer_in, 5, backLeftDriveCallback);
    backLeftDriveSub.set_topic_id(2);
    std_msgs::Float64 back_left_rotate_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> backLeftRotateSub("back_left_swerve_controller/command", back_left_rotate_buffer_in, 5, backLeftRotateCallback);
    backLeftRotateSub.set_topic_id(6);
    frontLeftDriveSub.init(nh);
    frontLeftRotateSub.init(nh);
    backLeftDriveSub.init(nh);
    backLeftRotateSub.init(nh);

    servo_init(HS_785HB, FRONT_LEFT_ROTATE_PIN);
    servo_init(HS_785HB, BACK_LEFT_ROTATE_PIN);

    pwm_init(FRONT_LEFT_DRIVE_PIN);
    pwm_init(BACK_LEFT_DRIVE_PIN);
    pwm_set_period(PWM_PAIR0, DRIVE_PWM_PERIOD);
    pwm_enable(FRONT_LEFT_DRIVE_PIN);
    pwm_enable(BACK_LEFT_DRIVE_PIN);

    
    ros_echronos::ROS_INFO("starting the main loop\n");
    while(true) {
        nh.spin();
    }
    /*
    // create a test message
    owr_messages::pwm out_msg;
    strncpy(out_msg.joint, "aaaa", 4);
    out_msg.pwm = 0xDEADBEEF;

    // create a publisher
    Publisher<owr_messages::pwm> pub("null", pwm_buffer, 5, false);

    //create some messages
    ros_echronos::can::can_ros_message cmsgs[7];

    // "publish them"
    pub.publish(out_msg, 0);
    bool has_next;
    bool is_empty;
    for(int i = 0; i < 7; ++i) {
        cmsgs[i] = pub.get_next_message(has_next, is_empty);
    }

    // create a subscriber
    owr_messages::pwm pwm_buffer_in[5];
    Subscriber<owr_messages::pwm> sub("null", pwm_buffer_in, 5, callback);


    // read them back
    owr_messages::pwm in_msg;
    for(int i = 0; i < 7; ++i) {
        sub.receive_message(cmsgs[i]);
    }
    sub.call_callback();

    //ros_echronos::ROS_INFO("Joint %s\n", in_msg.joint);
    //ros_echronos::ROS_INFO("PWM %d\n", in_msg.pwm);

    ros_echronos::ROS_INFO("Done\n");
    while (true) {}*/

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

    init_can();

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

void init_can(void) {
    // We enable GPIO E - E4 for RX and E5 for TX
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // enables the can function we have just configured on those pins
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //enable and initalise CAN0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);

    //TODO: change this to use the eChronos clock
    // Set the bitrate for the CAN BUS. It uses the system clock
    CANBitRateSet(CAN0_BASE, ROM_SysCtlClockGet(), CAN_BITRATE);

    // enable can interupts
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR); //| CAN_INT_STATUS);
    IntEnable(INT_CAN0);

    //start CAN
    CANEnable(CAN0_BASE);

}

static duty_pct speed_to_duty_pct(double speed) {
    duty_pct duty = speed / DRIVE_PWM_PERIOD;

    if (duty > DRIVE_DUTY_MAX) {
        duty = DRIVE_DUTY_MAX;
    }

    return duty;
}

static double wheel_to_servo_angle(double wheel_angle) {
    return wheel_angle * SERVO_ANGLE_CONVERSION_FACTOR;
}

void frontLeftDriveCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(FRONT_LEFT_DRIVE_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Front left drive received.\n");
}
  
void frontLeftRotateCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(HS_785HB, FRONT_LEFT_ROTATE_PIN, wheel_to_servo_angle(msg.data));
    UARTprintf("Front left swerve received. %lf\n", msg.data);
}
    
    
void backLeftDriveCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(BACK_LEFT_DRIVE_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Back left drive received. %lf\n", msg.data);
}
    
void backLeftRotateCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(HS_785HB, FRONT_LEFT_ROTATE_PIN, wheel_to_servo_angle(msg.data));
    UARTprintf("Back left swerve received.\n");
}
