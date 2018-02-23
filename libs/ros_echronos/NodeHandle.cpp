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


using namespace ros_echronos;

volatile bool can::node_handle_ready = false;
void NodeHandle::init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event,
                      RtosSignalId can_interupt_signal) {
    ros_echronos::ROS_INFO("can interupt event %d\n", can_interupt_event);
    can::can_interupt_event = can_interupt_event;
    can_receive_signal = can_interupt_signal;
    has_init = true;
    can::node_handle_ready = true;
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
    //TODO
    return 1;
}

void NodeHandle::run_handle_message_loop() {
    using namespace ros_echronos::can;
    ros_echronos::ROS_INFO("Waiting for NodeHandle to init\n");
    while(!has_init) {
        rtos_sleep(3);
    }
    ros_echronos::ROS_INFO("NodeHandle init done\n");
    int start_counter, end_counter;
    CAN_ROS_Message msg;
    while(true) {
        rtos_signal_wait(can_receive_signal);

        //ROS_INFO("start %d", input_buffer.start_counter);

        //TODO: check the queue is not empty, although if we get here it shouldn't be
        if(msg_queue.front()) {
            msg = *(msg_queue.front());
            msg_queue.pop();
            /*start_counter = input_buffer.start_counter;
            msg = input_buffer.buffer;
            end_counter = input_buffer.start_counter;*/

            // check we didn't interupt the message read half way through
            /*if(end_counter != start_counter) {
                //TODO: handle concurent requests
                ROS_INFO("Thread error\n");
                continue;
            }*/
            _Subscriber *current;
            for (current = subscribers; current; current = (_Subscriber *) current->next) {
                if (msg.head.fields.topic == current->topic_id) {
                    current->receive_message(msg);
                    break;
                }
            }
        }
    }
}