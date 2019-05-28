#include <rtos-kochab.h>
#include "boilerplate.h"
#include "ros.hpp"
#include "owr_messages/pwm.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"


#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
void callback(const owr_messages::pwm & msg);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

extern "C" void task_ros_sub_test_fn(void) {
    owr_messages::pwm pwm_buffer[5];

    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("ros_test_fn", "ros_test_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    ros_echronos::ROS_INFO("Done init\n");

    ros_echronos::ROS_INFO("sub init\n");
    ros_echronos::Subscriber<owr_messages::pwm> sub("/aaa", (owr_messages::pwm*)pwm_buffer, 5, callback);
    sub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);
    ros_echronos::ROS_INFO("starting the main loop\n");
//    owr_messages::pwm msg;
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
