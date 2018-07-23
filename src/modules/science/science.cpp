#include <rtos-kochab.h>
#include "boilerplate.h"
#include "ros.hpp"
#include "owr_messages/science.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "std_msgs/Bool.hpp"
#include "LIS3MDL.h"
#include "SI7021.h"
#include "TCS34725.h"
#include "LIS3MDL.h"

ros_echronos::NodeHandle * volatile nh_ptr = NULL;

#define SYSTICKS_PER_SECOND     100
#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];
static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
void data_request_callback(const std_msgs::Bool &msg);


extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

bool sent_message;

static uint32_t error_flag;

static TCS34725 tcs34725(I2C0);
static LIS3MDL lis3mdl(I2C0);
static SI7021 si7021(I2C0);
static ros_echronos::Publisher<owr_messages::science> *science_pub;

void task_science_fn(void) {
    // this creates a node handle
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("science_fn", "science_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    ros_echronos::ROS_INFO("Done init\n");
    
    nh_ptr = &nh;

    // Create the subscriber
    ros_echronos::ROS_INFO("Data request sub init\n");
    std_msgs::Bool science_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Bool> science_sub("science/request", 
                                    (std_msgs::Bool*) science_buffer_in, 5, data_request_callback);
    science_sub.set_topic_id(1);
    science_sub.init(nh);
    
    // Create the publisher
    ros_echronos::ROS_INFO("Data pub init\n");
    owr_messages::science science_buffer_out[5];
    science_pub = new ros_echronos::Publisher<owr_messages::science>("science/data", 
                                    (owr_messages::science*) science_buffer_out, 5, false);
    science_pub->init(nh);

    tcs34725.init();
    lis3mdl.init();
    si7021.init();

    while(true) {
        // this causes the callbacks to be called
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

void data_request_callback(const std_msgs::Bool &msg) {
    // create a message containing sensor data
    owr_messages::science data_msg;
    data_msg.temperature = si7021.read_temperature();
    data_msg.humidity = si7021.read_humidity();
    lis3mdl.read_magnetism(&data_msg.m.x, &data_msg.m.y, &data_msg.m.z)
    uint16_t r, g, b, c;
    tcs34725.read_raw_data(&r, &g, &b, &c);
    data_msg.colour_temperature = tcs34725.calculate_colour_temperature(r, g, b);
    data_msg.illuminance = tcs34725.calculate_lux(r, g, b);
    // publish data
    science_pub->publish(data_msg, 0);
}


