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

template <class T>
class Message_Buffer {

    public:
        T * const buffer_start;
        T * buffer_head;
        T * buffer_tail;
        T * const buffer_end;

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
        void put(T msg);
        bool is_empty();

};


#endif //NUMBAT_EMBEDDED_MESSAGE_BUFFER_H
