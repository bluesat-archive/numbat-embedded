/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides an interface for implementation specific can functions
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include "ros.hpp"

#ifndef NUMBAT_EMBEDDED_CAN_IMPL_H
#define NUMBAT_EMBEDDED_CAN_IMPL_H
namespace ros_echronos {
    namespace can {
        void send_can(ros_echronos::can::CAN_ROS_Message & msg);
    }
}

#endif //NUMBAT_EMBEDDED_CAN_IMPL_H
