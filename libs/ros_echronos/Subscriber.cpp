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
        : incoming_msgs(read_buffer), message_construction_buff_size(buffer_size/2), ready_msgs(incoming_msgs + message_construction_buff_size, buffer_size-message_construction_buff_size), callback(callback) {

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
    T * msg_ptr = NULL;

    // Step 1: Check if it is a new or existing message
    //ros_echronos::ROS_INFO("Receiving seq %d\n", msg.head.fields.seq_num);
    if (msg.head.fields.seq_num == 0) {
        msg_ptr = new (next_construction_msg()) T();
    } else {
        //try and match a buffer
        for(uint32_t i = 0, lmask=1; i < message_construction_buff_size; ++i, lmask = lmask<< 1) {
            if((lmask & mask) &&incoming_msgs[i].from_node == msg.head.fields.node_id) {
                // because we have a limited bits in the seq number we overflow to the message length field
                int msg_seq_num = msg.head.fields.seq_num != ros_echronos::can::SEQ_NUM_SPECIAL_MODE ? msg.head.fields.seq_num : msg.head.fields.message_length;
                // handle the case where we've dropped a packet
                if(
                        incoming_msgs[i].from_msg_num != msg.head.fields.message_num ||
                        msg_seq_num != incoming_msgs[i].decode_index
                ) {
                    if(mode == DROP_MISSING) {
                        ros_echronos::ROS_INFO("Dropped a packet got seq %d exp %d, mnum %d emnum %d\n",
                                               msg_seq_num,
                                               incoming_msgs[i].decode_index,
                                               msg.head.fields.message_num,
                                               incoming_msgs[i].from_msg_num
                        );
                        clear_slot(incoming_msgs+i);
                        msg_ptr = NULL;
                    } else {
                        ros_echronos::ROS_INFO("Chosen Missing Packet Function Not Implemented!\n");
                    }
                    continue;
                } else {
                    msg_ptr = incoming_msgs + i;
                }
                break;
            }
        }

    }
    if(msg_ptr) {
        msg_ptr->fill(msg);
        if (msg_ptr->is_done()) {
            T * a = ready_msgs.put(msg_ptr);
            ros_echronos::ROS_INFO("Done %d\n", a->is_done());
            clear_slot(msg_ptr);
        }
    }
}

template <class T> T * Subscriber<T>::next_construction_msg() {
    uint32_t local_mask = 1;
    for(int i = 0; i < message_construction_buff_size; ++i, local_mask= local_mask << 1) {
        if(!(local_mask & mask)) {
            mask |= local_mask;
            return incoming_msgs + i;
        }
    }
    return NULL;
    //return incoming_msgs + ;
}

template <class T> void Subscriber<T>::call_callback() {
    //ros_echronos::ROS_INFO("call callback! len %d\n", ready_msgs.length());
    // for now we assume that the top of the buffer must be valid for any future messages to be valid
    while(!ready_msgs.is_empty()) {
        callback(ready_msgs.pop());
    }
}

template <class T> void Subscriber<T>::clear_slot(T *msg_ptr) {
    // assign an empty message to clear arrays, descriptors, etc
    T msg;
    *msg_ptr = msg;
    // clear the mask
    mask ^= (1 << (msg_ptr - incoming_msgs));

}
//TODO: flush unfinished messages from the buffer or rerequest them