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
    memcpy(&this->field_ptrs, &field_ptrs, sizeof(void *) * num_fields);
    memcpy(&this->field_size, &field_size, sizeof(void *) * num_fields);
}

Message_Descriptor::~Message_Descriptor() {
    alloc::free(field_ptrs);
    alloc::free(field_size);
}

void Message_Descriptor::decode_msg(can::CAN_ROS_Message &msg) {
    uint8_t * current_in_byte = msg.body + 0;
    for(int i = 0; i  < msg.body_bytes; ) {
        size_t curr_field_size = field_size[field_offset];

        // if we are starting a field and the size needs to be calculated
        if(curr_field_size==0 && field_internal_offset == 0) {
            //TODO: calc this
        }
        curr_field_size-=field_internal_offset;
        curr_field_size = min(curr_field_size, (msg.body_bytes - i));
        memcpy((uint8_t *)field_ptrs[i],&msg.body[i], curr_field_size);
        field_internal_offset+=curr_field_size;
        if(field_internal_offset==field_size[field_offset]) {
            ++field_offset;
            field_internal_offset = 0;
        }

    }
}
