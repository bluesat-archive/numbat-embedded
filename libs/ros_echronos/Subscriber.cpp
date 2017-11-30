/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing subscribing to ros messages
 * @copydetails: This code is released under the LGPLv3 License and the BSD License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */


#include <can_impl.hpp>
#include "include/Subscriber.hpp"

using namespace ros_echronos;


template <class T> Subscriber<T>::Subscriber(char *topic_name, T *const read_buffer, int buffer_size, void (* callback)(const T &))  {

}

template <class T> Subscriber<T>::~Subscriber() {

}

template <class T> void Subscriber<T>::init(ros_echronos::NodeHandle &node_handle) {
    sub_id = can::subscribe_can(0,can::CAN_TOPIC_FILTER_BITMASK); //TODO: add node id, function, etc

}

template <class T> void Subscriber<T>::unsubscribe() {
    if(sub_id) {
        can::unsubscribe_can(sub_id);
        sub_id = 0;
    }
}