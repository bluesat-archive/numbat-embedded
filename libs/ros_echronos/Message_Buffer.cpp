//
// Created by ros on 26/05/17.
//

#include "include/Message_Buffer.hpp"
template <class T> Message_Buffer<T>::Message_Buffer(T *const buffer, const uint8_t size) :
    buffer_start(buffer), buffer_end(buffer + size), buffer_head(buffer_end-1), buffer_tail(buffer_head)  {

}

template <class T> T Message_Buffer<T>::pop() {
    T msg = (*buffer_head);
    --buffer_head;
    if(buffer_head < buffer_start) {
        buffer_head = buffer_end-1;
    }
    //note as we assume isEmpty is false, we don't need to check the case where we decrement past the tail
    return msg;
}

template <class T> void Message_Buffer<T>::put(T msg) {
    (*buffer_tail) = msg;
    --buffer_tail;
    if(buffer_tail < buffer_start) {
        buffer_tail = buffer_end-1;
    }
}

template <class T> bool Message_Buffer<T>::is_empty() {
    return buffer_head == buffer_tail;
}