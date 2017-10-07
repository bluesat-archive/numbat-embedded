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
enum adc_status adc_init(void)
{
    // Enable ADC clock
    // Enable GPIO clock if not enabled
    // Set GPIO AFSEL bit
    // Clear GPIO DEN bit
    // Configure averaging
    // Register interrupt handler?
    return ADC_FAILURE;
}
    

// External functions

// initialises a pin for adc capture
extern enum adc_status adc_init_pin(adc_pin pin)
{
    // Disable isolation via the GPIOAMSEL bit
    // Disable sample sequencer via ASENn in ADCACTSS
    // Append pin as source to sequencer via ADCSSCTLn
    // Re-enable sample sequencer via ASENn in ADCACTSS
}

// returns the most recently captured value for an 
// initialised adc pin
extern enum adc_value adc_get_pin_value(adc_pin pin) {
    // return the value for the pin from the adc buffer
}
