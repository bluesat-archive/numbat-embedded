//
// Created by hjed on 27/05/17.
//

#include "include/Message.hpp"

using namespace ros_echronos;

Message::Message() {}

uint8_t * Message::get_next_block(bool &has_next, uint8_t &bytes) {
    if(!block_generated || done) {
        generate_block();
        block_generated = true;
        offset = 0;
    }
    const uint16_t  diff = (size - offset);
    bytes = diff % can::CAN_MESSAGE_MAX_LEN;

    if(bytes == 0 && diff > 0) { // this is wrong!
        bytes = can::CAN_MESSAGE_MAX_LEN;
    }

    has_next = diff > can::CAN_MESSAGE_MAX_LEN;
    uint8_t * const ret = block+offset;
    if(has_next) {
        offset += can::CAN_MESSAGE_MAX_LEN;
    } else {
        done = true;
    }

    return ret;

}

bool Message::is_done() {
    return done;
}

uint16_t Message::message_size() {
    //TODO: this is pretty wrong
    return size / can::CAN_MESSAGE_MAX_LEN;
}

Message & Message::operator = (const Message & message) {
    block_generated = message.block_generated;
    offset = message.offset;
    size = message.size;
    done = message.done;
    block = message.block;
}
