#include "owr_messages/pwm.hpp"
 owr_messages::pwm_::pwm_()
  : joint()
  , pwm(0)
  , targetVel(0.0)
  , currentVel(0.0)
  , currentPos(0.0)
  , targetPos(0.0)
  {
  }

  owr_messages::pwm_::~pwm_() {
  } //deconstructor

  void owr_messages::pwm_::generate_block_impl() {
      size_t offset = 0;
      size = sizeof(joint)+sizeof(pwm)+sizeof(targetVel)+sizeof(currentVel)+sizeof(currentPos)+sizeof(targetPos);
      block = (uint8_t *) alloc::malloc(size);
      memcpy(block+offset, &joint, sizeof(joint));
      offset+=sizeof(joint);
      memcpy(block+offset, &pwm, sizeof(pwm));
      offset+=sizeof(pwm);
      memcpy(block+offset, &targetVel, sizeof(targetVel));
      offset+=sizeof(targetVel);
      memcpy(block+offset, &currentVel, sizeof(currentVel));
      offset+=sizeof(currentVel);
      memcpy(block+offset, &currentPos, sizeof(currentPos));
      offset+=sizeof(currentPos);
      memcpy(block+offset, &targetPos, sizeof(targetPos));
      offset+=sizeof(targetPos);
  } // generate_block
  ros_echronos::Message_Descriptor * owr_messages::pwm_::generate_descriptor() {
    void * desc = alloc::malloc(sizeof(ros_echronos::Message_Descriptor_Fixed<6, ros_echronos::Tuple>));
    ros_echronos::Message_Descriptor_Fixed<6, ros_echronos::Tuple> * descriptor = new (desc) ros_echronos::Message_Descriptor_Fixed<6, ros_echronos::Tuple>();
    descriptor->fixed_field_ptrs[0] = &joint;
    descriptor->fixed_field_sizes[0] = sizeof(joint);
    descriptor->fixed_field_ptrs[1] = &pwm;
    descriptor->fixed_field_sizes[1] = sizeof(pwm);
    descriptor->fixed_field_ptrs[2] = &targetVel;
    descriptor->fixed_field_sizes[2] = sizeof(targetVel);
    descriptor->fixed_field_ptrs[3] = &currentVel;
    descriptor->fixed_field_sizes[3] = sizeof(currentVel);
    descriptor->fixed_field_ptrs[4] = &currentPos;
    descriptor->fixed_field_sizes[4] = sizeof(currentPos);
    descriptor->fixed_field_ptrs[5] = &targetPos;
    descriptor->fixed_field_sizes[5] = sizeof(targetPos);
    return descriptor;
  }
#include "templates/Publisher.cpp"
#include "templates/Subscriber.cpp"
template class ros_echronos::Publisher<owr_messages::pwm>;
template class ros_echronos::Subscriber<owr_messages::pwm>;

