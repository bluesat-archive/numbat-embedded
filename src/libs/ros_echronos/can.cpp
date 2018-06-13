/*
 * @date: 13/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: helper functions for can
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "utils/uartstdio.h"
#include "include/can.hpp"

int ros_echronos::can::can_error_flag;

uint32_t  ros_echronos::can::can_base;
