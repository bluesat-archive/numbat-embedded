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

#include <cstdarg>
#include "SPSCQueue.h"
#include "boilerplate.h"
#include "alloc.hpp"

#define ROS_STR_LEN 15


namespace ros_echronos {
    namespace can {
        extern int can_error_flag;

        const uint8_t CAN_MESSAGE_MAX_LEN = 8;

        /**
         * Value for the ros mode on the can bus
         */
        const uint8_t ROS_CAN_MODE = 1;
        /**
         * Indicates that the "seq_num" field of a can message header is in special mode
         */
         const uint8_t SEQ_NUM_SPECIAL_MODE = 7;

        typedef enum _ROS_Function {
            FN_ROS_MESSAGE_TRANSMISSION = 0,
            FN_ROS_SERVICE = 1,
            FN_ROS_CONTROL_MSG = 2,
            FN_ROS_RESERVED = 3
        } ROS_Function;
        /**
         * Used to store the base register of the can bus (CAN0)
         */
        extern uint32_t can_base;

        typedef union can_header {
            uint32_t bits;
            struct _header_fields {
                // these MUST be ints to make the compiler place them in the same 32-bit container
                // See: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491i/Babjddhe.html
                // (Also gcc docs, but they are less clear)
                unsigned int mode : 1;
                unsigned int priority : 2;
                unsigned int ros_function : 2;
                unsigned int seq_num : 3;
                unsigned int message_num : 2;
                unsigned int topic : 7;
                unsigned int message_length : 8;
                unsigned int node_id : 4;
                unsigned int not_in_range : 3;
            }__attribute__((packed)) fields;
        } CAN_Header;

        typedef struct can_ros_message {
            CAN_Header head;
            uint8_t body[CAN_MESSAGE_MAX_LEN];
            uint8_t body_bytes;
        } CAN_ROS_Message __attribute__((aligned(4)));


        /**
         * We can't use mutexes in buffers so we have to assume that we do not receive another message
         * before this is written. I eventually want to replace this with a lockless queue.
         */
         typedef  struct _input_buffer {
             CAN_ROS_Message buffer;
             int start_counter = 0;
             int end_counter = 0;
         } input_buffer_t;

        const CAN_Header TOPIC_BITMASK_HEADER = {
                .fields={
                    1, 0, 0xF, 0, 0, 0xFFFFF, 0, 0, 0
                }
        };


        extern input_buffer_t input_buffer;
        extern rigtorp::SPSCQueue<CAN_ROS_Message, 5> msg_queue;

        extern RtosInterruptEventId can_interupt_event;
        extern volatile bool node_handle_ready;
    }
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
             * Used by message descriptors to access the values array
             * @return ptr to Array.values
             */
            virtual void * get_values_ptr() = 0;

        friend Message_Descriptor;
    };

    template <typename T> class Array : public _Array {
        public:
            T operator[] (int index) {
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
            virtual void * get_values_ptr();

    };


    typedef Array<char> String;

    extern RtosMutexId write_mutex;
    extern bool write_mutex_set;

    inline void ROS_INFO(const char *pcString, ...) {
        va_list args;
        va_start(args, pcString);
        if(write_mutex_set) {
            rtos_mutex_lock(write_mutex);
        }

        UARTvprintf(pcString, args);

        if(write_mutex_set) {
            rtos_mutex_unlock(write_mutex);
        }
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
    memcpy(values, arr.values, size);
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
        values = (char*) alloc::malloc(bytes);
        memcpy(values, new_value.values, size);
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

template <typename T> inline void * ros_echronos::Array<T>::get_values_ptr() {
    return (void*)values;
}

extern "C" void ros_can_int_handler(void);


#endif //NUMBAT_EMBEDDED_ROS_HPP
