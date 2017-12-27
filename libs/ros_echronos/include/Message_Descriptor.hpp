/*
 * @date: 26/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: 
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "ros.hpp"
#ifndef NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP
#define NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP

namespace ros_echronos {

    /**
     * Class used to write encoded data into a message class.
     * Note: a descriptor can only be used for one decoding
     */
    class Message_Descriptor {
        public:

            /**
             * Decode the given message into the message that created this descriptor
             * @param msg the msg to decode
             * @note: the message class this class references may be in an inconsistent state
             *  until all messages are decoded
             */
            void decode_msg(can::CAN_ROS_Message & msg);
        //protected:
            /**
             * Creates a new Message Descriptor with references to the fields
             * @param field_ptrs pointers to each of the fields in the message
             * @param field_size the size of each field, 0 represents a variable length field
             * @param num_fields the number of fields in the message (and thus the number of elements in each array)
             */
            Message_Descriptor(void ** field_ptrs, size_t * field_size, size_t num_fields);

            /**
             * Destroys the descriptor
             */
            ~Message_Descriptor();
        private:
            void ** field_ptrs;
            size_t * field_size;
            size_t num_fields;
            /**
             * The field we are currently on
             */
            uint16_t field_offset = 0;
            /**
             * If we have an offset inside the field
             */
            size_t field_internal_offset = 0;
            /**
             * If we have to split halfway through a length
             */
            bool decoding_len = false;

        // allow Messages to use our protected methofs
        //friend class Message;

    };
}

#endif //NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP
