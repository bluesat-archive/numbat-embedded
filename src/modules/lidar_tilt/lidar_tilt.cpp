#include <rtos-kochab.h>
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "pwm.h"
#include "servo.h"
#include "std_msgs/Float64.hpp"
//#include "std_msgs/Header.hpp"
//#include "sensor_msgs/JointState.hpp"


#define LIDAR_TILT_PIN PWM1
#define SECONDS_DELAY 0.02876707662

ros_echronos::NodeHandle * volatile nh_ptr = NULL;

#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8


static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
void lidarTiltControlCallback(const std_msgs::Float64 & msg);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

bool sent_message;

static uint32_t error_flag;

//static ros_echronos::Publisher<sensor_msgs::JointState> *lidar_tilt_pub_ptr;

extern "C" void task_lidar_tilt_fn(void) {
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("lidar_tilt_fn", "lidar_tilt_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    nh_ptr = &nh;

    ros_echronos::ROS_INFO("Initialising lidar setpoint subscriber\n");
    std_msgs::Float64 lidar_tilt_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Float64> lidar_tilt_sub("lidar_tilt_controller/command", lidar_tilt_buffer_in, 5, lidarTiltControlCallback);
    lidar_tilt_sub.set_topic_id(0);
    lidar_tilt_sub.init(nh);

    /*
    ros_echronos::ROS_INFO("Initialising lidar position publisher\n");
    sensor_msgs::joint_states lidar_tilt_buffer_out[5];
    ros_echronos::Publisher<sensor_msgs::JointState> lidar_tilt_pub("joint_states", lidar_tilt_buffer_out, 5, false);
    lidar_tilt_pub.init(nh);
    lidar_tilt_pub_ptr = &lidar_tilt_pub;
    */

    pwm_init(LIDAR_TILT_PIN); // redundant but needed to link driver library
    servo_init(LIDAR_TILT, LIDAR_TILT_PIN);
    servo_write_rads(LIDAR_TILT, LIDAR_TILT_PIN, 0.0);

    ros_echronos::ROS_INFO("starting the main loop\n");
    while(true) {
        /*
        UARTprintf("Enter degrees > "); 
        char pos[8];
        UARTgets(pos, 8);
        float duty = atof(pos); 
        servo_write(LIDAR_TILT, LIDAR_TILT_PIN, pos);
        */
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

void lidarTiltControlCallback(const std_msgs::Float64 & msg) {
    UARTprintf("lidar setpoint = %d degrees\n", msg.data * 180.0/PI);
    servo_write_rads(LIDAR_TILT, LIDAR_TILT_PIN, msg.data);
    /*
    sensor_msgs::JointState lidar_state;
    lidar_state.header.stamp = ros::Time::now() + ros::Duration(SECONDS_DELAY);
    // lidar_state.header.frame_id = 0;
    lidar_state.position = msg.data;
    lidar_state.velocity = 0;
    // lidar_state.effort = 0;
    lidar_tilt_pub_ptr->publish(lidar_state, 0);
    */
}
