/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides the implementation of the Node Handle class
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "include/NodeHandle.hpp"
#include "include/Publisher.hpp"
#include "include/can_impl.hpp"

using namespace ros_echronos;

void NodeHandle::init(char *node_name, char *ros_task) {

}

void NodeHandle::spin() {
    using namespace can;
    _Publisher * current = (_Publisher *) publishers;
    do {
        bool has_next  __attribute__((aligned(8))) = false;
        do {
            bool empty  __attribute__((aligned(8)))= false;
            CAN_ROS_Message msg = current->get_next_message(has_next, empty);
            if (!empty) {
                can::send_can(msg);
            }
        } while (has_next);
    } while(current = (_Publisher *) current->next);

    // TODO: receive
}

uint8_t NodeHandle::get_node_id() {
    //TODO
    return 1;
}