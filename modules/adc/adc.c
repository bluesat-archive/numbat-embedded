#include "adc.h"

#include <stdbool.h>
#include <stdint.h>


#define PC 8 // TODO number of adc pins


// Shared constants, and constant lookup tables

// Board specific constants, and constant lookup tables
#define ADC_TEST_BOARD
#ifdef ADC_TEST_BOARD
// Test board constants
#else
// Generic PCB constants
#endif


// Internal functions
    

// External functions
enum adc_status adc_init(void) {
    return ADC_FAILURE;
}
