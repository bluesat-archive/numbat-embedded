/*
 * @date: 22/08/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: Used to mirror the functionality provided by ros::Time and ros::Duration
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include <stdint.h>
#include <climits>

#ifndef PROJECT_TIME_HPP
#define PROJECT_TIME_HPP

namespace ros_echronos {

    /**
     * Represents a ros::Duration object
     */
    typedef struct Duration {
        uint64_t seconds;
        uint64_t nanos;
    } __attribute__((packed));

    /**
     * Represents a ros::Time object
     */
    struct Time {
        uint64_t seconds;
        uint64_t nanos;
    } __attribute__((packed));

    /**
     * Represents that the controller should
     */
    constexpr Time CTRL_NOW = {
        ULONG_MAX, ULONG_MAX
    };

}

#endif //PROJECT_TIME_HPP
