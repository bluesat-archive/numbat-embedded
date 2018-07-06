/*
 * @date: 13/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: helper functions for can
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include "include/can.hpp"

uint8_t ros_echronos::can::hash(char const * name) {
    constexpr uint8_t PRIME_HASH = 97;
    uint8_t hash = 31;
    // 8bit varient of FNV Hash that does not have primes, etc calibrated properly
    // (the chance of collision is very low here anyway)
    // http://isthe.com/chongo/tech/comp/fnv/#public_domain
    while(name[0] != '\0') {
        hash = hash ^ *(name++);
        hash = hash * PRIME_HASH;
    }
    return hash;
}

