/*
 * @date: 17/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors)
 * @details: Simple Promise Library for CAN Messages
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "include/can.hpp"
#include "rtos-kochab.h"

#ifndef PROJECT_CANPROMISE_H
#define PROJECT_CANPROMISE_H

namespace ros_echronos {
    namespace promise {
        /**
         * Class that manages CAN Promises
         * @tparam N the size of the promise queue
         */
        template <int N> class CANPromiseManager<N> {
            private:

        };

        class CANPromise {
            public:
                /**
                 * Function that specifies the callback function
                 * @param func the function
                 * @param data the data
                 * @return this object
                 */
                CANPromise then(bool (*func) (can::CAN_ROS_MSG &), void * data);
                /**
                 * Called if there is an error on reading the promise
                 * @param func the function to call on an error
                 * @return this object
                 */
                CANPromise on_error(void (*func) (can::CAN_ROS_MSG &));

                /**
                 * Block using the provided signal.
                 * Condition: signal is not otherwise triggered
                 * @param signal the signal to use to manage this wait
                 * @return the promise
                 */
                CANPromise wait(RtosSignalId signal);

                /**
                 * Gets the value, blocks if its not ready
                 * @return the value
                 * @see wait
                 */
                can::CAN_ROS_MSG get_value();

            private:

                /**
                 * Indicates the promise has returned
                 */
                bool ready = false;


                friend CANPromiseManager;
                /**
                 * Called by CANPromiseManager to give a promise waiting on the given
                 * mask and filter combo
                 * @param mask the header mask
                 * @param filter the header filter
                 */
                CANPromise(can::CAN_Header mask, can::CAN_Header filter);
        };
    }
}

#endif //PROJECT_CANPROMISE_H
