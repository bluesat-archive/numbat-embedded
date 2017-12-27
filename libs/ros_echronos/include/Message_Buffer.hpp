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
        T * put(T msg);
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
        T *operator[](int index);

};

/**
 * Provides a thread safe buffer for incoming can messages
 */
class Incoming_Message_Buffer : Message_Buffer<ros_echronos::can::can_ros_message> {
     public:
        ros_echronos::can::can_ros_message buffer[ROS_CAN_INPUT_BUFFER_SIZE];

        /**
         * Constructs a new fixed size buffer with the given mutex
         * @param mutex the mutex to use when modifying the buffer
         */
        Incoming_Message_Buffer(RtosMutexId mutex);

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


#endif //NUMBAT_EMBEDDED_MESSAGE_BUFFER_H
