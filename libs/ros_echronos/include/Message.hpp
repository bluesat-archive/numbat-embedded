//
// Created by hjed on 27/05/17.
//

#ifndef NUMBAT_EMBEDDED_MESSAGE_HPP
#define NUMBAT_EMBEDDED_MESSAGE_HPP

#include "ros.hpp"

namespace ros_echronos {
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
            virtual inline uint16_t message_size() = 0;

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
            Array<uint8_t> block;
            /**
             * If we are done
             */
            bool done = false;

    };
}


#endif //NUMBAT_EMBEDDED_MESSAGE_HPP
