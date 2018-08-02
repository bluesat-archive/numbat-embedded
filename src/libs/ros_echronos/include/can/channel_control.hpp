/*
 * @date: 2/08/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Header for messages used by for control function 9 - channel control. These are functions we should always listen
 * for.
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "include/can.hpp"

#ifndef PROJECT_SUBSCRIBE_TOPIC_H
#define PROJECT_SUBSCRIBE_TOPIC_H
namespace ros_echronos {
    namespace can {
        namespace control_9_channel_control {
            typedef union _channel_control_header {
                uint32_t  bits;
                struct _fields {
                    unsigned int : HEADER_COMMON_BITS;
                    unsigned int chan_ctrl_mode : 4;
                    unsigned int node_id : 4;
                    /**
                     * Optional, only used for heartbeat currently
                     */
                    unsigned int step : 1;
                } fields __attribute__((packed));
            } Channel_Control_Header;

            typedef enum _channel_control_sub_mode {
                RESET = 0,
                HEARTBEAT = 1
            } Channel_Control_Sub_Mode;

            /**
             * Ctrl message specific fields that are common
             */
            constexpr CAN_Header _chan_ctrl_ctrl_fields = {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        ((unsigned int)CHANNEL_CONTROL), 0
                    }
                }
            };

            /**
             * Merged CAN Header
             */
            const CAN_Header SUB_CTRL_HEADER {
                .bits = CAN_CTRL_BASE_FIELDS.bits | _chan_ctrl_ctrl_fields.bits
            };

            /**
             * Mask for the channel_controlr ctrl header
             */
            constexpr Subscribe_Header SUB_CTRL_HEADER_MASK {
                .fields = {
                    0xF, 0xF, 0xF, 0
                }
            };

        }

    }
}
#endif //PROJECT_SUBSCRIBE_TOPIC_H
