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

#include "rtos-kochab.h"
#define ROS_STR_LEN 15

namespace ros_echronos {
    namespace can {
        extern int can_error_flag;

        /**
         * Used to store the base register of the can bus (CAN0)
         */
        extern uint32_t can_base;
    }
    template <class T> class Publisher;
    template <class T> class Subscriber;
    class NodeHandle;

    template <typename T> struct _Array {
        T * values;
        size_t size;
        T operator[] (int index) {
            return values[index];
        }
    };

    template <typename T> using Array = struct _Array<T>;

    typedef char String[ROS_STR_LEN];

}

extern "C" void ros_can_int_handler(void);


#endif //NUMBAT_EMBEDDED_ROS_HPP
