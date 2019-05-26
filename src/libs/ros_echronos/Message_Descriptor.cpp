/*
 * @date: 26/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: 
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/Message_Descriptor.hpp"
#include <new>
#include <Message.hpp>

using namespace ros_echronos;

#define min(a,b) (a < b ? a : b)


Message_Descriptor::Message_Descriptor(void **field_ptrs, size_t *field_size, Message_Descriptor **sub_descriptor_ptrs,
                                       size_t num_fields, const bool copy)
    : num_fields(num_fields), mem_manage_arrays(copy), is_cloned(copy) {
    if(copy) {
        this->field_ptrs = (void **) alloc::malloc(sizeof(void *) * num_fields);
        this->field_size = (size_t *) alloc::malloc(sizeof(size_t) * num_fields);
        this->sub_descriptors = (Message_Descriptor **) alloc::malloc(sizeof(Message_Descriptor *) * num_fields);
        memcpy(this->field_ptrs, field_ptrs, sizeof(void *) * num_fields);
        memcpy(this->field_size, field_size, sizeof(size_t) * num_fields);
        for(int i = 0; i < num_fields; ++i) {
            if(sub_descriptor_ptrs[i]) {
                sub_descriptors[i]  = sub_descriptor_ptrs[i]->clone();
            } else {
                // ptr we are copying is null so we can be too
                this->sub_descriptors = NULL;
            }
        }
    } else {
        this->field_ptrs = field_ptrs;
        this->field_size = field_size;
        this->sub_descriptors = sub_descriptor_ptrs;
    }
}

Message_Descriptor::Message_Descriptor(const Message_Descriptor &to_copy) :
    Message_Descriptor(to_copy.field_ptrs, to_copy.field_size, to_copy.sub_descriptors, to_copy.num_fields, true){
}

Message_Descriptor::~Message_Descriptor() {
    if(is_cloned) {
        for(Message_Descriptor ** i = sub_descriptors; i < sub_descriptors+num_fields; ++i) {
            if(i[0]) {
                alloc::free(i[0]);
            }
        }
    }
    if(mem_manage_arrays) {
        alloc::free(field_ptrs);
        alloc::free(field_size);
        alloc::free(sub_descriptors);
    }
}

void Message_Descriptor::decode_msg(const can::CAN_ROS_Message &msg) {
    const uint8_t * curr_bdy = msg.body;
    for(int i = 0; i  < msg.body_bytes;  curr_bdy = msg.body+i) {
        const bool should_break = decode_msg_inner_loop(i, curr_bdy, msg);
        if(should_break) {
            break;
        }
    }
}

constexpr int OFFSET_SUB_MESSAGE_START = 0;
constexpr int OFFSET_SUB_MESSAGE_CONT = 1;

bool Message_Descriptor::decode_msg_inner_loop(int & i, const uint8_t *  &curr_bdy, const can::CAN_ROS_Message & msg) {
    uint32_t curr_field_size = field_size[field_offset];


    // if we are starting a field and the size needs to be calculated
    if(sub_descriptors[field_offset]) {
        Message * const sub_message = (Message *)field_ptrs[field_offset];
        if(field_internal_offset == OFFSET_SUB_MESSAGE_START) {
            // populate the references in the descriptor
            sub_message->populate_descriptor(sub_descriptors[field_offset]);
            field_internal_offset = OFFSET_SUB_MESSAGE_CONT;

        }
        const bool should_break = sub_descriptors[field_offset]->decode_msg_inner_loop(i, curr_bdy, msg);

        const bool sub_message_done = sub_message->record_fill();
        if(sub_message_done) {
            field_internal_offset = OFFSET_SUB_MESSAGE_START;
            ++field_offset;
        }
        return should_break;
    }

    if(curr_field_size==0 && field_internal_offset == 0) {
        if(msg.body_bytes-i >= sizeof(uint16_t)) {
            // we can't memcpy this as we are changing unsinged/signed and integer size
            field_size[field_offset] = *((uint16_t *) (curr_bdy));
            i+= sizeof(uint16_t);
            // setup the array so we can copy into it, then switch it out for the actual array ptr
            _Array * array = ((_Array *) field_ptrs[field_offset]);
            array->override_with_new_bytes(field_size[field_offset]);
            field_ptrs[field_offset] = array->get_values_ptr();
            // continue
            return false;
        } else {
            // copy least significant byte
            field_size[field_offset] = *curr_bdy;
            decoding_len = true;
            // we have finished this message then
            //break
            return true;
        }
    } else if (decoding_len) {
        // this should always be the first byte in the message
        // copy most significant byte
        field_size[field_offset] |= (*curr_bdy) << 8;
        ++i;
        decoding_len = false;
        _Array * array = ((_Array *) field_ptrs[field_offset]);
        array->override_with_new_bytes(field_size[field_offset]);
        field_ptrs[field_offset] = array->get_values_ptr();
        // continue
        return false;
    }
    curr_field_size-=field_internal_offset;
    //ros_echronos::ROS_INFO("cfs %d, body_bytes %d bb-i\n", curr_field_size, msg.body_bytes, msg.body_bytes-i);
    curr_field_size = min(curr_field_size, (msg.body_bytes - i));
    //ros_echronos::ROS_INFO("Copying %d bytes from %p to %p\n", curr_field_size, msg.body + i, field_ptrs[field_offset]);
    memcpy(
        (uint8_t*)field_ptrs[field_offset]+field_internal_offset,
        curr_bdy,
        curr_field_size
    );
    i+=curr_field_size;
    field_internal_offset+=curr_field_size;
    if(field_internal_offset==field_size[field_offset]) {
        ++field_offset;
        field_internal_offset = 0;
    }
    return false;

}

Message_Descriptor *Message_Descriptor::clone() {
    return new(alloc::malloc(sizeof(*this))) Message_Descriptor(*this);
}
