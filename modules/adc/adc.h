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
 * for these pins. */
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
enum adc_return {
    ADC_SUCCESS,
    ADC_FAILURE
};

/* Used as a return status indicating whether an ADC capture has
 * completed. */
enum adc_status {
    ADC_COMPLETE,
    ADC_BUSY
};

//----------------------------FUNCTIONS-------------------------------//
/* INITIALISATION */
/* Initialises the ADC module, configures the needed GPIO pins, and
 * configures and enables the needed ADC pins.
 *
 * This function should ONLY BE RUN ONCE during I/O setup.
 *
 * The maximum number of pins currently supported is 8. This function
 * should fail if num_pins > 8. */
extern enum adc_status adc_init_pins(adc_pin pins[], uint8_t num_pins, 
    uint16_t buffer[], void (*callback)(void));

/* INTERRUPT CAPTURE INTERFACE */
/* Sets up an ADC capture using its hardware capture interrupt. That is,
 * it registers the buffer and callback such that when the capture 
 * completes, an interrupt occurs, the capture data is copied to the
 * buffer, and the callback is called. */
enum adc_return adc_capture_interrupt(uint16_t *buffer, 
    void (*callback)(void));

/* POLLING CAPTURE INTERFACE */
/* Sets up the ADC capture without enabling the interrupt. A capture
 * will occur without creating any signals, so the status must be 
 * probed with adc_capture_status(). */
enum adc_return adc_capture_polling();

/* Checks the adc capture status. Will return ADC_COMPLETE if the ADC
 * is not currently busy, but does not guarentee that new data is 
 * available. */
enum adc_status adc_capture_status();

/* Transfers ADC capture data from the sequence registers to the given
 * buffer. Must be called between captures, else new data will be
 * dropped. */
enum adc_status adc_get_capture(uint16_t *buffer);
#endif
