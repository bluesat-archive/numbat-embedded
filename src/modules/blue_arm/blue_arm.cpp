// Module for the setup for the BlueTongue arm onto NUMBAT
// Author: Anita Smirnov
// Date: 21/07/18

#include <rtos-kochab.h>
#include "boilerplate.h"
#include "ros.hpp"
#include "owr_messages/pwm.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "pwm.h"
#include "std_msgs/Float64.hpp"
#include "servo.h"
#include "rtos-kochab.h"
#include "driverlib/can.h"

#define ARM_ROTATE_PIN PWM0
#define ARM_TOP_PIN PWM1
#define ARM_BOTTOM_PIN PWM2
#define CLAW_ROTATE_PIN PWM3
#define CLAW_GRIP_PIN PWM4

#define PWM_PERIOD 10.0
#define MAX 2.0 // ms 20%
#define MIN 1.0 // ms 10%
#define DUTY_RANGE (100*(MAX + MIN)/PWM_PERIOD) // 30%
#define DUTY_NEUTRAL (DUTY_RANGE/2) // 15%
#define MAX_SPEED 3 // m/s
#define MAP_SPEED_TO_DUTY_PCT ((MAX*100)/PWM_PERIOD - DUTY_NEUTRAL) // (20-15)% = 5%

#define SERVO_ANGLE_CONVERSION_FACTOR 7.85 // 2826 deg. / 360 deg.

#define SYSTICKS_PER_SECOND 100 //ask about this

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

#define CLAW_ROTATION_MID 45
#define CLAW_ROTATION_MAX 90
#define CLAW_ROTATION_MIN 0


ros_echronos::NodeHandle * volatile nh_ptr = NULL;

static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
void callback(const owr_messages::pwm & msg);
//more functions
void armRotateCallback(const std_msgs::Float64 & msgs);
void armTopCallback(const std_msgs::Float64 & msgs);
void armBottomCallback(const std_msgs::Float64 & msg);
void clawRotateCallback(const std_msgs::Float64 & msg);
void clawGripCallback(const std_msgs::Float64 & msg);


extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

bool sent_message;

static uint32_t error_flag;


extern "C" void task_blue_arm_fn(void) {
    owr_messages::pwm pwm_buffer[5]; //do we need this? 
    // this creates a node handle
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("blue_arm_fn", "blue_arm_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    ros_echronos::ROS_INFO("Done init\n");
    nh_ptr = &nh;

    ros_echronos::ROS_INFO("Initalising BlueTongue arm subscribers\n");
    // Creating the subscribers

    std_msgs::Float64 arm_rotate_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> armRotateSub("/arm_base_rotate_controller/command", arm_rotate_buffer_in, 5, armRotateCallback);
    armRotateSub.set_topic_id(0);
    std_msgs::Float64 arm_top_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> armTopSub("/arm_top_controller/command", arm_top_buffer_in, 5, armTopCallback);
    armTopSub.set_topic_id(1);
    std_msgs::Float64 arm_bottom_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> armBottomSub("/arm_bottom_controller/command", arm_bottom_buffer_in, 5, armBottomCallback);
    armBottomSub.set_topic_id(2);
    std_msgs::Float64 claw_rotate_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> clawRotateSub("/claw_rotate_controller/command", claw_rotate_buffer_in, 5, clawRotateCallback);
    clawRotateSub.set_topic_id(3);
    std_msgs::Float64 claw_grip_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> clawGripSub("/claw_grip_controller/command", claw_grip_buffer_in, 5, clawGripCallback);
    clawGripSub.set_topic_id(4);

    armRotateSub.init(nh);
    armTopSub.init(nh);
    armBottomSub.init(nh);
    clawRotateSub.init(nh);
    clawGripSub.init(nh);

    
    //servo_init(HS_785HB, ARM_ROTATE_PIN);
    //servo_init(HS_785HB, CLAW_ROTATE_PIN);

    //pwm_init(ARM_TOP_PIN);
    //pwm_init(ARM_BOTTOM_PIN);
    //pwm_init(CLAW_GRIP_PIN);


    /* pwm_set_period(PWM_PAIR0, PWM_PERIOD); //not 100% about this one
    pwm_set_duty(ARM_TOP_PIN,15.0); //or these... is 15 a standard #?
    pwm_set_duty(ARM_BOTTOM_PIN,15.0);
    pwm_set_duty(CLAW_GRIP_PIN,15.0); */


    //pwm_enable(ARM_TOP_PIN);
    //pwm_enable(ARM_BOTTOM_PIN);
    //pwm_enable(CLAW_GRIP_PIN);


    ros_echronos::ROS_INFO("starting the main loop\n");
    owr_messages::pwm msg;
    while(true) {
        // this causes the callbacks to be called
        nh.spin();
    }
}

void callback(const owr_messages::pwm & msg) {
    ros_echronos::ROS_INFO("\n\nReceived Full Message %d\n", msg.from_msg_num);
    ros_echronos::ROS_INFO("\tJoint \"%s\"\n", msg.joint.values);
    ros_echronos::ROS_INFO("\tpwm %d\n", msg.pwm);
    ros_echronos::ROS_INFO("\ttarget vel %lf\n", msg.targetVel);
    ros_echronos::ROS_INFO("\ttarget pos %lf\n", msg.targetPos);
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
// finds the duty cycle of the motors in terms of a percentage
// 
static duty_pct speed_to_duty_pct(double speed) {
    duty_pct duty = DUTY_NEUTRAL + (speed / MAX_SPEED * MAP_SPEED_TO_DUTY_PCT);

    return duty;
}

static double wheel_to_servo_angle(double wheel_angle) {
    return wheel_angle * SERVO_ANGLE_CONVERSION_FACTOR;
}

/*
#define FULL_EXTENSION 1700
//1700
#define FULL_RETRACTION 1200

#define DEFAULT_POS 1500

#define OPEN  0
#define STOP  1
#define CLOSE 2*/

//make a better function name
// valid claw grip values range 1200 - 1700, 1500 is default/neutral
static float claw_grip_convert (int clawGripPos) {
    float clawGrip = (float)clawGripPos/(float)CLAW_ROTATION_MAX*1000.0 + 1000;
}

//callback for the base rotation of the arm
//void armRotateCallback(const std_msgs::Float64 & msg) {
    //servo_write_rads(HS_785HB, ARM_ROTATE_PIN, wheel_to_servo_angle(msg.data));
//    UARTprintf("Arm rotate received.\n");
//}

void armTopCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(ARM_TOP_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Arm top received.\n");
}

void armBottomCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(ARM_BOTTOM_PIN, speed_to_duty_pct(msg.data));
    UARTprintf("Arm bottom received.\n");
}

// needs arduino code?
void clawRotateCallback(const std_msgs::Float64 & msg) {
    servo_write_rads(HS_785HB, CLAW_ROTATE_PIN, wheel_to_servo_angle(msg.data));
    UARTprintf("Claw rotate received.\n");
}
//arduino code instead of duty
void clawGripCallback(const std_msgs::Float64 & msg) {
    pwm_set_duty(CLAW_GRIP_PIN, claw_grip_convert(msg.data));
    UARTprintf("Claw grip received.\n");
}
