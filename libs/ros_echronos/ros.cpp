/*
 * Date Started: 30/04/17
 * Original Author: Harry J.E Day <harry@dayfamilyweb.com>
 * Editors:
 * Purpose: This provides the core file for implementing ros on echronos
 * This code is released under the AGPLv3 License.
 * Copyright BLUEsat UNSW, 2017
 */

#include "ros.hpp"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "utils/uartstdio.h"

extern "C" void ros_can_int_handler(void) {

    uint32_t can_status = 0;

    // read the register
    can_status = CANIntStatus(ros_echronos::can::can_base, CAN_INT_STS_CAUSE);

    // in this case we are reciving a status interupt
    if(can_status == CAN_INT_INTID_STATUS) {
        // read the error status and store it to be handled latter
        ros_echronos::can::can_error_flag = CANStatusGet(ros_echronos::can::can_base, CAN_STS_CONTROL);
    } else {
        // we are reciving a message, TODO: handle this
        // for now we clear the interup so we can continue
        CANIntClear(ros_echronos::can::can_base, 1);

        // clear the error flag (otherwise we will store recive or write statuses)
        ros_echronos::can::can_error_flag = 0;

        // if we haven't just sent a message read.
        /*if(!sent_message) {
            CANMessageGet(CAN0_BASE, can_status, &rx_object, 0);
            UARTprintf("Received: %c\n", can_input_buffer);
        }
        sent_message = false;*/
    }

    UARTprintf("A Error Code %x\n", can_status);
}


int ros_echronos::can::can_error_flag;

uint32_t  ros_echronos::can::can_base;
