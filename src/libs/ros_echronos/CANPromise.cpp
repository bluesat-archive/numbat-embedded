/*
 * @date: 17/06/18
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors)
 * @details: Simple Promise Library for CAN Messages
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */

#include <string.h>
#include <new>
#include <can_impl.hpp>

#include "include/CANPromise.hpp"

using namespace ros_echronos::promise;

CANPromise::CANPromise(can::CAN_Header & mask, can::CAN_Header & filter) :
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

ros_echronos::can::CAN_ROS_Message CANPromise::wait(const RtosSignalId &signal) {
    this->signal = signal;
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
    return m;
}

bool CANPromise::matches(const can::CAN_Header &header) {
    //TODO: return something different if we have already matched
    return (header.bits & filter.bits) == (mask.bits & filter.bits);
}

void CANPromise::trigger_match(const can::CAN_ROS_Message &msg, const bool error) {
    //TODO: handle if wait hasn't been called
    this->error = error;
    this->msg = msg;
    rtos_signal_send(waiting_on, signal);
}

CANPromise * CANPromiseManager::match(can::CAN_Header mask, can::CAN_Header filter) {
    CANPromise * ret;
    // overall subscription masks & filters
    uint32_t ctrl_mask = mask.bits;
    uint32_t ctrl_filter = filter.bits;


    for(CANPromise * local_buffer = static_cast<CANPromise *>(this->buffer); local_buffer < (this->buffer + buffer_size); ++local_buffer) {
        // NOTE: this will break if we support more than 64 promises!
        const uint64_t buffer_free_mask = (1 << local_buffer - static_cast<CANPromise *>(this->buffer));
        if(!(*(int64_t *)bitmask & buffer_free_mask)) {
            ret = new(local_buffer) CANPromise(mask, filter);
            *(int64_t *)bitmask |= buffer_free_mask;
        } else {
            // space is occupied
            // the sum of the filters is the & of the ctrl masks
            // and the and of the bits in the filter that are masked and of equal value
            ctrl_filter = ((ctrl_mask & ctrl_filter) & (local_buffer->mask.bits & (local_buffer->filter.bits))) &
                ((~ctrl_mask & ctrl_filter) & (~(local_buffer->mask.bits) & (local_buffer->filter.bits)));
            //TODO: simplify the above boolean logic

            ctrl_mask &= ((CANPromise*)local_buffer)->mask.bits;
        }
    }
    ros_echronos::can::set_ctrl_sub(ctrl_mask, ctrl_filter);
    return ret;
}

bool CANPromiseManager::match_message(const can::CAN_ROS_Message & msg) {
    // short circuit for message fast path
    // check if the buffer is empty
    if(!bitmask) {
        return false;
    }

    bool found = false;
    uint8_t promises = 0;
    // overall subscription masks & filters
    uint32_t ctrl_mask = 0;
    uint32_t ctrl_filter = 0;
    for(
            char * current_buff = (char*) buffer;
            (CANPromise*)current_buff < ((CANPromise*)buffer)+buffer_size;
            current_buff+=sizeof(CANPromise)*buffer_size
    ) {
        // NOTE: this will break if we support more than 64 promises!
        const uint64_t buffer_free_mask = (1 << (CANPromise*)current_buff - static_cast<CANPromise *>(this->buffer));
        if((*(int64_t *)bitmask & buffer_free_mask)) {
            CANPromise * pbuff = ((CANPromise *)current_buff);
            if(pbuff->matches(msg.head)) {
                pbuff->trigger_match(msg, false);
                found = true;
                *(int64_t *)bitmask ^= buffer_free_mask;
            } else {
                ++promises;
                // space is occupied
                // the sum of the filters is the & of the ctrl masks
                // and the and of the bits in the filter that are masked and of equal value
                ctrl_filter = ((ctrl_mask & ctrl_filter) & (((CANPromise*)buffer)->mask.bits & ((CANPromise*)buffer)->filter.bits)) &
                              ((~ctrl_mask & ctrl_filter) & (~((CANPromise*)buffer)->mask.bits) & ~((CANPromise*)buffer)->filter.bits);
                //TODO: simplify the above boolean logic

                ctrl_mask &= ((CANPromise*)buffer)->mask.bits;
            }
        }
    }
    // update the filter
    if(found) {
        if(promises > 0) {
            can::set_ctrl_sub(ctrl_mask, ctrl_filter);
        } else {
            can::clear_ctrl_sub();
        }
    }
    return found;
}

CANPromiseManager::CANPromiseManager(void *buffer, void *bitmask, size_t buffer_size) :
    buffer(buffer),
    bitmask(bitmask),
    buffer_size(buffer_size)
{
    // we use this to determine if the buffer is empty
    memset(buffer, 0, buffer_size*sizeof(CANPromise));
    memset(bitmask, 0, buffer_size);
}