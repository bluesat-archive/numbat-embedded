/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing publishing ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "include/NodeHandle.hpp"
#include "include/Publisher.hpp"
#include "boilerplate.h"
// needed because template
#include "Message_Buffer.cpp"

using namespace ros_echronos;

template <class T> Publisher<T>::Publisher(
        char *topic_name,
        T * const write_buffer,
        uint8_t buffer_size,
        bool latch
    ) : buffer(write_buffer, buffer_size) {


}

template <class T> Publisher<T>::~Publisher() {
    if(next != NULL) {
        if(prev != NULL) {
            prev->next = next;
            next->prev = prev;
        } else {
            nh->publishers = (_Publisher*) next;
            // TODO: move start of list
        }
    } else if (prev != NULL) {
        prev->next = NULL;
    } else {
        nh->publishers = NULL;
    }
}

template <class T> void Publisher<T>::init(ros_echronos::NodeHandle & node_handle) {
    nh = &node_handle;
    prev = NULL;
    if(nh->publishers != NULL) {
        next = nh->publishers;
        next->prev = this;
    }
    nh->publishers = this;
    header.fields.mode = can::ROS_CAN_MODE;
    header.fields.ros_function = can::FN_ROS_MESSAGE_TRANSMISSION;
    header.fields.node_id = nh->get_node_id();
    header.fields.not_in_range = 0;

    // TODO: topic registration
    header.fields.topic = 1;
}

template <class T> void Publisher<T>::publish(T & message, uint8_t priority) {
    buffer.put(&message);
    //TODO: priority
}

template <class T> __attribute__((used))
ros_echronos::can::CAN_ROS_Message Publisher<T>::get_next_message(bool &has_next, bool &empty)  {
    using namespace ros_echronos::can;
    CAN_ROS_Message can_msg;

    if(!message_in_progress) {
        if(buffer.is_empty()) {
            empty = true;
            return  can_msg;
        }
        current_message.~T();
        //current_message.~Message();
        current_message = buffer.pop();
        seq_num = 0;
        ++msg_num;
    }

    can_msg.head = header;

    uint8_t * current = current_message.get_next_block(has_next, can_msg.body_bytes);

    memcpy(can_msg.body, current, can_msg.body_bytes);
    ros_echronos::ROS_INFO("msg: 0: %c, 1: %c\n", can_msg.body[0], can_msg.body[1]);
    can_msg.head.fields.message_length = current_message.message_size();
    can_msg.head.fields.seq_num = seq_num++;
    can_msg.head.fields.message_num = msg_num;
    if (can_msg.head.fields.seq_num >= can::SEQ_NUM_SPECIAL_MODE) {
        can_msg.head.fields.seq_num = can::SEQ_NUM_SPECIAL_MODE;
        can_msg.head.fields.message_length = seq_num;
    }
    message_in_progress = has_next;

    // we also have next if there are more messages in the buffer
    has_next |= !buffer.is_empty();

    return can_msg;
}

