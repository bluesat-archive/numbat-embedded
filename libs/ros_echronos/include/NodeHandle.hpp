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

#include <cstdint>
#include <stdlib.h>
#include "ListNode.hpp"
#include "rtos-kochab.h"
#include "Subscriber.hpp"
#include "Publisher.hpp"

namespace ros_echronos {
    /**
     * Manages the state of the ros system on this node
     * Only one instance should be created per protocol channel
     */
    class NodeHandle {

        public:
            /**
             * Registers the ros node with the controller, setsup any CAN handlers required
             * @param node_name
             * @param ros_task
             */
            void init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event,
                      RtosSignalId can_interupt_signal);

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

        private:
            /**
             * If init has been run
             */
            volatile bool has_init = false;

            /**
             * Signal for incoming can messages
             */
            RtosSignalId can_receive_signal;

    };
}


#endif //NUMBAT_EMBEDDED_NODE_HANDLE_HPP
