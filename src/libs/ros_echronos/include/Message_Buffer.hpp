/**
 * @date: 27/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a buffer for messages. Allows them to be queued in FIFO order
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_MESSAGE_BUFFER_H
#define NUMBAT_EMBEDDED_MESSAGE_BUFFER_H

#include "ros.hpp"
#include "can.hpp"

#define ROS_CAN_INPUT_BUFFER_SIZE 5

template <class T>
class Message_Buffer {

    public:
        T * const buffer_start;
        T * const buffer_end;
        T * buffer_head;
        T * buffer_tail;

        /**
         * Creates a new write buffer
         * @param buffer the buffer
         * @param size the size
         */
        Message_Buffer(T * const buffer, const uint8_t size);

        /**
         * Removes the next message in the queue
         * @return the next message
         *
         * @pre isEmpty() == False
         */
        T pop();
        /**
         * Adds a message to the buffer
         * @param msg the message to store
         * @return a pointer to the message in the buffer
         */
        T * put(T *const msg);
        bool is_empty();

        /**
         * Calculates and returns the length of the buffer
         * @return the length
         */
        size_t length();

        /**
         * Allows idempotent access to the buffer
         * @param index the index of the array
         * @return the pointer to the item in the buffer specified by the index
         */
        T *operator[](const int index);
    private:
        bool is_full = false;

};

/**
 * Provides a thread safe buffer for incoming can messages
 */
class _Incoming_Message_Buffer : public Message_Buffer<ros_echronos::can::can_ros_message> {
     public:
        ros_echronos::can::can_ros_message buffer[ROS_CAN_INPUT_BUFFER_SIZE];

        /**
         * Constructs a new fixed size buffer with the given mutex
         * @param mutex the mutex to use when modifying the buffer
         */
        _Incoming_Message_Buffer();

        /**
         * Removes the next message in the queue
         * @return the next message
         *
         * @pre isEmpty() == False
         */
        ros_echronos::can::can_ros_message pop_locked();
        /**
         * Add a message to the queue
         * @param msg the message to add
         */
        void put_locked(ros_echronos::can::can_ros_message & msg);

    private:
        RtosMutexId mutex;


};


template <class T> Message_Buffer<T>::Message_Buffer(T *const buffer, const uint8_t size) :
        buffer_start(buffer), buffer_end(buffer + size), buffer_head(buffer_end-1), buffer_tail(buffer_head)  {

}

template <class T> T Message_Buffer<T>::pop() {
        T msg = (*buffer_head);
        --buffer_head;
        if(buffer_head < buffer_start) {
            buffer_head = buffer_end-1;
        }
        // faster then doing a check
        is_full = false;
        //note as we assume isEmpty is false, we don't need to check the case where we decrement past the tail
        return msg;
}

template <class T>
T * Message_Buffer<T>::put(T *const msg) {
        (*buffer_tail) = *msg;
        T * output = buffer_tail;
        --buffer_tail;
        if(buffer_tail < buffer_start) {
            buffer_tail = buffer_end-1;
        }
        if(buffer_tail == buffer_head) {
             is_full = true;
        }
        // if we're full we need to step back the tail one
        if(is_full) {
             is_full = false;
             --buffer_head;
        }
        //TODO: override first item if full

        return output;
}

template <class T> inline bool Message_Buffer<T>::is_empty() {
        return buffer_head == buffer_tail && !is_full;
}

template <class T> size_t Message_Buffer<T>::length() {
        // the calc is wrong for this special case
        if(is_full) {
             return buffer_end - buffer_start;
        } else if(buffer_head > buffer_tail) {
            return buffer_head - buffer_tail;
        } else {
            return (buffer_end - buffer_tail) + (buffer_head - buffer_start);
        }
}

template <class T> T * Message_Buffer<T>::operator[](const int index) {
        int nindex = (buffer_tail - buffer_start) + index;
        nindex = nindex % (buffer_start - buffer_end);
        return buffer_start + nindex;
}

#endif //NUMBAT_EMBEDDED_MESSAGE_BUFFER_H
