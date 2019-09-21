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
            typedef union _advertise_header {
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
            } Advertise_Header;

            /**
             * Ctrl message specific fields that are common
             */
            constexpr CAN_Header adv_ctrl_fields = {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        0x0, ((unsigned int)ADVERTISE_TOPIC), 0
                    }
                }
            };

            /**
             * Merged CAN Header
             */
            const CAN_Header ADV_CTRL_HEADER {
                .bits = CAN_CTRL_BASE_FIELDS.bits | adv_ctrl_fields.bits
            };

            /**
             * Mask for the publisher ctrl header
             */
            constexpr Advertise_Header ADV_CTRL_HEADER_MASK {
                .fields = {
                    0xF, 0xF, 0xF, 0
                }
            };

            typedef control_2_subscribe::Response_Body Response_Body;

            constexpr CAN_Header add_common_headers(const Advertise_Header & advertise_header) {
                return {
                        .bits = CAN_CTRL_BASE_FIELDS.bits | advertise_header.bits | adv_ctrl_fields.bits
                    };
            }

            inline uint8_t send_string(can::CAN_ROS_Message & msg, Advertise_Header & msg_head, char  * const  start_str_ptr, const uint32_t str_len, const uint32_t index_offset) {
                uint8_t index = 0;
                const char * const end_ptr = start_str_ptr+str_len;
                for(const char * str_ptr = start_str_ptr; str_ptr < end_ptr; ++str_ptr) {
                    const uint32_t ptr_offset = (str_ptr - start_str_ptr) + index_offset;
                    index = ptr_offset % (CAN_MESSAGE_MAX_LEN);
                    msg.body[index] = *str_ptr;
                    if(index == (CAN_MESSAGE_MAX_LEN-1)) {
                        msg.body_bytes = CAN_MESSAGE_MAX_LEN;
                        send_can(msg);
                        ++(msg_head.fields.seq_num);
                        msg.head = add_common_headers(msg_head);
                        memset(msg.body, 0, CAN_MESSAGE_MAX_LEN);
                    }
                }
                return index;
            }
        }
    }
}
#endif //PROJECT_ADVERTISE_TOPIC_H
