/**
 * @date: 30/04/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * Editors:
 * Purpose: This provides the core file for implementing ros on echronos
 * This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_ROS_HPP
#define NUMBAT_EMBEDDED_ROS_HPP

#include "boilerplate.h"
#include "alloc.hpp"
#include "time.hpp"

#define ROS_STR_LEN 15


namespace ros_echronos {
    /**
     * bits in a node id
     */
    constexpr uint8_t NODE_ID_WIDTH = 4;

    template <class T> class Publisher;
    template <class T> class Subscriber;
    class NodeHandle;
    /**
     * used to indicate which transmission mode and publisher or subscriber is in
     */
    enum Transmission_Mode { DROP_MISSING, REREQUEST_MISSING};

    class Message_Descriptor;

    /**
     * Class used by message descriptors to decode arrays
     */
    class _Array {
        protected:
            /**
             * Used by message descriptors to load data into empty array classes
             * @param size the size to load
             */
            virtual void override_with_new_size(const size_t &size) = 0;

            /**
             * Used by message descriptors to load data into empty array classes
             * @param bytes the number of bytes to load
             */
            virtual void override_with_new_bytes(const size_t &bytes) = 0;

            /**
             * Used by message descriptors to access the values array
             * @return ptr to Array.values
             */
            virtual void * get_values_ptr() = 0;

        friend Message_Descriptor;
    };

    template <typename T> class Array : public _Array {
        public:
            T operator[] (const int index) {
                return values[index];
            }

            const T operator[] (const int index) const {
                return values[index];
            }

            Array(size_t size);

            //copy constructor
            Array(const Array & arr);

            /**
             * Used to define an empty array
             */
            Array();

            ~Array();

            /**
             * Allows this to be assigned to an array if needed
             * @return the array
             */
            operator T*();
            /**
             * Allows this to be assigned to an array if needed
             * @return the array
             */
            operator void*();

            /**
             * Allows overwriting a class with a new array
             *
             * This operation performs a deep copy
             *
             * @param new_value the new value to overwrite with
             */
            Array<T> & operator  =(const Array<T> & new_value);


            /**
             * Number of elements in the array
             */
            size_t size;
            /**
             * Number of bytes
             */
            size_t bytes;
            T * values;

        protected:

            virtual void override_with_new_size(const size_t &size);
            virtual void override_with_new_bytes(const size_t &bytes);
            virtual void * get_values_ptr();

    };


    typedef Array<char> String;

    extern RtosMutexId write_mutex;
    extern bool write_mutex_set;

    inline void ROS_INFO(const char *pcString, ...) {
        va_list args;
        va_start(args, pcString);
//#define ROS_INFO_SERIAL
#if ROS_INFO_SERIAL==1
        if(write_mutex_set) {
            rtos_mutex_lock(write_mutex);
        }

        UARTvprintf(pcString, args);
        UARTprintf("\n\r");
#ifdef UART_BUFFERED
        UARTFlushTx(false);
#endif

        if(write_mutex_set) {
            rtos_mutex_unlock(write_mutex);
        }
#endif
    }
}
//NOTE: this is too slow with this on
//#define DEBUG_ARRAY

/**
 * Because GCC for ARM is broken we need to add these here rather than in the class decleration
 */
template <typename  T>
ros_echronos::Array<T>::Array(size_t size) :
        size(size),
        bytes(size*sizeof(T)),
        // allow for empty arrays (note size is unsinged so we can't use >)
        values(size!=0 ? (T*)alloc::malloc(bytes) : NULL)
{
#ifdef DEBUG_ARRAY
    ROS_INFO("Init array size %d\n", size);
#endif
}

template <typename  T>
ros_echronos::Array<T>::Array() : size(0), bytes(0), values(NULL) {
#ifdef DEBUG_ARRAY
    ROS_INFO("Warning: initalising empty array\n");
#endif
}
template <typename T>
ros_echronos::Array<T>::Array(const Array & arr) : Array(arr.size) {
#ifdef DEBUG_ARRAY
    ROS_INFO("Init array - cc. Size %d\n", arr.size);
#endif
    memcpy(values, arr.values, bytes);
}
template <typename T> inline
ros_echronos::Array<T>::~Array() {
    if(size != 0) {
        alloc::free(values);
    }
}

template <typename T> inline ros_echronos::Array<T>::operator T*() {
    return values;
}

template <typename T> inline ros_echronos::Array<T>::operator void*() {
    return values;
}

template <typename T> inline  ros_echronos::Array<T> & ros_echronos::Array<T>::operator  =(const ros_echronos::Array<T> & new_value) {
#ifdef DEBUG_ARRAY
    ROS_INFO("Init array - op=. size %d\n", new_value.size);
#endif
    if(size!=0) {
        alloc::free(values);
    }
    size = new_value.size;
    if(size!=0) {
        bytes = size * sizeof(T);
        values = (T*) alloc::malloc(bytes);
        memcpy(values, new_value.values, bytes);
    }
    return *this;
}

template <typename T> inline void ros_echronos::Array<T>::override_with_new_size(const size_t &new_size) {

    if(size!=0) {
        alloc::free(values);
    }

    size=new_size;
    bytes=size * sizeof(T);

    if (size!=0) {
        values = (T *) alloc::malloc(bytes);
    } else {
        values = NULL;
    }
}
template <typename T> inline void ros_echronos::Array<T>::override_with_new_bytes(const size_t &new_bytes) {
    override_with_new_size(new_bytes / sizeof(T));
}

template <typename T> inline void * ros_echronos::Array<T>::get_values_ptr() {
    return (void*)values;
}

extern "C" void ros_can_int_handler(void);


#endif //NUMBAT_EMBEDDED_ROS_HPP
