#include <rtos-kochab.h>
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"
#include "owr_messages/pwm.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"

owr_messages::pwm pwm_buffer[5];
ros_echronos::Publisher<owr_messages::pwm> * pub;

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

extern "C" void task_ros_test_fn(void) {

    UARTprintf("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("ros_test_fn", "ros_test_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT,  RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    UARTprintf("Done init\n");

    UARTprintf("pub init\n");
    pub->init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
     UARTprintf("pub init done\n");
    owr_messages::pwm msg;
    msg.currentPos=0.0;
    msg.targetPos=0.0;
    while(true) {
        UARTprintf("Next loop!\n");
        msg.currentPos+=1.5;
        msg.targetVel = 2;
        msg.pwm = 100;
        msg.targetPos+=2;
        ros_echronos::String j(15);
        msg.joint = j;
        memcpy(msg.joint.values, "Hello CAN!", 11);
        UARTprintf("pub pub\n");
        pub->publish(msg);
        UARTprintf("pub done\n");

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
    ros_echronos::Publisher<owr_messages::pwm> _pub("aaa", (owr_messages::pwm*)pwm_buffer, 5, false);
    pub = &_pub;

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

