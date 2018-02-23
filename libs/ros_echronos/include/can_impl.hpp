/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides an interface for implementation specific can functions
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include "ros.hpp"

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


    }
}

/**
 * Function used to handle can interupts for ros
 */
extern "C" void ros_can_interupt_handler(void);

#endif //NUMBAT_EMBEDDED_CAN_IMPL_H
