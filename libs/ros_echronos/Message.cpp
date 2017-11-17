//
// Created by hjed on 27/05/17.
//

#include "include/Message.hpp"
#include "boilerplate.h"
#include "ros.hpp"

using namespace ros_echronos;

Message::Message() {}

uint8_t * Message::get_next_block(bool &has_next, uint8_t &bytes) {
    if(!block_generated || done) {
        generate_block();
        block_generated = true;
        offset = 0;
        done = false;
    }
    const uint16_t  diff = (size - offset);

    // do we have a full block left or a partial one?
    if(diff > can::CAN_MESSAGE_MAX_LEN) {
        bytes = can::CAN_MESSAGE_MAX_LEN;
        has_next = true;
    } else {
        bytes = diff;
        has_next = false;
    }

    uint8_t * const ret = block+offset;
    if(has_next) {
        // += does not work here for some reason
        offset += (uint16_t)  can::CAN_MESSAGE_MAX_LEN;
        UARTprintf("Offset %d\n", offset);
    } else {
        done = true;
    }

    return ret;

}

bool Message::is_done() {
    return done;
}

uint16_t Message::message_size() {
    if(!block_generated || done) {
        // we don't know the size in messages until we have generated the block
        generate_block();
    }
    return size / can::CAN_MESSAGE_MAX_LEN;
}

Message & Message::operator = (const Message & message) {
    block_generated = message.block_generated;
    offset = message.offset;
    size = message.size;
    done = message.done;
    block = message.block;
}
