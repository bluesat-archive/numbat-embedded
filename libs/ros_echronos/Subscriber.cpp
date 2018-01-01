/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing subscribing to ros messages
 * @copydetails: This code is released under the LGPLv3 License and the BSD License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "boilerplate.h"
#include <can_impl.hpp>
#include <NodeHandle.hpp>
#include "include/Subscriber.hpp"
#include "include/Message.hpp"
#include "Message_Buffer.cpp"

using namespace ros_echronos;


template <class T> Subscriber<T>::Subscriber(char *topic_name, T *const read_buffer, int buffer_size, void (* callback)(const T &))
        : incoming_msgs(read_buffer, buffer_size), callback(callback) {

}

template <class T> Subscriber<T>::~Subscriber() {

}

template <class T> void Subscriber<T>::init(ros_echronos::NodeHandle &node_handle) {
    can::can_ros_message msg;
    // TODO: replace with allocation negotiation
    topic_id = 1;

    msg.head.fields.message_length = 0;
    msg.head.fields.mode = 1;
    msg.head.fields.node_id = 0;
    msg.head.fields.priority = 0;
    msg.head.fields.ros_function = can::FN_ROS_MESSAGE_TRANSMISSION;
    msg.head.fields.seq_num = 0;
    msg.head.fields.topic = topic_id;
    sub_id = can::subscribe_can(msg.head.bits, can::TOPIC_BITMASK_HEADER.bits); //TODO: add node id, function, etc
    nh = &node_handle;
    prev = NULL;
    if(nh->subscribers != NULL) {
        //ros_echronos::ROS_INFO("null\n");
        next = nh->subscribers;
        next->prev = this;
    }
    //ros_echronos::ROS_INFO("not null\n");
    nh->subscribers = this;
    ros_echronos::ROS_INFO("done init\n");
}

template <class T> void Subscriber<T>::unsubscribe() {
    if(topic_id) {
        can::unsubscribe_can(topic_id);
        topic_id = 0;
    }
}

template <class T> void Subscriber<T>::receive_message(ros_echronos::can::CAN_ROS_Message &msg) {
    // Step 1: Check if it is a new or existing message
    ros_echronos::ROS_INFO("Receiving Message seq num %d\n", msg.head.fields.seq_num);
    if (msg.head.fields.seq_num == 0) {
        T t;
        T * msg_ptr = incoming_msgs.put(t);
        msg_ptr->fill(msg);
    } else {
        //try and match a buffer
        for(int i = 0; i < incoming_msgs.length(); ++i) {
            if(incoming_msgs[i]->from_node == msg.head.fields.node_id) {
                // because we have a limited bits in the seq number we overflow to the message length field
                int msg_seq_num = msg.head.fields.seq_num != ros_echronos::can::SEQ_NUM_SPECIAL_MODE ? msg.head.fields.seq_num : msg.head.fields.message_length;
                if(msg_seq_num != incoming_msgs[i]->decode_index) {
                    //TODO: error handling
                    ros_echronos::ROS_INFO("Missing Message!\n");
                } else {
                    ros_echronos::ROS_INFO("Adding to Message in the buffer\n");
                    incoming_msgs[i]->fill(msg);
                }
                break;
            }
        }

        // TODO: go into the buffer and find the message
    }
    ros_echronos::ROS_INFO("Finished Message\n");
}

template <class T> void Subscriber<T>::call_callback() {
    ros_echronos::ROS_INFO("call callback!\n");
    // for now we assume that the top of the buffer must be valid for any future messages to be valid
    while(!incoming_msgs.is_empty()) {
        if(incoming_msgs[0]->is_done()) {
            callback(incoming_msgs.pop());
        } else {
            break;
        }
    }
}

//TODO: flush unfinished messages from the buffer or rerequest them