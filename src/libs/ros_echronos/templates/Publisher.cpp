/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing publishing ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "NodeHandle.hpp"
#include "Publisher.hpp"
#include "boilerplate.h"
// needed because template
#include "Message_Buffer.cpp"
#include "include/can/advertise_topic.hpp"

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


template <class T>
void Publisher<T>::init(ros_echronos::NodeHandle &node_handle, RtosSignalId promise_signal) {
    nh = &node_handle;
    register_topic(promise_signal);
    prev = NULL;
    if(nh->publishers != NULL) {
        next = nh->publishers;
        next->prev = this;
    }
    nh->publishers = this;
    header.fields.base_fields.mode = can::ROS_CAN_MODE;
    header.fields.base_fields.ros_function = can::FN_ROS_MESSAGE_TRANSMISSION;
    header.fields.f0_ros_msg_fields.node_id = nh->get_node_id();
    header.fields.f0_ros_msg_fields.not_in_range = 0;

    // TODO: topic registration
    header.fields.f0_ros_msg_fields.topic = topic_id;
}

template <class T> void Publisher<T>::publish(T & message, uint8_t priority) {
    buffer.put(&message);
    //TODO: priority
}

template <class T>
ros_echronos::can::CAN_ROS_Message Publisher<T>::get_next_message(bool &has_next, bool &empty)  {
    using namespace ros_echronos::can;
    CAN_ROS_Message can_msg;

    if(!message_in_progress) {
        if(buffer.is_empty()) {
            empty = true;
            return  can_msg;
        }
        current_message = buffer.pop();
        seq_num = 0;
        ++msg_num;
    }

    can_msg.head = header;

    uint8_t * current = current_message.get_next_block(has_next, can_msg.body_bytes);

    memcpy(can_msg.body, current, can_msg.body_bytes);
    ros_echronos::ROS_INFO("msg: 0: %c, 1: %c\n", can_msg.body[0], can_msg.body[1]);
    can_msg.head.fields.f0_ros_msg_fields.message_length = current_message.message_size();
    can_msg.head.fields.base_fields.seq_num = seq_num++;
    can_msg.head.fields.f0_ros_msg_fields.message_num = msg_num;
    if (can_msg.head.fields.base_fields.seq_num >= can::SEQ_NUM_SPECIAL_MODE) {
        can_msg.head.fields.base_fields.seq_num = can::SEQ_NUM_SPECIAL_MODE;
        can_msg.head.fields.f0_ros_msg_fields.message_length = seq_num;
    }
    message_in_progress = has_next;

    // we also have next if there are more messages in the buffer
    has_next |= !buffer.is_empty();

    return can_msg;
}

template <class T> void Publisher<T>::register_topic(const RtosSignalId signal_wait) {
    using namespace ros_echronos::can::control_4_advertise;
    using namespace ros_echronos::can;

    //calculate lengths
    const size_t topic_length = strlen(topic_name);
    const size_t msg_name_len = strlen(T::NAME);

    // build the request header
    Advertise_Header msg_head;
    msg_head.fields.step = 0;
    msg_head.fields.node_id = nh->get_node_id();
    msg_head.fields.length = (topic_length + msg_name_len + 1) / CAN_MESSAGE_MAX_LEN;
    msg_head.fields.hash = hash(topic_name); // this gets truncated but that's fine
    msg_head.fields.seq_num = 0;

    // build the response mask
    CAN_Header mask;
    mask.bits = _CTRL_HEADER_MASK_BASE_FIELDS.bits
                | _CTRL_HEADER_MASK_F2_FIELDS.bits
                | ADV_CTRL_HEADER_MASK.bits;

    // build the expected response header
    Advertise_Header response_head = msg_head;
    response_head.fields.step = 1;
    CAN_Header can_response_head;
    can_response_head.bits = response_head.bits | ADV_CTRL_HEADER.bits;
    promise::CANPromise * const promise = nh->promise_manager.match(can_response_head, mask);

    // build the strings and send messages
    CAN_ROS_Message msg;
    msg.head = ADV_CTRL_HEADER;
    msg.head.bits |= msg_head.bits;
    msg.body_bytes = CAN_MESSAGE_MAX_LEN;
    const uint8_t index_offset = control_4_advertise::send_string(msg, msg_head, topic_name, topic_length, 0);
    const uint8_t index = control_4_advertise::send_string(msg, msg_head, T::NAME, msg_name_len, index_offset+1);
    // if we haven't just sent a message and its not the last null terminator, send
    if((index != (CAN_MESSAGE_MAX_LEN-1)) && (index != 0)) {
        msg.body_bytes = index+1;
        send_can(msg);
    }

    typedef struct _recurse_data {
        Publisher * this_obj;
        RtosSignalId sig;
    } _Recurse_Data;

    _Recurse_Data recurse_data = {this, signal_wait};

    promise->then((promise::PromiseFn)([](can::CAN_ROS_Message & msg, void * data) {
        Response_Body resp;
        ROS_INFO("CAN Received Header %x\n", msg.head.bits);
        memcpy(resp.bytes, msg.body, msg.body_bytes);
        ((Publisher*)data)->topic_id = resp.fields.topic_id;
        ROS_INFO("%s got topic id %d\n", ((Publisher*)data)->topic_name, resp.fields.topic_id);

    }), this)->on_error([](can::CAN_ROS_Message & msg, void * data) {
        // all we can do is try again
        ((_Recurse_Data *) data)->this_obj->register_topic(((_Recurse_Data *) data)->sig);
    }, &recurse_data)->wait(signal_wait);
}
