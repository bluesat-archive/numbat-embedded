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
        size_t num_fields
) : num_fields(num_fields) {
    this->field_ptrs = (void **) alloc::malloc(sizeof(void*)* num_fields);
    this->field_size = (size_t *) alloc::malloc(sizeof(size_t)* num_fields);
    memcpy(this->field_ptrs, field_ptrs, sizeof(void *) * num_fields);
    memcpy(this->field_size, field_size, sizeof(size_t) * num_fields);
}

Message_Descriptor::Message_Descriptor(const Message_Descriptor &to_copy) {
    num_fields = to_copy.num_fields;
    field_ptrs = (void **) alloc::malloc(sizeof(void*)* num_fields);
    field_size = (size_t *) alloc::malloc(sizeof(size_t)* num_fields);
    memcpy(field_ptrs, to_copy.field_ptrs, sizeof(void *) * num_fields);
    memcpy(field_size, to_copy.field_size, sizeof(size_t) * num_fields);
}

Message_Descriptor::~Message_Descriptor() {
    alloc::free(field_ptrs);
    alloc::free(field_size);
}

void Message_Descriptor::decode_msg(can::CAN_ROS_Message &msg) {
    for(int i = 0; i  < msg.body_bytes; ) {
        size_t curr_field_size = field_size[field_offset];

        // if we are starting a field and the size needs to be calculated
        if(curr_field_size==0 && field_internal_offset == 0) {
            if(msg.body_bytes-i > sizeof(uint16_t)) {
                // we can't memcpy this as we are changing unsinged/signed and integer size
                field_size[field_offset] = *((uint16_t *) (msg.body + i));
                i+= sizeof(uint16_t);
                // setup the array so we can copy into it, then switch it out for the actual array ptr
                _Array * array = ((_Array *) field_ptrs[field_internal_offset]);
                array->override_with_new_size(field_size[field_offset]);
                field_ptrs[field_offset] = array->get_values_ptr();
                continue;
            } else {
                // got to copy second byte
                // shift it by one byte and convert to the right type
                field_size[field_offset] = ((size_t)(*((uint8_t *) (msg.body + i)))) << 8;
                decoding_len = true;
                // we have finished this message then
                break;
            }
        } else if (decoding_len) {
            //this should always be the first byte in the message
            field_size[field_offset] |= msg.body[i];
            ++i;
            continue;
        }
        curr_field_size-=field_internal_offset;
        //ros_echronos::ROS_INFO("cfs %d, body_bytes %d bb-i\n", curr_field_size, msg.body_bytes, msg.body_bytes-i);
        curr_field_size = min(curr_field_size, (msg.body_bytes - i));
        //ros_echronos::ROS_INFO("Copying %d bytes from %p to %p\n", curr_field_size, msg.body + i, field_ptrs[field_offset]);
        memcpy(
                (uint8_t*)field_ptrs[field_offset]+field_internal_offset,
                msg.body + i,
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
