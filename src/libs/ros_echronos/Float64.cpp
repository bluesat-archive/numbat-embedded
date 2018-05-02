#include "std_msgs/Float64.hpp"
std_msgs::Float64_::Float64_()
        : data(0.0)
{
}

std_msgs::Float64_::~Float64_() {
} //deconstructor

void std_msgs::Float64_::generate_block_impl() {
    size_t offset = 0;
    size = sizeof(data);
    block = (uint8_t *) alloc::malloc(size);
    memcpy(block+offset, &data, sizeof(data));
    offset+=sizeof(data);
} // generate_block
ros_echronos::Message_Descriptor * std_msgs::Float64_::generate_descriptor() {
    void * desc = alloc::malloc(sizeof(ros_echronos::Message_Descriptor_Fixed<1>));
    ros_echronos::Message_Descriptor_Fixed<1> * descriptor = new (desc) ros_echronos::Message_Descriptor_Fixed<1>();
    descriptor->fixed_field_ptrs[0] = &data;
    descriptor->fixed_field_sizes[0] = sizeof(data);
    return descriptor;
}
#include "Publisher.cpp"
#include "Subscriber.cpp"
template class ros_echronos::Publisher<std_msgs::Float64>;
template class ros_echronos::Subscriber<std_msgs::Float64>;
template class ros_echronos::Message_Descriptor_Fixed<1>;
template class Message_Buffer<std_msgs::Float64>;
