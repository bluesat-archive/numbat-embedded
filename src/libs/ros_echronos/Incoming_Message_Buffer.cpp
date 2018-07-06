/*
 * @date: 13/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Implements a buffer that works with the CAN interupt
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/Message_Buffer.hpp"
#include "include/can_impl.hpp"

//because templates
#include "templates/Message_Buffer.cpp"

_Incoming_Message_Buffer * ros_echronos::can::incoming_msg_buffer;
//rigtorp::SPSCQueue<ros_echronos::can::CAN_ROS_Message, 5> ros_echronos::can::msg_queue;

RtosInterruptEventId ros_echronos::can::can_interupt_event;

_Incoming_Message_Buffer::_Incoming_Message_Buffer() :
        Message_Buffer(buffer, ROS_CAN_INPUT_BUFFER_SIZE) {

}

ros_echronos::can::can_ros_message _Incoming_Message_Buffer::pop_locked() {
    ros_echronos::can::can_ros_message msg;
    ros_echronos::can::can_receive_lock();
    msg = pop();
    ros_echronos::can::can_receive_unlock();

    return msg;
}

void _Incoming_Message_Buffer::put_locked(ros_echronos::can::can_ros_message &msg) {
    // note the assumption is this is called inside the can interupt and cannot be
    // interupted by any tasks that may edit the buffer
    put(&msg);
}
template class Message_Buffer<ros_echronos::can::can_ros_message>;
