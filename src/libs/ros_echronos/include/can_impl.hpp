/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides an interface for implementation specific can functions
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include "ros.hpp"
#include "Message_Buffer.hpp"

#ifndef NUMBAT_EMBEDDED_CAN_IMPL_H
#define NUMBAT_EMBEDDED_CAN_IMPL_H
namespace ros_echronos {
    namespace can {

        typedef uint32_t can_sub_id;

        /**
         * Send a Can Msg (calls the architecture specific function for sending a message)
         * @param msg the message to send
         */
        void send_can(ros_echronos::can::CAN_ROS_Message & msg);

        /**
         * Wrapper for architecture specific filter for subscribing to a CAN msg.
         *
         * @note on some platforms filtering may not be possible, in which case this method should simply receive all msgs
         * @param id_mask a mask to filter can ids with
         * @param mask_bits the bits that will be used for comparison
         *
         * @return an id that can be used to disable this filter
         */
        can_sub_id subscribe_can(uint32_t id_mask, uint32_t mask_bits);

        /**
         * Wrapper for architecture specific filter for unsubscribing from a CAN msg id filter
         *
         * @param can_sub_id the id to unsubscribes
         */
        void unsubscribe_can(can_sub_id id);

        /**
         * Sets the ctrl subscription.
         * This is used by the promise manager to update the control subscription with the current combined mask/filter
         * for all promises
         * @param id_mask the id mask
         * @param mask_bits the bits to be used for comparison
         */
        void set_ctrl_sub(uint32_t id_mask, uint32_t mask_bits);

        /**
         * Disables the ctrl subscription until `set_ctrl_sub` is called again
         */
        void clear_ctrl_sub();

        /**
         * Implementation specific lock that guarentees the code will not be
         * interupted by an incoming CAN packet
         */
        void can_receive_lock();

        /**
         * Unlocks the can lock from can_receive lock. Note that this lock will unlock fully
         * even if can_receive_lock was called twice
         */
        void can_receive_unlock();

        /**
         * Creates a subscription for channel control messages
         */
        void init_channel_ctrl_sub();

        /**
         * Incoming can msg buffer
         */
        extern _Incoming_Message_Buffer * incoming_msg_buffer;

        /**
         * The "subscription" that can be used for control messages
         */
        extern const can_sub_id CTRL_SUB_ID;
        extern const can_sub_id CHANNEL_CTRL_SUB_ID;
    }
}

/**
 * Function used to handle can interupts for ros
 */
extern "C" void ros_can_interupt_handler(void);

#endif //NUMBAT_EMBEDDED_CAN_IMPL_H
