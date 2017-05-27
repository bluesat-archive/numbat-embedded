/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing publishing ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "include/NodeHandle.hpp"
#include "include/Publisher.hpp"

using namespace ros_echronos;

template <class T> Publisher<T>::Publisher(
        char *topic_name,
        T * const write_buffer,
        uint8_t buffer_size,
        bool latch
    ) : buffer(write_buffer, buffer_size) {


}

template <class T> Publisher<T>::~Publisher() {
    if(next != NULL) {
        if(prev != NULL) {
            prev->next = next;
            next->prev = prev;
        } else {

            // TODO: move start of list
        }
    } else if (prev != NULL) {
        prev->next = NULL;
    }
}

template <class T> void Publisher<T>::init(ros_echronos::NodeHandle & node_handle) {
    nh = &node_handle;
}

template <class T> void Publisher<T>::publish(T message, uint8_t priority) {
    buffer.put(message);
    //TODO: priority
}




