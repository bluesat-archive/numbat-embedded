/**
 * @date 04/05/17
 * @author (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors Editors
 * @details Purpose This provides the implementation of the Node Handle class
 * @copydetails This code is released under the AGPLv3 License.
 * @copyright Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_NODE_HANDLE_HPP
#define NUMBAT_EMBEDDED_NODE_HANDLE_HPP

#include <stdlib.h>
#include "ListNode.hpp"
#include "rtos-kochab.h"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "CANPromise.hpp"

namespace ros_echronos {
    /**
     * Manages the state of the ros system on this node
     * Only one instance should be created per protocol channel
     */
    class NodeHandle {

        public:

            NodeHandle() : promise_manager(promise_buffer, &promise_bitmask, 3) {}
            /**
             * Registers the ros node with the controller, setsup any CAN handlers required
             * @param node_name the name of the node
             * @param ros_task
             * @param can_interupt_event the event for can interupts
             * @param can_interupt_signal the signal to be sent as a result of the interupt
             * @param register_node_signal the signal used temporarily to wait for the node registration response. Can be reused
             *        Can't be the same as `can_interupt_signal` as it will cause a deadlock
             */
            void init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event,
                      RtosSignalId can_interupt_signal, RtosSignalId register_node_signal);

            /**
             * The first publisher in the publisher list
             */
            _Publisher * publishers = NULL;

            /*
             * The first subscriber in the subscribers list
             */
            _Subscriber * subscribers = NULL;

            /**
             * Causes all messages to be published
             */
            void spin();

            /**
             * Returns the Node ID
             */
             uint8_t get_node_id();

            /**
             * Called by the highest priority task and used to handle incoming interupt singals
             * This function does not return!
             */
            void run_handle_message_loop();
            /**
             * Stores promises
             */
            promise::CANPromiseManager promise_manager;

        private:
            /**
             * If init has been run
             */
            volatile bool has_init = false;

            /**
             * Signal for incoming can messages
             */
            RtosSignalId can_receive_signal;

            /**
             * The message buffer
             */
            _Incoming_Message_Buffer in_buff;


            /**
             * Handler for incoming control messages
             * @param msg the message
             */
            void handle_ctrl_msg(ros_echronos::can::CAN_ROS_Message msg);

            // we want an empty buffer, not one that has been initalised
            uint8_t promise_buffer[sizeof(promise::CANPromise)*3];
            uint8_t promise_bitmask;

            /**
             * Register the node with the master
             * @param node_name the name of the node
             * @param msg_signal the signal to wait for
             */
            void do_register_node(char *node_name, RtosSignalId msg_signal);

            uint8_t node_id : NODE_ID_WIDTH;

    };
}


#endif //NUMBAT_EMBEDDED_NODE_HANDLE_HPP
