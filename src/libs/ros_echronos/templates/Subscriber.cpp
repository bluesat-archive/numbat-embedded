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
#include "Subscriber.hpp"
#include "include/can/subscribe_topic.hpp"
#include "Message.hpp"
#include "Message_Buffer.cpp"

using namespace ros_echronos;


template <class T> Subscriber<T>::Subscriber(char *topic_name, T *const read_buffer, int buffer_size, void (* callback)(const T &))
        : incoming_msgs(read_buffer),
          message_construction_buff_size(buffer_size/2),
          ready_msgs(incoming_msgs + message_construction_buff_size, buffer_size-message_construction_buff_size),
          callback(callback),
          topic_name(topic_name)
{

}

template <class T> Subscriber<T>::~Subscriber() {

}
template <class T> void Subscriber<T>::set_topic_id(int id) {
    topic_id = id;
}

template <class T>
void Subscriber<T>::init(ros_echronos::NodeHandle &node_handle, RtosSignalId ctrl_wait_sig) {
    can::can_ros_message msg = {0};

    nh = &node_handle;
    register_topic(ctrl_wait_sig);
    msg.head.fields.f0_ros_msg_fields.message_length = 0;
    msg.head.fields.f0_ros_msg_fields.message_num = 0;
    msg.head.fields.base_fields.mode = 1;
    // this is the "sending" node id, so we don't want to match on it
    msg.head.fields.f0_ros_msg_fields.node_id = 0;
    msg.head.fields.base_fields.priority = 0;
    msg.head.fields.base_fields.ros_function = can::FN_ROS_MESSAGE_TRANSMISSION;
    msg.head.fields.base_fields.seq_num = 0;
    msg.head.fields.f0_ros_msg_fields.topic = topic_id;
    const unsigned long mask = can::getTopicHeaderBitmask();
    // NOTE: the first parameter will in fact be '1' if the topic id is 0
    sub_id = can::subscribe_can(msg.head.bits, mask);
    prev = NULL;
    if(nh->subscribers != NULL) {
        //ros_echronos::ROS_INFO("null\n");
        next = nh->subscribers;
        next->prev = this;
    }
    //ros_echronos::ROS_INFO("not null\n");
    nh->subscribers = this;
    ros_echronos::ROS_INFO("Sub %d done init. Got sub id %d\n", topic_id, sub_id);
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
//    if(msg.head.fields.base_fields.seq_num != 0) {
//        ros_echronos::ROS_INFO("Receiving seq %d\n", msg.head.fields.base_fields.seq_num);
//    }
    if (msg.head.fields.base_fields.seq_num == 0) {
        msg_ptr = new (next_construction_msg()) T();
    } else {
        //try and match a buffer
        for(uint32_t i = 0, lmask=1; i < message_construction_buff_size; ++i, lmask = lmask<< 1) {
            if((lmask & mask) && incoming_msgs[i].from_node == msg.head.fields.f0_ros_msg_fields.node_id) {
                // because we have a limited bits in the seq number we overflow to the message length field
                const uint16_t msg_seq_num = msg.head.fields.base_fields.seq_num != ros_echronos::can::SEQ_NUM_SPECIAL_MODE ? msg.head.fields.base_fields.seq_num : msg.head.fields.f0_ros_msg_fields.message_length;
                // handle the case where we've dropped a packet
                if(
                        incoming_msgs[i].from_msg_num == msg.head.fields.f0_ros_msg_fields.message_num &&
                        msg_seq_num == incoming_msgs[i].decode_index
                ) {
                    msg_ptr = incoming_msgs + i;
                } else {
                    if(mode == DROP_MISSING) {
                        ros_echronos::ROS_INFO("Dropped a packet got seq %d exp %d, mnum %d emnum %d\n",
                                               msg_seq_num,
                                               incoming_msgs[i].decode_index,
                                               msg.head.fields.f0_ros_msg_fields.message_num,
                                               incoming_msgs[i].from_msg_num
                        );
                        clear_slot(incoming_msgs+i);
                        msg_ptr = NULL;
                    } else {
                        ros_echronos::ROS_INFO("Chosen Missing Packet Function Not Implemented!\n");
                    }
                    continue;
                }
                break;
            }
        }

    }
    if(msg_ptr) {
        msg_ptr->fill(msg);
        //ros_echronos::ROS_INFO("Fill done");
        if (msg_ptr->is_done()) {
            ready_msgs.put(msg_ptr);
            //ros_echronos::ROS_INFO("Done %d\n", a->is_done());
            clear_slot(msg_ptr);
        }
    }
}

template <class T> T * Subscriber<T>::next_construction_msg() {
    uint32_t local_mask = 1;
    for(size_t i = 0; i != message_construction_buff_size; ++i, local_mask= local_mask << 1) {
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
    *msg_ptr = EMPTY_MSG;
    // clear the mask
    mask ^= (1 << (msg_ptr - incoming_msgs));

}


template <class T> void Subscriber<T>::register_topic(const RtosSignalId signal_wait) {
    using namespace ros_echronos::can::control_2_subscribe;
    using namespace ros_echronos::can;

    //calculate lengths
    const size_t topic_length = strlen(topic_name);
    const size_t msg_name_len = strlen(T::NAME);

    // build the request header
    Subscribe_Header msg_head = {0};
    msg_head.fields.step = 0;
    msg_head.fields.node_id = nh->get_node_id();
    const size_t  lengths = (topic_length + msg_name_len + 1);
    msg_head.fields.length = (lengths % CAN_MESSAGE_MAX_LEN) ? lengths / CAN_MESSAGE_MAX_LEN + 1 : lengths / CAN_MESSAGE_MAX_LEN;
    msg_head.fields.hash = hash(topic_name); // this gets truncated but that's fine
    msg_head.fields.seq_num = 0;

    // build the response mask
    CAN_Header mask = {0};
    mask.bits = _CTRL_HEADER_MASK_BASE_FIELDS.bits
                        | _CTRL_HEADER_MASK_F2_FIELDS.bits
                        | SUB_CTRL_HEADER_MASK.bits;

    // build the expected response header
    Subscribe_Header response_head = msg_head;
    response_head.fields.step = 1;
    CAN_Header can_response_head = add_common_headers(response_head);
    promise::CANPromise * const promise = nh->promise_manager.match(can_response_head, mask);

    // build the strings and send messages
    CAN_ROS_Message msg = {0};
    msg.head = add_common_headers(msg_head);
    ROS_INFO("header.mode %x", msg.head.fields.base_fields.mode);
    msg.body_bytes = CAN_MESSAGE_MAX_LEN;
    const uint8_t index_offset = send_string(msg, msg_head, topic_name, topic_length+1, 0);
    const uint8_t index = send_string(msg, msg_head, T::NAME, msg_name_len+1, index_offset+1);
    // if we haven't just sent a message and its not the last null terminator, send
    if((index != (CAN_MESSAGE_MAX_LEN-1)) && (index != 0)) {
        msg.body_bytes = index + 1;
        ROS_INFO("last packet size %u", msg.body_bytes);
        send_can(msg);
    }

    typedef struct _recurse_data {
        Subscriber * this_obj;
        RtosSignalId sig;
    } _Recurse_Data;

    _Recurse_Data recurse_data = {this, signal_wait};

    promise->then((promise::PromiseFn)([](can::CAN_ROS_Message & msg, void * data) {
        Response_Body resp;
        ROS_INFO("CAN Received Header %x\n", msg.head.bits);
        memcpy(resp.bytes, msg.body, msg.body_bytes);
        ((Subscriber<T>*)data)->topic_id = resp.fields.topic_id;
        ROS_INFO("%s got topic id %d\n", ((Subscriber*)data)->topic_name, resp.fields.topic_id);
        ROS_INFO("sub %x", data);
    }), this)->on_error([](can::CAN_ROS_Message & msg, void * data) {
        // all we can do is try again
        ROS_INFO("Got an error retrying");
        ((_Recurse_Data *) data)->this_obj->register_topic(((_Recurse_Data *) data)->sig);
    }, &recurse_data)->wait(signal_wait);
    ROS_INFO("this 0x%x, topic id %d", this, topic_id);
}
//TODO: flush unfinished messages from the buffer or rerequest them
