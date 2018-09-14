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

#define ARM_ROTATE_PIN PWM4
#define ARM_TOP_PIN PWM1
#define ARM_BOTTOM_PIN PWM2
#define CLAW_ROTATE_PIN PWM3
#define CLAW_GRIP_PIN PWM0

#define PWM_PERIOD 10.0
#define MAX 2.0 // ms 20%
#define MIN 1.0 // ms 10%
#define DUTY_RANGE (100*(MAX - MIN)/PWM_PERIOD) // 10%
#define DUTY_NEUTRAL ((1.5 * 100) / PWM_PERIOD) // 15%
//#define MAX_SPEED 1 // m/s
#define MAP_SPEED_TO_DUTY_PCT ((MAX*100)/PWM_PERIOD - DUTY_NEUTRAL) // (20-15)% = 5%

#define SERVO_ANGLE_CONVERSION_FACTOR 0.5 // 180 deg. / 360 deg.

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

void clawGripCallback(const std_msgs::Float64 & msg);
static duty_pct speed_to_duty_pct(double speed);
static float claw_grip_convert (int clawGripPos);
static double wheel_to_servo_angle(double wheel_angle);

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


    std_msgs::Float64 claw_grip_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> clawGripSub("/claw_grip_controller/command", claw_grip_buffer_in, 5, clawGripCallback);
    clawGripSub.set_topic_id(15);

    clawGripSub.init(nh);

    servo_init(GENERIC, CLAW_GRIP_PIN);
   


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
    duty_pct duty = DUTY_NEUTRAL + (speed * MAP_SPEED_TO_DUTY_PCT);

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


//arduino code instead of duty
void clawGripCallback(const std_msgs::Float64 & msg) {
    //pwm_set_duty(CLAW_GRIP_PIN, msg.data);
    if(msg.data != 0.0){
        servo_write_rads(GENERIC, CLAW_GRIP_PIN, msg.data);
    }
    UARTprintf("Claw grip received.\n");
}
