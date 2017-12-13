/*
 * @date: 13/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: 
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/Message_Buffer.hpp"

//because templates
#include "Message_Buffer.cpp"

ros_echronos::can::input_buffer_t ros_echronos::can::input_buffer;

RtosInterruptEventId ros_echronos::can::can_interupt_event;

Incoming_Message_Buffer::Incoming_Message_Buffer(RtosMutexId mutex) :
        Message_Buffer(buffer, ROS_CAN_INPUT_BUFFER_SIZE),
        mutex(mutex) {


}

ros_echronos::can::can_ros_message Incoming_Message_Buffer::pop_locked() {
    ros_echronos::can::can_ros_message msg;
    rtos_mutex_lock(mutex);
    msg = pop();
    rtos_mutex_unlock(mutex);
    return msg;
}

void Incoming_Message_Buffer::put_locked(ros_echronos::can::can_ros_message &msg) {
    rtos_mutex_lock(mutex);
    put(msg);
    rtos_mutex_unlock(mutex);
}
