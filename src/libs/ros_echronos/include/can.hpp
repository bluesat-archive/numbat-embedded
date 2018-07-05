/*
 * @date: 13/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Header for can functionality
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "boilerplate.h"
#include "rtos-kochab.h"

#ifndef PROJECT_CAN_H
#define PROJECT_CAN_H

namespace ros_echronos {
/**
 * Namespace for can specific functionality
 */
    namespace can {
        /**
         * the number of bits to skip when making custom control headers
         */
        constexpr uint8_t HEADER_COMMON_BITS = 12;
        extern int can_error_flag;

        /**
         * The maximum length of a can message
         */
        constexpr uint8_t CAN_MESSAGE_MAX_LEN = 8;

        /**
         * Value for the ros mode on the can bus
         */
        constexpr uint8_t ROS_CAN_MODE = 1;
        /**
         * Indicates that the "seq_num" field of a can message header is in special mode
         */
        constexpr uint8_t SEQ_NUM_SPECIAL_MODE = 7;

        /**
         * Possible function modes on the can buss
         */
        typedef enum _ROS_Function {
            /**
             * Means the can message is part of a message transmission
             */
                FN_ROS_MESSAGE_TRANSMISSION = 0,
            /**
             * Means the can message is part of a service call (Not implemented)
             */
                FN_ROS_SERVICE = 1,
            /**
             * Means the can message is a control message
             */
                FN_ROS_CONTROL_MSG = 2,
            /**
             * Undefined functionality, reserved for future expansion of the protocol
             */
                FN_ROS_RESERVED = 3
        } ROS_Function;

        /**
         * Possible Control Functions
         */
         typedef enum _ctrl_function {
                REGISTER_NODE = 0,
                DEREGISTER_NODE = 1,
                SUBSCRIBE_TOPIC = 2,
                UNSUBSCRIBE_TOPIC = 3,
                ADVERTISE_TOPIC = 4,
                DEREGISTER_ADVERTISED_TOPIC = 5,
                ADVERTISE_SERVICE = 6,
                DEREGISTER_SERIVCE = 7,
                MANAGE_PARAMETERS = 8,
                HEARTBEAT = 9,
                EXTENDED = 10
         } Ctrl_Function;
        /**
         * Used to store the base register of the can bus (CAN0)
         */
        extern uint32_t can_base;

        /**
         * Represents a can header as per the ros over can protocol
         */
        typedef union can_header {
            unsigned long bits;
            union _header_fields {
                // these MUST be ints to make the compiler place them in the same 32-bit container
                // See: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491i/Babjddhe.html
                // (Also gcc docs, but they are less clear)
                struct _base_fields {
                    unsigned int mode : 1;
                    unsigned int priority : 2;
                    unsigned int ros_function : 2;
                    unsigned int seq_num : 3;
                } __attribute__((packed)) base_fields;
                /**
                 * Header fields for ROS msgs
                 */
                struct _ros_msg_fields {
                    unsigned int : sizeof(base_fields);
                    unsigned int message_num : 2;
                    unsigned int topic : 7;
                    unsigned int message_length : 8;
                    unsigned int node_id : 4;
                    unsigned int not_in_range : 3;
                } __attribute__((packed)) f0_ros_msg_fields;
                /**
                 * Header fields for control msgs
                 */
                struct _ros_ctrl_fields {
                    unsigned int : sizeof(base_fields);
                    unsigned int mode : 4;
                    unsigned int control_specific : 28 - sizeof(base_fields) - 4;
                } __attribute__((packed)) f2_ctrl_msg_fields;
            } __attribute__((packed)) fields;
        } CAN_Header;

        /**
         * Represents a can packet.
         */
        typedef struct can_ros_message {
            CAN_Header head;
            uint8_t body[CAN_MESSAGE_MAX_LEN];
            uint8_t body_bytes;
        } CAN_ROS_Message __attribute__((aligned(4)));


        /**
         * We can't use mutexes in buffers so we have to assume that we do not receive another message
         * before this is written. I eventually want to replace this with a lockless queue.
         */
        typedef struct _input_buffer {
            CAN_ROS_Message buffer;
            int start_counter = 0;
            int end_counter = 0;
        } input_buffer_t;

        constexpr CAN_Header _TOPIC_BITMASK_BASE = {
            .fields = {
                .base_fields = { 1, 0, 0xF, 0},
            }
        };
        constexpr CAN_Header _TOPIC_BITMASK_F0 = {
            .fields = {
                .f0_ros_msg_fields = { 0, 0xFFFF, 0, 0, 0}
            }
        };

        const CAN_Header TOPIC_BITMASK_HEADER = {
            .bits = _TOPIC_BITMASK_BASE.bits || _TOPIC_BITMASK_BASE.bits
        };


        extern input_buffer_t input_buffer;
//        extern rigtorp::SPSCQueue<CAN_ROS_Message, 5> msg_queue;

        extern RtosInterruptEventId can_interupt_event;
        extern volatile bool node_handle_ready;

        /**
         * Hashes a null terminated string
         */
        uint8_t hash(const char * name);
    }
}
#endif //PROJECT_CAN_H
