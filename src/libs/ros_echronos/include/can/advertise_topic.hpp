/*
 * @date: 3/07/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Header for messages used by for control function 4 (advertise). Since functions 2 and 4 are very similar
 * this depends on function 2 in some cases
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "include/can.hpp"
#include "include/can/subscribe_topic.hpp"

#ifndef PROJECT_ADVERTISE_TOPIC_H
#define PROJECT_ADVERTISE_TOPIC_H
namespace ros_echronos {
    namespace can {
        namespace control_4_advertise {
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
             * The subscribe and advertise headers are identical.
             * Represents a header message for advertising
             */
            typedef control_2_subscribe::Subscribe_Header Advertise_Header;

            /**
             * Ctrl message specific fields that are common
             */
            constexpr CAN_Header _adv_ctrl_fields = {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        ((unsigned int)ADVERTISE_TOPIC), 0
                    }
                }
            };

            /**
             * Merged CAN Header
             */
            const CAN_Header ADV_CTRL_HEADER {
                .bits = CAN_CTRL_BASE_FIELDS.bits | _adv_ctrl_fields.bits
            };

            /**
             * Mask for the subscriber ctrl header
             */
            constexpr Subscribe_Header ADV_CTRL_HEADER_MASK {
                .fields = {
                    0xFu, 0xFu, 0xFu, 0u, 0u
                }
            };

            typedef control_2_subscribe::Response_Body Response_Body;

            inline uint8_t send_string(can::CAN_ROS_Message & msg, Advertise_Header & msg_head, char  * const  start_str_ptr, const uint32_t str_len, const uint32_t index_offset) {
                return control_2_subscribe::send_string(msg, msg_head, start_str_ptr, str_len, index_offset);
            }
        }
    }
}
#endif //PROJECT_ADVERTISE_TOPIC_H
