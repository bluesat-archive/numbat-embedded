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
                    unsigned int hash : 3;
                    unsigned int seq_num : 4;
                    /**
                     * length of the topic name in packets
                     */
                    unsigned int length : 4;
                } fields __attribute__((packed));
            } Subscribe_Header;

            /**
             * Ctrl message specific fields that are common
             */
            constexpr CAN_Header _sub_ctrl_fields = {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        ((unsigned int)SUBSCRIBE_TOPIC), 0
                    }
                }
            };

            /**
             * Merged CAN Header
             */
            const CAN_Header SUB_CTRL_HEADER {
                .bits = CAN_CTRL_BASE_FIELDS.bits | _sub_ctrl_fields.bits
            };

            /**
             * Mask for the subscriber ctrl header
             */
            constexpr Subscribe_Header SUB_CTRL_HEADER_MASK {
                .fields = {
                    0xF, 0xF, 0xF, 0
                }
            };

            typedef union _response_body {
                uint8_t bytes[CAN_MESSAGE_MAX_LEN];
                struct _fields {
                    unsigned int topic_id : 6;
                } fields __attribute__((packed));
            } Response_Body;


        }
    }
}
#endif //PROJECT_SUBSCRIBE_TOPIC_H
