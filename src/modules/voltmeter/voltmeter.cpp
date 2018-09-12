#include <rtos-kochab.h>
#include "boilerplate.h"
#include "ros.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "std_msgs/Float64.hpp"
#include "adc.h"

std_msgs::Float64 vm_buffer[5];
ros_echronos::NodeHandle * volatile nh_ptr = NULL;
ros_echronos::Publisher<std_msgs::Float64> * pub;

#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

bool sent_message;

static uint32_t error_flag;

static bool adc_cap;
static uint32_t adc_buf[1];


extern "C" void callback(void) {
    adc_cap = true;
}


extern "C" void task_voltmeter_fn(void) {

    // this creates a node handle
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("voltmeter_fn", "voltmeter_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    ros_echronos::ROS_INFO("Done init\n");
    nh_ptr = &nh;

    // publisher
    ros_echronos::ROS_INFO("pub init\n");
    ros_echronos::Publisher<std_msgs::Float64> _pub("/sensor/voltmeter", (std_msgs::Float64*)vm_buffer, 5, false);
    pub = &_pub;
    pub->init(nh);

    ros_echronos::ROS_INFO("starting the main loop\n");
    std_msgs::Float64 msg;
    msg.data = 0.0;
    adc_interrupt_enable();
    while (true) {
        adc_cap = false;
        adc_capture_interrupt(adc_buf, callback);

        while (!adc_cap) {
            rtos_sleep(1);
        }

        msg.data = (double)adc_buf[0];
        pub->publish(msg);
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
    enum adc_pin pin[1] = {AIN3};
    adc_init_pins(pin, 1, true);

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

