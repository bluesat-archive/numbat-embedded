/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides the implementation of can for the tm4c
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include "can_impl.hpp"
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"


void ros_echronos::can::send_can(ros_echronos::can::CAN_ROS_Message & msg) {

    tCANMsgObject can_tx_message;
    can_tx_message.ui32MsgID = msg.head.bits;
    can_tx_message.ui32MsgIDMask = 0;
    can_tx_message.ui32Flags = MSG_OBJ_TX_INT_ENABLE | MSG_OBJ_EXTENDED_ID;
    can_tx_message.ui32MsgLen = msg.body_bytes;
    can_tx_message.pui8MsgData = msg.body;

    CANMessageSet(CAN0_BASE, 1, &can_tx_message, MSG_OBJ_TYPE_TX);
}

