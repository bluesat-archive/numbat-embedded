/**
 * @date: 30/04/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * Editors:
 * Purpose: This provides the core file for implementing ros on echronos
 * This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_ROS_HPP
#define NUMBAT_EMBEDDED_ROS_HPP

namespace ros_echronos {
    namespace can {
        int can_error_flag;
    };
    template <class T> class Publisher;
    template <class T> class Subscriber;
    class Node_Handle;
};

extern "C" ros_can_int_handler(void);


#endif //NUMBAT_EMBEDDED_ROS_HPP
