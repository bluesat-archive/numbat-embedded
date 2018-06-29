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

            constexpr CAN_Header _register_base_fields = {
                    .fields = {
                            .base_fields = {
                                    ((unsigned int)ROS_CAN_MODE),
                                    0,
                                    ((unsigned int)FN_ROS_CONTROL_MSG),
                                    0
                            }
                    }
            };

            constexpr CAN_Header _register_ctrl_fields = {
                    .fields = {
                            .f2_ctrl_msg_fields = {
                                    ((unsigned int) REGISTER_NODE), 0
                            }
                    }
            };

            const CAN_Header REGISTER_BASE_FIELDS = {
                    .bits = _register_base_fields.bits || _register_ctrl_fields.bits
            };

            typedef union _register_response {
                uint8_t bytes[CAN_MESSAGE_MAX_LEN];
                struct _fields {
                    unsigned int node_id : 4;
                } fields __attribute__((packed));
            } Register_Response_Body;

            constexpr CAN_Header _register_header_mask_base_fields {
                .fields = {
                        .base_fields = {
                                1, 0xFFFF, 0xFFFF, 0xFFF
                        }
                }
            };

            constexpr CAN_Header _register_header_mask_f2_fields {
                    .fields = {
                            .f2_ctrl_msg_fields = {
                                    0xFFF, 0xFFF
                            }
                    }
            };

            constexpr Register_Header _register_header_mask_reg_fields {
                .fields = {
                    0xFFFF, 0xFFFF
                }
            };

            const CAN_Header REGISTER_HEADER_MASK {
                .bits = _register_header_mask_base_fields.bits
                        | _register_header_mask_f2_fields.bits
                        | _register_header_mask_reg_fields.bits
            };

        }
    }
}

#endif //PROJECT_REGISTER_NODE_H
