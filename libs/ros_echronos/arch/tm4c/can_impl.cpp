/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides the implementation of can for the tm4c
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include <driverlib/can.h>
#include <inc/hw_memmap.h>
#include "include/can_impl.hpp"
#include "../../include/ros.hpp"


void ros_echronos::can::send_can(ros_echronos::can::CAN_ROS_Message & msg) {

    tCANMsgObject can_tx_message;
    can_tx_message.ui32MsgID = msg.head.bits;
    can_tx_message.ui32MsgIDMask = 0;
    can_tx_message.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    can_tx_message.ui32MsgLen = msg.body_bytes;
    can_tx_message.pui8MsgData = msg.body;

    CANMessageSet(CAN0_BASE, msg.head.bits, &can_tx_message, MSG_OBJ_TYPE_TX);
}


