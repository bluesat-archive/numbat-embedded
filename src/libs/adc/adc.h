/* Author: William Miles
 *
 * NB: This library provides a simplified interface to the ADC
 *     module. If more features or greater performance is required then
 *     direct usage of the module through the Tiva libraries is
 *     recommended.
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

/* ADC Pins, using ADC Pin names. See docs/adc/PIN_MAPPING for the other 
 * names for these pins. */
enum adc_pin {
    AIN0 = 0x0,
    AIN1 = 0x1,
    AIN2 = 0x2,
    AIN3 = 0x3,
    AIN4 = 0x4,
    AIN5 = 0x5,
    AIN6 = 0x6,
    AIN7 = 0x7,
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

#ifdef __cplusplus
extern "C" {
#endif

/* Must be configured in a module's PRX file if the interrupt mode is
 * used. */
void adc_irq_handler(void);

/* INITIALISATION */
/* Initialises the ADC module, configures the needed GPIO pins, and
 * configures,enables the needed ADC pins, and configures the ADC for interrupt
 * or polling mode.
 *
 * This function should ONLY BE RUN ONCE during I/O setup.
 *
 * The maximum number of pins currently supported is 8. This function
 * should fail if num_pins > 8. */
enum adc_return adc_init_pins(enum adc_pin *pins, uint8_t num_pins, bool interrupt_mode);

/* INTERRUPT CAPTURE INTERFACE */
/* Sets up an ADC capture using its hardware capture interrupt. That is,
 * it registers the buffer and callback such that when the capture
 * completes, an interrupt occurs, the capture data is copied to the
 * buffer, and the callback is called.
 *
 * The ADC module must be initialised in interrupt mode, or interrupts can be
 * enabled manually via adc_interrupt_enable() for this to function properly.
 */
enum adc_return adc_capture_interrupt(uint32_t *buffer, void (*callback)(void));

/* POLLING CAPTURE INTERFACE */
/* Initiates an ADC capture sequence in polling mode. The status register is
 * polled until a capture sequence is complete and the result is stored in the
 * specified buffer.
 *
 * The ADC module must be intiialised in polling mode, or interrupts can be
 * disbled manually via adc_interrupt_disable() for this to function properly.
 *
 * Returns the number of samples captured in the sequence.
 */
uint32_t adc_capture_polling(uint32_t *buffer);

// Enable the ADC interrupt; required for ADC functionality.
enum adc_return adc_interrupt_disable();

// Disable the ADC interrupt that is required for ADC functionality.
enum adc_return adc_interrupt_enable();

#ifdef __cplusplus
}
#endif


#endif
