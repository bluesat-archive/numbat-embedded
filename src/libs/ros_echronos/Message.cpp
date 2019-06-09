//
// Created by hjed on 27/05/17.
//

#include "include/Message.hpp"
#include "boilerplate.h"
#include "ros.hpp"

using namespace ros_echronos;

//NOTE: this makes the system to slow to read can messages in sequence
//#define DEBUG_MESSAGE_INTERNAL

Message::Message() {

    //ros_echronos::ROS_INFO("constructor block %p \n", block);
}

uint8_t * Message::get_next_block(bool &has_next, uint8_t &bytes) {
    if(!block_generated || done) {
        generate_block();
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
#ifdef DEBUG_MESSAGE_INTERNAL
        ros_echronos::ROS_INFO("Offset %d\n", offset);
#endif
    } else {
        done = true;
    }

    return ret;

}

uint16_t Message::message_size() {
    if(!block_generated) {
        // we don't know the size in messages until we have generated the block
        generate_block();
    }
    uint8_t floored = size / can::CAN_MESSAGE_MAX_LEN;
    return size % can::CAN_MESSAGE_MAX_LEN == 0 ? floored : floored + 1;
}

Message::Message(const Message & message) {
#ifdef DEBUG_MESSAGE_INTERNAL
    ros_echronos::ROS_INFO("%p: Copy constructor block %p old block %p\n",this, block, message.block);
#endif
    block_generated = message.block_generated;
    offset = message.offset;
    size = message.size;
    done = message.done;
    if (message.block) {
        // if the block is generated the size is set
        block = (uint8_t *) alloc::malloc(size);
#ifdef DEBUG_MESSAGE_INTERNAL
        ros_echronos::ROS_INFO("block is %p\n", block);
#endif
        memcpy(block, message.block, size);
    } else {
        block = NULL;
    }
    desc = NULL;
}

unsigned int Message::get_next_msg_index() {
    return next_message_index;
}

void Message::fill(const can::CAN_ROS_Message &msg) {
    //ros_echronos::ROS_INFO("fill\n");
    // if we don't have a descriptor yet generate one.
    // NOTE: a descriptor is not copied when a message is coppied, it will always start from
    // scratch
    if(!desc) {
        desc = generate_descriptor();
        //set the from node
        from_node = msg.head.fields.f0_ros_msg_fields.node_id;
        from_msg_num = msg.head.fields.f0_ros_msg_fields.message_num;
        size = msg.head.fields.f0_ros_msg_fields.message_length;
        decode_index = 0;
    }
    //ros_echronos::ROS_INFO("Decoding\n");
    desc->decode_msg(msg);
    ++decode_index;
    done = decode_index == size;
#ifdef DEBUG_MESSAGE_DECODE
    ros_echronos::ROS_INFO("Decoding Done %d/%d seq %d\n", decode_index, size, msg.head.fields.seq_num);
#endif //DEBUG_MESSAGE_DECODE
    if(done) {
#ifdef DEBUG_MESSAGE_DECODE
        ros_echronos::ROS_INFO("Done! %d\n", is_done());
#endif //DEBUG_MESSAGE_DECODE
        // we remove the descriptor here so we can start again
        // as we allocated the memory using tlsf rather than malloc/new
        // we have to deallocate using the deconstructor and alloc::free
        desc->~Message_Descriptor();
        alloc::free(desc);
        desc = NULL;
    }
}

Message::~Message() {
#ifdef DEBUG_MESSAGE_INTERNAL
    ros_echronos::ROS_INFO("Deconstructor block %p this %p\n", block, this);
#endif
    //cleanup any message descriptors that still exist
    if(desc) {
        desc->~Message_Descriptor();
        alloc::free(desc);
        desc = NULL;
    }
    if(block) {
        alloc::free(block);
        block = NULL;
    }
}

void Message::generate_block() {
    if(block) {
        alloc::free(block);
        block = NULL;
    }
#ifdef DEBUG_MESSAGE_INTERNAL
    ros_echronos::ROS_INFO("Generate block for %p generated befor %d\n", this, block_generated);
#endif
    generate_block_impl();
    block_generated = true;
    offset = 0;
    done = false;
}

Message & Message::operator=(const Message &new_value) {

#ifdef DEBUG_MESSAGE_INTERNAL
    ros_echronos::ROS_INFO("%p: Assignment Opperator called on block %p old block %p\n",this, block, new_value.block);
#endif
    //cleanup any message descriptors that still exist
    if(desc) {
        desc->~Message_Descriptor();
        alloc::free(desc);
        desc = NULL;
    }
    if(block) {
        alloc::free(block);
        block = NULL;
    }

    block_generated = new_value.block_generated;
    offset = new_value.offset;
    size = new_value.size;
    done = new_value.done;
    from_msg_num = new_value.from_msg_num;
    from_node = new_value.from_node;
    if (new_value.block) {
        // if the block is generated the size is set
        block = (uint8_t *) alloc::malloc(size);
#ifdef DEBUG_MESSAGE_INTERNAL
        ros_echronos::ROS_INFO("block is %p\n", block);
#endif
        memcpy(block, new_value.block, size);
    } else {
        block = NULL;
    }
    desc = NULL;
}