/*
 * @date: 26/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors)
 * @details: Defines a task to run the ros subscriber task in
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "boilerplate.h"
#include "include/ros.hpp"
#include "NodeHandle.hpp"
extern ros_echronos::NodeHandle * volatile nh_ptr = NULL;

extern "C" void task_can_wait_task_fn(void) {

    UARTprintf("Entered wait task. Waiting for node handle to be initalised\n");
    while (!nh_ptr) {
        rtos_sleep(2);
    }

    UARTprintf("Node Handle Initalised\n");

    nh_ptr->run_handle_message_loop();
    UARTprintf("Message Loop Exited Unexpectedly!\n");

}
