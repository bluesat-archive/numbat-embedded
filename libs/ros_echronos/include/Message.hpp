//
// Created by hjed on 27/05/17.
//

#ifndef NUMBAT_EMBEDDED_MESSAGE_HPP
#define NUMBAT_EMBEDDED_MESSAGE_HPP

#include "ros.hpp"
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

        protected:
            // needed because inheritance breaks the linker if there is not constructor
            Message();

            Message &operator = (const Message & message);
            /**
             * If the message has been generated
             */
            bool block_generated = false;
            /**
             * The current index of the block
             */
            uint16_t offset = 0;
            /**
             * All of the data blocks
             */
            uint8_t * block;
            /**
             * The size of the block
             */
            uint8_t size = 0;
            /**
             * If we are done
             */
            bool done = false;

    } __attribute__((aligned(16)));
}


#endif //NUMBAT_EMBEDDED_MESSAGE_HPP
