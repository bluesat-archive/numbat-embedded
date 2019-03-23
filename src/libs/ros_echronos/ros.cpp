/*
 * Date Started: 30/04/17
 * Original Author: Harry J.E Day <harry@dayfamilyweb.com>
 * Editors:
 * Purpose: This provides the core file for implementing ros on echronos
 * This code is released under the AGPLv3 License.
 * Copyright BLUEsat UNSW, 2017 - 2018
 */

#include "ros.hpp"

RtosMutexId ros_echronos::write_mutex = 0;
bool ros_echronos::write_mutex_set = false;
