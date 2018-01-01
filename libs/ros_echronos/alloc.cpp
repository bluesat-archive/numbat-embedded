/**
 * @date: 31/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a thread safe memory allocation system using echronos
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "boilerplate.h"
#include "include/alloc.hpp"
#include "ros.hpp"
#include "tlsf.h"

using namespace alloc;

#define MEMORY_MANAGMENT_D_STRUCTURE_SIZE (ALLOC_BUFFER_SIZE/ALLOC_MIN_ALLOC)
#define BUFF_2_ALLOC_SHIFT 3
#define FREE_MASK 0x80

uint8_t buffer[ALLOC_BUFFER_SIZE];
static RtosMutexId mutex;
static tlsf_t tlsf;


#define is_free(ptr) ((*ptr) ^ FREE_MASK)

void alloc::init_mm(const RtosMutexId alloc_mutex) {
    mutex = alloc_mutex;
    tlsf = tlsf_create_with_pool(buffer, ALLOC_BUFFER_SIZE);
    UARTprintf("tlsf %p\n", tlsf);
}

void * alloc::malloc(size_t size) {
    rtos_mutex_lock(mutex);
    ros_echronos::ROS_INFO("alloc size %d\n",size);
    void * val = tlsf_malloc(tlsf, size);
    ros_echronos::ROS_INFO("alloc'd %p\n", val);
    rtos_mutex_unlock(mutex);
    return val;
}

void alloc::free(void * ptr) {
    rtos_mutex_lock(mutex);
    ros_echronos::ROS_INFO("Dealloc %p\n", ptr);
    tlsf_free(tlsf, ptr);
    rtos_mutex_unlock(mutex);
}

