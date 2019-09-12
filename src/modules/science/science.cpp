// Module for science module on the NUMBAT rover
// Author: William Miles
// Date: 2019-09-12
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

#define SYSTICKS_PER_SECOND     100

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

#define ROTATE_PIN PWM0

void rotate_callback(const std_msgs::Float64 & msg);

extern "C" void task_science_fn(void) {

    ros_echronos::ROS_INFO("Entered science task. Initialising...\n");

    // init node handle
    ros_echronos::NodeHandle nh;
    nh.init("science_fn", "science_fn", 
            RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, 
            RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL, 
            RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    ros_echronos::ROS_INFO("Initialised node handle.\n");

    ros_echronos::ROS_INFO("Initialising subscribers...\n");

    // init rotate subscriber
    std_msgs::Float64 rotate_buffer[5];
    ros_echronos::Subscriber<std_msgs::Float64> sub("/e/sci", rotate_buffer, 
            5, rotate_callback);
    sub.init(nh, RTOS_SIGNAL_ID_ROS_PROMISE_SIGNAL);

    ros_echronos::ROS_INFO("Subscribers initialised.\n");

    // init rotate servo pwm output
    servo_init(HS_785HB, ROTATE_PIN);

    // wait for ROS events
    ros_echronos::ROS_INFO("Starting the main loop.\n");
    while(true) {
        nh.spin();
    }
}

void rotate_callback(const std_msgs::Float64 & msg) {
    ros_echronos::ROS_INFO("Received science rotate message.\n");
    servo_write_rads(HS_785HB, ROTATE_PIN, msg.data);
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

