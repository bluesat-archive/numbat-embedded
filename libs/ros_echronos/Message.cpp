//
// Created by hjed on 27/05/17.
//

#include "include/Message.hpp"

using namespace ros_echronos;

uint8_t * Message::get_next_block(bool &has_next, uint8_t &bytes) {
    if(!block_generated || done) {
        generate_block();
        block_generated = true;
        offset = 0;
    }
    const uint16_t  diff = (block.size - offset);
    bytes = diff % can::CAN_MESSAGE_MAX_LEN;

    if(bytes == 0 && diff > 0) {
        bytes = can::CAN_MESSAGE_MAX_LEN;
    }

    has_next = diff > can::CAN_MESSAGE_MAX_LEN;
    if(has_next) {
        offset += can::CAN_MESSAGE_MAX_LEN;
    } else {
        done = true;
    }

    return block.values+offset;

}

bool Message::is_done() {
    return done;
}