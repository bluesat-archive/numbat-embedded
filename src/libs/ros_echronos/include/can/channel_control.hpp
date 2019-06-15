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
        namespace control_9_channel_control {
            typedef union _channel_control_header {
                uint32_t  bits;
                struct _fields {
                    unsigned int : HEADER_COMMON_BITS;
                    unsigned int channel_ctrl_mode : 4;
                    unsigned int node_id : 4;
                    /**
                     * Optional, currently only used for heartbeat
                     */
                    unsigned int step : 1;
                } fields __attribute__((packed));
            } Channel_Control_Header;

            typedef enum _channel_control_sub_mode {
                RESET = 0,
                HEARTBEAT = 1
            } Channel_Control_Sub_Mode;

            /**
             * Ctrl message specific field values that are common
             */
            constexpr CAN_Header CHANNEL_CTRL_CTRL_FIELDS_HEADER {
                .fields = {
                    .f2_ctrl_msg_fields = {
                        ((unsigned int)CHANNEL_CONTROL), 0u
                    }
                }
            };

        }

    }
}
#endif //PROJECT_SUBSCRIBE_TOPIC_H
