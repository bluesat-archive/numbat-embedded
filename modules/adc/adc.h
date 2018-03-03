/* Author: William Miles
 * Date: 2018-03-03
 *
 * NB: This library provides a vastly simplified interface to the ADC
 *     module. If more features or greater performance is required then
 *     direct usage of the module through the Tiva libraries is 
 *     recommended. 
 */

#ifndef ADC_H
#define ADC_H

//------------------------------ENUMS---------------------------------//
/* ADC Pins, using ADC Pin names. See ./PIN_MAPPING for the other names
 * for these pins.
 */
enum adc_pin {
    AIN0,
    AIN1,
    AIN2,
    AIN3,
    AIN4,
    AIN5,
    AIN6,
    AIN7,
};

/* Possible return values for the ADC functions. */
enum adc_status {
    ADC_SUCCESS,
    ADC_FAILURE
};

//----------------------------FUNCTIONS-------------------------------//
/* Initialises the given pins, registers the data buffer, and registers
 * the callback that is called when an ADC capture occurs.
 *
 * The buffer and pins list must have num_pins number of entries.
 *
 * The buffer will be automatically updated on an adc capture. The
 * callback is provided as a way to create a signal of some sort for a 
 * task that will handle the ADC output.
 *
 * This function should ONLY BE RUN ONCE during I/O setup.
 *
 * The maximum number of pins currently supported is 8. This function
 * should fail if num_pins > 8.
 */
extern enum adc_status adc_init_pins(adc_pin pins[], uint8_t num_pins, 
    uint16_t buffer[], void (*callback)(void));

/* Sets the hardware flags that start an ADC capture. 
 *
 * This library currently only supports manual single-shot captures that
 * are started by this function. However, the hardware does support 
 * continuous capture, capture triggered by a PWM module, and capture
 * triggered by an external source.
 */
extern enum adc_status adc_start_capture();

#endif
