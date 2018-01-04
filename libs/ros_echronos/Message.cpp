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
        offset += can::CAN_MESSAGE_MAX_LEN;
        ros_echronos::ROS_INFO("Offset %d\n", offset);
    } else {
        done = true;
    }

    return ret;

}

bool Message::is_done() {
    return done;
}

uint16_t Message::message_size() {
    if(!block_generated ) {
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
    desc = NULL;
}

unsigned int Message::get_next_msg_index() {
    return next_message_index;
}

void Message::fill(ros_echronos::can::CAN_ROS_Message &msg) {
    //ros_echronos::ROS_INFO("fill\n");
    // if we don't have a descriptor yet generate one.
    // NOTE: a descriptor is not copied when a message is coppied, it will always start from
    // scratch
    if(!desc) {
        desc = generate_descriptor();
        //set the from node
        from_node = msg.head.fields.node_id;
        size = msg.head.fields.message_length;
        decode_index = 0;
    }
    //ros_echronos::ROS_INFO("Decoding\n");
    desc->decode_msg(msg);
    done = decode_index == size;
    decode_index++;
    //ros_echronos::ROS_INFO("Decoding Done %d/%d seq %d\n", decode_index, size, msg.head.fields.seq_num);
    if(done) {
        ros_echronos::ROS_INFO("Done! %d\n", is_done());
        // we remove the descriptor here so we can start again
        // as we allocated the memory using tlsf rather than malloc/new
        // we have to deallocate using the deconstructor and alloc::free
        desc->~Message_Descriptor();
        alloc::free(desc);
        desc = NULL;
    }
}

Message::~Message() {
    //cleanup any message descriptors that still exist
    if(desc) {
        desc->~Message_Descriptor();
        alloc::free(desc);
        desc = NULL;
    }
}

