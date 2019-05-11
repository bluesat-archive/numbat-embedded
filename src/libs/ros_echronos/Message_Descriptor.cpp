/*
 * @date: 26/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: 
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/Message_Descriptor.hpp"

using namespace ros_echronos;

#define min(a,b) (a < b ? a : b)

Message_Descriptor::Message_Descriptor(
        void **field_ptrs,
        size_t *field_size,
        const size_t num_fields
) : Message_Descriptor(field_ptrs, field_size, num_fields, true) {}

Message_Descriptor::Message_Descriptor(
        void **field_ptrs,
        size_t *field_size,
        const size_t num_fields,
        const bool copy
) : num_fields(num_fields), mem_manage_arrays(copy) {
    if(copy) {
        this->field_ptrs = (void **) alloc::malloc(sizeof(void *) * num_fields);
        this->field_size = (size_t *) alloc::malloc(sizeof(size_t) * num_fields);
        memcpy(this->field_ptrs, field_ptrs, sizeof(void *) * num_fields);
        memcpy(this->field_size, field_size, sizeof(size_t) * num_fields);
    } else {
        this->field_ptrs = field_ptrs;
        this->field_size = field_size;
    }
}

Message_Descriptor::Message_Descriptor(const Message_Descriptor &to_copy) : num_fields(to_copy.num_fields), mem_manage_arrays(to_copy.mem_manage_arrays) {
    field_ptrs = (void **) alloc::malloc(sizeof(void*)* num_fields);
    field_size = (size_t *) alloc::malloc(sizeof(size_t)* num_fields);
    memcpy(field_ptrs, to_copy.field_ptrs, sizeof(void *) * num_fields);
    memcpy(field_size, to_copy.field_size, sizeof(size_t) * num_fields);
}

Message_Descriptor::~Message_Descriptor() {
    if(mem_manage_arrays) {
        alloc::free(field_ptrs);
        alloc::free(field_size);
    }
}

void Message_Descriptor::decode_msg(const can::CAN_ROS_Message &msg) {
    const uint8_t * curr_bdy = msg.body;
    for(int i = 0; i  < msg.body_bytes;  curr_bdy = msg.body+i) {
        uint32_t curr_field_size = field_size[field_offset];

        // if we are starting a field and the size needs to be calculated
        if(curr_field_size==0 && field_internal_offset == 0) {
            if(msg.body_bytes-i > sizeof(uint16_t)) {
                // we can't memcpy this as we are changing unsinged/signed and integer size
                field_size[field_offset] = *((uint16_t *) (curr_bdy));
                i+= sizeof(uint16_t);
                // setup the array so we can copy into it, then switch it out for the actual array ptr
                _Array * array = ((_Array *) field_ptrs[field_offset]);
                array->override_with_new_bytes(field_size[field_offset]);
                field_ptrs[field_offset] = array->get_values_ptr();
                continue;
            } else {
                // got to copy second byte
                // shift it by one byte and convert to the right type
                field_size[field_offset] = ((size_t)(*(curr_bdy))) << 8;
                decoding_len = true;
                // we have finished this message then
                break;
            }
        } else if (decoding_len) {
            //this should always be the first byte in the message
            field_size[field_offset] |= *curr_bdy;
            ++i;
            decoding_len = false;
            _Array * array = ((_Array *) field_ptrs[field_offset]);
            array->override_with_new_bytes(field_size[field_offset]);
            field_ptrs[field_offset] = array->get_values_ptr();
            continue;
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

    }
}
