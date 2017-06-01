/**
 * @date: 31/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a thread safe memory allocation system using echronos
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_ALLOC_HPP
#define NUMBAT_EMBEDDED_ALLOC_HPP

#define ALLOC_BUFFER_SIZE 2048
#define ALLOC_MIN_ALLOC 16

#include "rtos-kochab.h"

#define ALLOC_MUTEX RTOS_MUTEX_ID_ALLOC

void *memcpy(void *dest, const void *src, size_t n);


namespace alloc {

    /**
     * Initalise the memory manegment system
     */
    void init_mm();
    /**
     * Allocates a block of memory
     *
     * Triggers a software interupt if no memory is available
     *
     * @param size the amount of memory (in bytes) to allocate
     * @return the pointer to that memory
     */
    void * malloc(size_t size);

    /**
     * Frees a block of memory
     *
     * @pre ptr must be the start of a block of allocated memory
     *
     * @param ptr the *start* of the block of memory to free
     */
    void free(void * ptr);
}

#endif //NUMBAT_EMBEDDED_ALLOC_HPP
