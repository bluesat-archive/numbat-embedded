/* Auto-generated by genmsg_cpp for file /home/hjed/Bluesat/owr_software/rover/src/owr_messages/msg/pwm.msg */
#ifndef OWR_MESSAGES_MESSAGE_PWM_H
#define OWR_MESSAGES_MESSAGE_PWM_H
#include "ros.hpp"
#include "Message.hpp"

namespace owr_messages
{
class pwm_ : public ros_echronos::Message {
  typedef pwm_ Type;

  public:
  pwm_();
  pwm_(const owr_messages::pwm_& copy); 
  ~pwm_();
  virtual void generate_block();
  typedef ros_echronos::String _joint_type;
  ros_echronos::String joint;

  typedef int32_t _pwm_type;
  int32_t pwm;

  typedef double _targetVel_type;
  double targetVel;

  typedef double _currentVel_type;
  double currentVel;

  typedef double _currentPos_type;
  double currentPos;

  typedef double _targetPos_type;
  double targetPos;


  typedef  ::owr_messages::pwm_  * Ptr;
  typedef  ::owr_messages::pwm_  * const ConstPtr;
} __attribute__((aligned (4)));; // class pwm
typedef  ::owr_messages::pwm_ pwm;

typedef  ::owr_messages::pwm pwmPtr;
typedef  ::owr_messages::pwm const pwmConstPtr;

 owr_messages::pwm_::pwm_()
  : joint()
  , pwm(0)
  , targetVel(0.0)
  , currentVel(0.0)
  , currentPos(0.0)
  , targetPos(0.0)
  {
  }

  owr_messages::pwm_::pwm_(const owr_messages::pwm_& copy) : 
  pwm(copy.pwm),  targetVel(copy.targetVel),  currentVel(copy.currentVel),  currentPos(copy.currentPos),  targetPos(copy.targetPos)
  {
      memcpy(joint, copy.joint, ROS_STR_LEN);
  } // copy constructor

  owr_messages::pwm_::~pwm_() {
      if (block) { alloc::free(block); }
  } //deconstructor

  void owr_messages::pwm_::generate_block() {
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

} // namespace owr_messages
/*
namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::owr_messages::pwm_ > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::owr_messages::pwm_  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::owr_messages::pwm_ > {
  static const char* value() 
  {
    return "bb3ea5f1085fb8cc6952e03e3d0af6e9";
  }

  static const char* value(const  ::owr_messages::pwm_ &) { return value(); } 
  static const uint64_t static_value1 = 0xbb3ea5f1085fb8ccULL;
  static const uint64_t static_value2 = 0x6952e03e3d0af6e9ULL;
};

template<class ContainerAllocator>
struct DataType< ::owr_messages::pwm_ > {
  static const char* value() 
  {
    return "owr_messages/pwm";
  }

  static const char* value(const  ::owr_messages::pwm_ &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::owr_messages::pwm_ > {
  static const char* value() 
  {
    return "string joint\n\
int32 pwm\n\
float64 targetVel\n\
float64 currentVel\n\
float64 currentPos\n\
float64 targetPos\n\
";
  }

  static const char* value(const  ::owr_messages::pwm_ &) { return value(); } 
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::owr_messages::pwm_ >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.joint);
    stream.next(m.pwm);
    stream.next(m.targetVel);
    stream.next(m.currentVel);
    stream.next(m.currentPos);
    stream.next(m.targetPos);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER
}; // struct pwm_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::owr_messages::pwm_ >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::owr_messages::pwm_ & v) 
  {
    s << indent << "joint: ";
    Printer<ros_echronos::String>::stream(s, indent + "  ", v.joint);
    s << indent << "pwm: ";
    Printer<int32_t>::stream(s, indent + "  ", v.pwm);
    s << indent << "targetVel: ";
    Printer<double>::stream(s, indent + "  ", v.targetVel);
    s << indent << "currentVel: ";
    Printer<double>::stream(s, indent + "  ", v.currentVel);
    s << indent << "currentPos: ";
    Printer<double>::stream(s, indent + "  ", v.currentPos);
    s << indent << "targetPos: ";
    Printer<double>::stream(s, indent + "  ", v.targetPos);
  }
};


} // namespace message_operations
} // namespace ros
*/

//#include "/home/hjed/Bluesat/numbat-embedded/libs/ros_echronos/Publisher.cpp"
#include "/media/spare/numbat-embedded/libs/ros_echronos/Publisher.cpp"
template class ros_echronos::Publisher<owr_messages::pwm_>;
#endif // OWR_MESSAGES_MESSAGE_PWM_H

