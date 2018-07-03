/*
 * @date: 3/07/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Header for messages used by for control function 2 (subscribe)
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "include/can.hpp"

#ifndef PROJECT_SUBSCRIBE_TOPIC_H
#define PROJECT_SUBSCRIBE_TOPIC_H
namespace ros_echronos {
    namespace can {
        namespace control_2_subscribe {
            typedef union _subscribe_header {
                uint32_t  bits;
                struct _fields {
                    unsigned int : HEADER_COMMON_BITS;
                    unsigned int node_id : 4;
                    unsigned int step : 1;
                    unsigned int hash : 8;
                    /**
                     * length of the topic name in packets
                     */
                    unsigned int length : 3;
                } fields __attribute__((packed));
            };

            /**
             * Base fields that are set for all ctrl_2 messages
             */
            constexpr CAN_Header _sub_base_fields = {
                .fields = {
                    .base_fields = {
                        ((unsigned int)ROS_CAN_MODE),
                        0,
                        ((unsigned int)FN_ROS_CONTROL_MSG),
                        0
                    }
                }
            };

            /**
             * Ctrl message specific fields that are common
             */
            constexpr CAN_Header _sub_ctrL_fields = {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        ((unsigned int)SUBSCRIBE_TOPIC), 0
                    }
                }
            };


        }
    }
}
#endif //PROJECT_SUBSCRIBE_TOPIC_H
