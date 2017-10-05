/* Auto-generated by genmsg_cpp for file /home/ros/owr_software/rover/src/owr_messages/msg/pwm.msg */
#ifndef OWR_MESSAGES_MESSAGE_PWM_H
#define OWR_MESSAGES_MESSAGE_PWM_H
#include "ros_echronos/ros.hpp"
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"


namespace owr_messages
{
template <class ContainerAllocator>
struct pwm_ {
  typedef pwm_<ContainerAllocator> Type;

  pwm_()
  : joint()
  , pwm(0)
  , targetVel(0.0)
  , currentVel(0.0)
  , currentPos(0.0)
  , targetPos(0.0)
  {
  }

  pwm_(const ContainerAllocator& _alloc)
  : joint(_alloc)
  , pwm(0)
  , targetVel(0.0)
  , currentVel(0.0)
  , currentPos(0.0)
  , targetPos(0.0)
  {
  }

  typedef char[ROS_STR_LENGTH] _joint_type;
  char[ROS_STR_LENGTH] joint;

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
}; // struct pwm
typedef  ::owr_messages::pwm_ pwm;

typedef  ::owr_messages::pwm pwmPtr;
typedef  ::owr_messages::pwm const pwmConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::owr_messages::pwm_ & v)
{
  ros::message_operations::Printer< ::owr_messages::pwm_ >::stream(s, "", v);
  return s;}

} // namespace owr_messages

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
    Printer<char[ROS_STR_LENGTH]>::stream(s, indent + "  ", v.joint);
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

#endif // OWR_MESSAGES_MESSAGE_PWM_H
