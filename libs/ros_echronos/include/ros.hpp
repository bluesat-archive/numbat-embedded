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

        const uint8_t CAN_MESSAGE_MAX_LEN = 8;
        /**
         * Value for the ros mode on the can bus
         */
        const uint8_t ROS_CAN_MODE = 1;

        typedef enum _ROS_Function {
            FN_ROS_MESSAGE_TRANSMISSION = 0,
            FN_ROS_SERVICE = 1,
            FN_ROS_CONTROL_MSG = 2,
            FN_ROS_RESERVED = 3
        } ROS_Function;
        /**
         * Used to store the base register of the can bus (CAN0)
         */
        extern uint32_t can_base;

        typedef union can_header {
            uint32_t bits;
            struct _header_fields {
                // these MUST be ints to make the compiler place them in the same 32-bit container
                // See: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491i/Babjddhe.html
                // (Also gcc docs, but they are less clear)
                unsigned int mode : 1;
                unsigned int priority : 5;
                unsigned int ros_function : 2;
                unsigned int topic : 8;
                unsigned int message_length : 9;
                unsigned int node_id : 4;
            }__attribute__((packed)) fields;
        } CAN_Header;

        typedef struct can_ros_message {
            CAN_Header head;
            uint8_t body[CAN_MESSAGE_MAX_LEN];
            uint8_t body_bytes;
        } CAN_ROS_Message;
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
