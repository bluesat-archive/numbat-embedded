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
            void init(char * node_name, char * ros_task);

    };
}


#endif //NUMBAT_EMBEDDED_NODE_HANDLE_HPP
