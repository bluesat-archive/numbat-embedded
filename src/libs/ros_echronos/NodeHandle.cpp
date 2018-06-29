/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides the implementation of the Node Handle class
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "boilerplate.h"
#include "include/NodeHandle.hpp"
#include "include/Publisher.hpp"
#include "include/Subscriber.hpp"
#include "include/can_impl.hpp"
#include "include/can.hpp"
#include "include/can/register_node.hpp"
#include <atomic>


using namespace ros_echronos;

volatile bool can::node_handle_ready = false;


void NodeHandle::init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event,
                      RtosSignalId can_interupt_signal, RtosSignalId register_node_signal) {
    ros_echronos::ROS_INFO("can interupt event %d\n", can_interupt_event);
    can::can_interupt_event = can_interupt_event;
    can_receive_signal = can_interupt_signal;
    can::incoming_msg_buffer = &in_buff;
    has_init = true;
    can::node_handle_ready = true;
    // we do this here so we won't be waiting infinetly for ourselves
    do_register_node(node_name, register_node_signal);
}

void NodeHandle::do_register_node(char *node_name, RtosSignalId msg_signal) {
    using namespace ros_echronos::can::control_0_register;
    using namespace ros_echronos::can;
    Register_Header header;
    //TODO: hash
    header.fields.hash = 0;
    CAN_ROS_Message msg;
    msg.head.bits = header.bits;
    strncpy(reinterpret_cast<char *>(msg.body), node_name, CAN_MESSAGE_MAX_LEN);
    // the actual message will be the same apart from the step number
    Register_Header match_reg_head = header;
    match_reg_head.fields.step = 1;
    CAN_Header match_head  = REGISTER_BASE_FIELDS;
    match_head.bits |= match_reg_head.bits;
    // register the check before we send so we don't mis it
    // we register the signal here so we can catch it anyway
    CAN_Header reg_head;
    // for some reason static initilisation of this does not work
    reg_head.bits = _register_header_mask_base_fields.bits
                        | _register_header_mask_f2_fields.bits
                        | _register_header_mask_reg_fields.bits;
    promise::CANPromise * promise = promise_manager.match(reg_head ,match_head);
    ros_echronos::can::send_can(msg);

    // we create this here as its only used for the next operation (wish we could use curying here...)
    // because we wait here it is safe to do this on the stack
    typedef struct _on_error_data {
        NodeHandle * this_node;
        char * node_name;
        RtosSignalId msg_signal;
    } On_Error_Data;
    On_Error_Data on_error_data;
    on_error_data.this_node = this;
    on_error_data.node_name = node_name;
    on_error_data.msg_signal = msg_signal;
    promise->then((promise::PromiseFn)([](can::CAN_ROS_Message & msg, void * data) {
        can::control_0_register::Register_Response_Body bdy;
        memcpy(bdy.bytes, msg.body, CAN_MESSAGE_MAX_LEN);
        // we can't capture because it requires std lib stuff
        // but we can pass ourselves as a pointer
        ((NodeHandle*)data)->node_id = bdy.fields.node_id;
    }), this)->on_error([](can::CAN_ROS_Message & msg, void * data){
        // all we can do is try again
        ((On_Error_Data*)data)->this_node->do_register_node(((On_Error_Data*)data)->node_name, ((On_Error_Data*)data)->msg_signal);
    }, &on_error_data)->wait(msg_signal);
}

void NodeHandle::spin() {
    using namespace can;
    _Publisher * current =  publishers;
    if(current) {
        do {
            bool has_next  __attribute__((aligned(8))) = false;
            do {
                bool empty  __attribute__((aligned(8))) = false;
                CAN_ROS_Message msg = current->get_next_message(has_next, empty);
                if (!empty) {
                    can::send_can(msg);
                }
            } while (has_next);
        } while (current = (_Publisher *) current->next);
    }

    _Subscriber * current_sub = subscribers;
    if(current_sub) {
        do {
            current_sub->call_callback();
        } while(current_sub = (_Subscriber *)current_sub->next);
    }
    // TODO: receive
}

uint8_t NodeHandle::get_node_id() {
    return node_id;
}

void NodeHandle::run_handle_message_loop() {

    using namespace ros_echronos::can;
    ros_echronos::ROS_INFO("Waiting for NodeHandle to init\n");
    while(!has_init) {
        rtos_sleep(3);
    }
    ros_echronos::ROS_INFO("NodeHandle init done\n");
    CAN_ROS_Message msg;
    while(true) {
        rtos_signal_wait(can_receive_signal);

        //TODO: check the queue is not empty, although if we get here it shouldn't be
//        if(msg_queue.front()) {
//            msg_ptr = msg_queue.front();
//            msg = *msg_ptr;
//            //msg = *(msg_queue.front());
//            msg_queue.pop();

        msg = in_buff.pop_locked();
        // if its a promised message, don't bother with the decoding
        if(promise_manager.match_message(msg)) {
            continue;
        }

        if (msg.head.fields.base_fields.mode == (unsigned int)FN_ROS_MESSAGE_TRANSMISSION) {
            _Subscriber *current;
            for (current = subscribers; current; current = (_Subscriber *) current->next) {
                if (msg.head.fields.f0_ros_msg_fields.topic == current->topic_id) {
                    current->receive_message(msg);
//                ROS_INFO("Found a match!\n");
                    break;
                } else {
                    //ROS_INFO("No match for topic %d w/ %d", msg.head.fields.topic, current->topic_id);
                }

            }
        } else if (msg.head.fields.base_fields.mode == (unsigned int)FN_ROS_CONTROL_MSG) {
            //TODO: do control message things...
        }
    }
}


void NodeHandle::handle_ctrl_msg(ros_echronos::can::CAN_ROS_Message msg) {

}