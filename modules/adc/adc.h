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
extern enum adc_status adc_init(void);


#endif
