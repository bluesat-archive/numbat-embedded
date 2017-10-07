#ifndef ADC_H
#define ADC_H


// adc types


// adc enums

// adc function status return values
enum adc_status {
    ADC_SUCCESS,
    ADC_FAILURE
};


// adc functions

// initialises a pin for adc capture
extern enum adc_status adc_init_pin(adc_pin pin);

// returns the most recently captured value for an 
// initialised adc pin
extern enum adc_value adc_get_pin_value(adc_pin pin);

#endif
