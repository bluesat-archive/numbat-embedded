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

/**
 * Used so we can run tasks based on the nh being ready
 */
ros_echronos::NodeHandle * volatile nh_ptr = NULL;


void NodeHandle::init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event,
                      RtosSignalId can_interupt_signal, RtosSignalId register_node_signal) {
    ros_echronos::ROS_INFO("can interupt event %d\n", can_interupt_event);
    can::can_interupt_event = can_interupt_event;
    can_receive_signal = can_interupt_signal;
    can::incoming_msg_buffer = &in_buff;
    has_init = true;
    can::node_handle_ready = true;
    nh_ptr = this;

    // we do this here so we won't be waiting infinetly for ourselves
    do_register_node(node_name, register_node_signal);
}

void NodeHandle::do_register_node(char *node_name, RtosSignalId msg_signal) {
    using namespace ros_echronos::can::control_0_register;
    using namespace ros_echronos::can;
    // register the check before we send so we don't mis it
    // we register the signal here so we can catch it anyway
    CAN_Header reg_header_mask = {0};
    // for some reason static initilisation of this does not work
    reg_header_mask.bits = _CTRL_HEADER_MASK_BASE_FIELDS.bits
                    | _CTRL_HEADER_MASK_F2_FIELDS.bits
                    | _register_header_mask_reg_fields.bits;
    ROS_INFO("C_H_M_B_F 0x%x, C_H_M_F_F 0x%x R_H_M_R_F 0x%x\n", _CTRL_HEADER_MASK_BASE_FIELDS.bits
                    , _CTRL_HEADER_MASK_F2_FIELDS.bits
                    , _register_header_mask_reg_fields.bits);
    Register_Header reg_specific_header = {0};
    reg_specific_header.fields.hash = hash(node_name);
    reg_specific_header.fields.step = 0;
    ROS_INFO("r_s_g 0x%x\n", reg_specific_header.bits);

    ROS_INFO("\nmsg.mode %ul\n\n", register_ctrl_fields.fields.f2_ctrl_msg_fields.mode);
    // build the message to send
    CAN_ROS_Message msg = {
      .head = register_ctrl_fields
    };
    
    msg.head.bits |= reg_specific_header.bits | CAN_CTRL_BASE_FIELDS.bits;

    //TODO: handle node names shorter than the message length
    strncpy((char *)(msg.body), node_name, CAN_MESSAGE_MAX_LEN);
    msg.body_bytes = CAN_MESSAGE_MAX_LEN;


    // build the header to match response, it will be the same apart from the step number
    Register_Header match_reg_specific_head = reg_specific_header;
    match_reg_specific_head.fields.step = 1;
    CAN_Header match_head = {};
    match_head.bits = CAN_CTRL_BASE_FIELDS.bits | register_ctrl_fields.bits; //REGISTER_BASE_FIELDS;
    match_head.bits |= match_reg_specific_head.bits;

    ROS_INFO("Registering %x mask %x", match_head.bits, reg_header_mask.bits);
    promise::CANPromise * promise = promise_manager.match(match_head ,reg_header_mask);
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
        ROS_INFO("CAN Success recv head: %x\n", msg.head.bits);
    }), this)->on_error([](can::CAN_ROS_Message & msg, void * data){
        // all we can do is try again
        ((On_Error_Data*)data)->this_node->do_register_node(((On_Error_Data*)data)->node_name, ((On_Error_Data*)data)->msg_signal);
        ROS_INFO("CAN Error\n");
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
    bool more = false;
    while(true) {
        rtos_signal_wait(can_receive_signal);

        //TODO: check the queue is not empty, although if we get here it shouldn't be
//        if(msg_queue.front()) {
//            msg_ptr = msg_queue.front();
//            msg = *msg_ptr;
//            //msg = *(msg_queue.front());
//            msg_queue.pop();

        do {
            msg = in_buff.pop_locked(more);
            // if its a promised message, don't bother with the decoding
            if (promise_manager.match_message(msg)) {
                continue;
            }

            if (msg.head.fields.base_fields.ros_function == (unsigned int) FN_ROS_MESSAGE_TRANSMISSION) {
                for (_Subscriber *current = subscribers; current; current = (_Subscriber *) current->next) {
                    if (msg.head.fields.f0_ros_msg_fields.topic == current->topic_id) {
                        current->receive_message(msg);
//                    ROS_INFO("Found a match!\n");
                        break;
                    } else {
//                    ROS_INFO("No match for topic %d w/ %d", msg.head.fields.f0_ros_msg_fields.topic, current->topic_id);
                    }

                }
            } else if (msg.head.fields.base_fields.ros_function == (unsigned int) FN_ROS_CONTROL_MSG) {
                //TODO: do control message things...
            } else {
                ROS_INFO("Recived Unmatched Header %x", msg.head.bits);
            }
        } while (more);
        // ^ the above loop allows us to handle big messages much more effectively
    }
}

