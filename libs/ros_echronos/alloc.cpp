/**
 * @date: 31/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a thread safe memory allocation system using echronos
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#include "include/alloc.hpp"

using namespace alloc;

#define MEMORY_MANAGMENT_D_STRUCTURE_SIZE (ALLOC_BUFFER_SIZE/ALLOC_MIN_ALLOC)
#define BUFF_2_ALLOC_SHIFT 3
#define FREE_MASK 0x80

uint8_t buffer[ALLOC_BUFFER_SIZE];
/**
 * Points to a index in the *allocation* data structer
 */
static uint8_t * free_ptr;
static uint8_t alloc_structure[MEMORY_MANAGMENT_D_STRUCTURE_SIZE];
static uint8_t offset;

static inline void * buff_2_alloc(void *);
static inline void * alloc_2_buff(void *);

#define is_free(ptr) ((*ptr) ^ FREE_MASK)

void alloc::init_mm() {
    // zero the control structure, indicating a fully free area
    for(int i = 0; i < MEMORY_MANAGMENT_D_STRUCTURE_SIZE; ++i) {
        alloc_structure[i] = 0;
    }

    // set the free pointer
    free_ptr = alloc_structure;

    // calculate the offset between the buffer and the alloc structure
    offset = (uint8_t)(alloc_structure - buffer);
}

void * alloc::malloc(size_t size) {
    uint8_t i;
    uint8_t * start = NULL;
    void * const end = alloc_structure + MEMORY_MANAGMENT_D_STRUCTURE_SIZE;

    rtos_mutex_lock(ALLOC_MUTEX);
    do {
        while (!is_free(free_ptr)) {
            // jump ahead the distance indicated
            free_ptr+=(*free_ptr^FREE_MASK);
            if(free_ptr >= end) {
                free_ptr=alloc_structure;
            }
        }
        if(free_ptr+size <= end) {
            uint8_t *start = free_ptr;
            //check the region is large enough
            for (i = 0; i < size && is_free(start + i); ++i) {}
        } else {
            start = alloc_structure;
            i = 0;
        }
        //TODO: check for endless loop
    } while(i != size);

    // TODO: do allocation
    for(uint8_t j = size - 1; j >= 0; --j) {
        start[j] = j;
        start[j] |= FREE_MASK;
    }
    rtos_mutex_unlock(ALLOC_MUTEX);

    return alloc_2_buff(start);
}

void alloc::free(void * ptr) {
    uint8_t * struct_ptr = (uint8_t *) buff_2_alloc(ptr);
    uint8_t * const end = struct_ptr + ((*struct_ptr) ^ FREE_MASK);
    while(struct_ptr <= end) {
        (*struct_ptr) = 0;
        ++struct_ptr;
    }
}

static inline void * buff_2_alloc(void * ptr) {
    return (void *)((((uint8_t *)ptr - (uint8_t *)buffer) >> BUFF_2_ALLOC_SHIFT)+alloc_structure);
}
static inline void * alloc_2_buff(void * ptr) {
    return (void *)((((uint8_t *)ptr - (uint8_t *)alloc_structure) << BUFF_2_ALLOC_SHIFT)+buffer);
}
