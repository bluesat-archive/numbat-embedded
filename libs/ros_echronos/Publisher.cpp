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

template <class T> Publisher<T>::Publisher(char *topic_name, T * const write_buffer, int buffer_size, bool latch) {

}

template <class T> Publisher<T>::~Publisher() {

}

template <class T> void init(ros_echronos::NodeHandle & node_handle) {

}

template <class T> void publish(T message, uint8_t priority) {

}



