//
// Created by ros on 7/10/17.
//
extern "C" {

typedef void (*func_ptr)(void);
func_ptr _init_array_end[0] __attribute__ ((used, section(".init_array"), aligned(sizeof(func_ptr)))) = {};
func_ptr _fini_array_end[0] __attribute__ ((used, section(".fini_array"), aligned(sizeof(func_ptr)))) = {};


}