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
//#define DEBUG_ALLOC

uint8_t buffer[ALLOC_BUFFER_SIZE];
static RtosMutexId mutex;
static tlsf_t tlsf;


#define is_free(ptr) ((*ptr) ^ FREE_MASK)

#ifdef DEBUG_ALLOC
void walker(void * ptr, size_t size, int used, void * user) {
    UARTprintf("\tptr 0x%x, size %u used %d\n", ptr, size, used);
}
#endif

void alloc::init_mm(const RtosMutexId alloc_mutex) {
    mutex = alloc_mutex;
    tlsf = tlsf_create_with_pool(buffer, ALLOC_BUFFER_SIZE);
#ifdef DEBUG_ALLOC
    UARTprintf("tlsf %p\n", tlsf);
#endif
}

void * alloc::malloc(size_t size) {
#ifdef DEBUG_ALLOC
    ros_echronos::ROS_INFO("alloc size %d\n",size);
#endif
    rtos_mutex_lock(mutex);
    void * val = tlsf_malloc(tlsf, size);
    rtos_mutex_unlock(mutex);
#ifdef DEBUG_ALLOC
    ros_echronos::ROS_INFO("A+ alloc'd %p\n", val);
    tlsf_walk_pool(tlsf, walker, NULL);
#endif
    if(!val) {
        while (true){
            ros_echronos::ROS_INFO("NULL alloc\n");
        }
    }
    return val;
}

void alloc::free(void * ptr) {
    rtos_mutex_lock(mutex);
#ifdef DEBUG_ALLOC
    ros_echronos::ROS_INFO("A- Dealloc %p\n", ptr);
#endif
    tlsf_free(tlsf, ptr);
    rtos_mutex_unlock(mutex);
}


