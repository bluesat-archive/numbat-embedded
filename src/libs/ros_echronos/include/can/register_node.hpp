/*
 * @date: 22/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors)
 * @details: defines messages for the CAN Register Message
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/can.hpp"

#ifndef PROJECT_REGISTER_NODE_H
#define PROJECT_REGISTER_NODE_H
namespace ros_echronos {
    namespace can {
        namespace control_0_register {
            typedef union _reigster_header {
                uint32_t bits;
                struct _fields {
                    unsigned int : 12;
                    unsigned int step : 1;
                    unsigned int hash : 8;
                } fields __attribute__((packed));
            } Register_Header;

            const ROS_CAN_Header _register_base_fields = {
                    .fields = {
                            .base_fields = {
                                    ROS_CAN_MODE, 0, ROS_Function.FN_ROS_CONTROL_MSG, 0
                            }
                    }
            };

            const ROS_CAN_Header _register_ctrl_fields = {
                    .fields = {
                            .f2_ros_msg_fields = {
                                    (int) Ctrl_Function.REGISTER_NODE, 0
                            }
                    }
            };

            const ROS_CAN_Header REGISTER_BASE_FIELDS = {
                    .bits = _register_base_fields.bits || _reigster_ctrl_fields.bits
            };

            typedef union _register_response {
                uint8_t bytes[CAN_MMESSAGE_MAX_LEN];
                struct _fields {
                    unsigned int node_id : 4;
                } fields __attribute__((packed));
            } Register_Response_Body;

        }
    }
}

#endif //PROJECT_REGISTER_NODE_H
