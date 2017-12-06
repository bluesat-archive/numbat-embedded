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
            void init(char *node_name, char *ros_task, RtosInterruptEventId can_interupt_event);

            /**
             * The first publisher in the publisher list
             */
            ListNode * publishers = NULL;

            /**
             * Causes all messages to be published
             */
            void spin();

            /**
             * Returns the Node ID
             */
             uint8_t get_node_id();

    };
}


#endif //NUMBAT_EMBEDDED_NODE_HANDLE_HPP
