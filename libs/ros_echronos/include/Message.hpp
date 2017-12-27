//
// Created by hjed on 27/05/17.
//

#ifndef NUMBAT_EMBEDDED_MESSAGE_HPP
#define NUMBAT_EMBEDDED_MESSAGE_HPP

#include "ros.hpp"
#include "Message_Descriptor.hpp"

namespace ros_echronos {

    /**
     * Used to derserialise messages. Stores references so we can write dynamic sized variables
     */
    typedef struct _field_store {
        /**
         * Stores a pointer to the start of the field. Is uint8_t so we can write bytes to it.
         */
        uint8_t * start_of_field;
        /**
         * The size of the field we are working on
         */
        size_t size;
        /**
         * If this field is variable length (so we need to store the size before we start)
         */
        bool is_var_length;
    } _Field_Store;

    class Message {
        public:

            /**
             * Returns the next block of can messages.
             *
             * If is_done() == true this should restart at the start and set is_done to false
             *
             * @param has_next if there is another can message left
             *
             * @param bytes how many bytes this message is
             *
             * @return the array of bytes as a reference to the Message class's stack.
             */
            uint8_t * get_next_block(bool & has_next, uint8_t & bytes);

            /**
             * Get the number of can messages this message is made up of
             * @return the size of the message
             */
            uint16_t message_size();

            /**
             * Have all the parts of this message been transmitted
             * @return
             */
            bool is_done();

            /**
             * Function to generate a block
             */
            virtual inline void generate_block() = 0;

            /**
             * Function to fill data from a can msg
             * @param msg the msg to fill from
             */
            void fill(ros_echronos::can::CAN_ROS_Message & msg);

            /**
             * Getter function to determine which can msg this message is waiting for
             */
            unsigned int get_next_msg_index();
            // needed because inheritance breaks the linker if there is not constructor
            Message();

            Message &operator = (const Message & message);

            ~Message();

            /**
             * Indicates the node this message is from. Only valid for incoming messages.
             */
            uint8_t from_node;

            /**
             * The index for the next message to decode
             */
            uint8_t decode_index;
        protected:
            /**
             * If the message has been generated
             */
            bool block_generated = false;
            /**
             * The current index of the block
             */
            volatile uint16_t offset = 0;
            /**
             * All of the data blocks
             */
            uint8_t * block = NULL;
            /**
             * The size of the block
             */
            uint8_t size = 0;
            /**
             * If we are done
             */
            bool done = false;
            /**
             * The next message we are waiting for, 0 if we are an outgoing message or have recived no data
             */
             uint32_t next_message_index = 0;

            /**
             * Overiden by subclasses to provide a means of deserialising fields
             * @return the `Message_Descriptor` class needed
             */
            virtual Message_Descriptor * generate_descriptor() = 0;
        private:
            /**
             * Represents the current message descriptor, if needed
             */
            Message_Descriptor * desc = NULL;

    } __attribute__((aligned(16)));
}


#endif //NUMBAT_EMBEDDED_MESSAGE_HPP
