/*
 * @date: 17/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors)
 * @details: Simple Promise Library for CAN Messages
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include <string.h>

#include "include/CANPromise.hpp"

using namespace ros_echronos::promise;

CANPromise::CANPromise(can::CAN_Header mask, can::CAN_Header filter) :
    mask(mask),
    filter(filter)
{

}

CANPromise * CANPromise::then(PromiseFn func, void * data) {
    then_fn = func;
    // C++ doesn't have inbuilt currying
    then_data = data;
    return this;
}

CANPromise * CANPromise::on_error(PromiseFn func, void *data) {
    error_fn = func;
    error_data = data;
    return this;
}

ros_echronos::can::CAN_ROS_Message CANPromise::wait(RtosSignalId signal) {
    signal = signal;
    waiting = true;
    waiting_on = rtos_task_current();
    rtos_signal_wait(signal);
    if(error) {
        error_fn(msg, error_data);
    } else {
        then_fn(msg, then_data);
    }
    // we want to deconstruct and 0 this class now
    // so we can't touch any class variables once we do that
    // (Surprisingly this is valid standard C++)
    can::CAN_ROS_Message m = msg;
    void * this_ptr = this;
    this->~CANPromise();
    // can't use any class variables or functions now
    memset(this_ptr, 0, sizeof(CANPromise));
    return msg;
}

bool CANPromise::matches(can::CAN_Header &header) {
    //TODO: return something different if we have already matched
    return (header.bits && mask.bits) == filter.bits;
}

void CANPromise::trigger_match(can::CAN_ROS_Message msg, bool error) {
    //TODO: handle if wait hasn't been called
    this->error = error;
    this->msg = msg;
    rtos_signal_send(waiting_on, signal);
}

bool CANPromiseManager::match_message(can::CAN_ROS_Message msg) {
    for(
            char * current_buff = (char*) buffer;
            (CANPromise*)current_buff < ((CANPromise*)buffer)+buffer_size;
            current_buff+=sizeof(CANPromise)*buffer_size
    ) {
        // if the first two bytes are null
        if(!current_buff[0] && !current_buff[1]) {
            CANPromise * pbuff = ((CANPromise *)current_buff);
            if(pbuff->matches(msg.head)) {
                pbuff->trigger_match(msg, false);
                return true;
            }
        }
    }
    return false;
}

CANPromiseManager::CANPromiseManager(void *buffer, size_t buffer_size) :
    buffer(buffer),
    buffer_size(buffer_size)
{
    // we use this to determine if the buffer is empty
    memset(buffer, 0, buffer_size*sizeof(CANPromise));
}