#include "adc.h"

#include <stdbool.h>
#include <stdint.h>


#define PC 9 // number of adc pins

// GPIO data structuring
#define GPIO_PORT_COUNT 2

static enum gpio_port {
    GPIOE,
    GPIOD
};

static struct gpio_port {
    uint32_t sysctl;
    uint32_t base;
};


// Shared constants, and constant lookup tables
static const uint32_t adc_clock_div = 1;
static const uint32_t adc_hw_oversampling = 64;
static const uint32_t adc_clock_config = ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;

// Board specific constants, and constant lookup tables
#define ADC_TEST_BOARD
#ifdef ADC_TEST_BOARD
// Test board constants
// non-static so that it can be optimised out?
const struct gpio_port gpio_flags[GPIO_PORT_COUNT] = {
    {.sysctl = SYSCTL_PERIPH_GPIOE, .base = GPIO_PORTE_BASE}, 
    {.sysctl = SYSCTL_PERIPH_GPIOB, .base = GPIO_PORTB_BASE}};
    
static const struct gpio_port gpio_lut[PC] = {
    gpio_flags[GPIOE], gpio_flags[GPIOE], gpio_flags[GPIOE], gpio_flags[GPIOE],
    gpio_flags[GPIOB], gpio_flags[GPIOB], gpio_flags[GPIOB], gpio_flags[GPIOB]};


#else
// Generic PCB constants
#endif


// Internal functions
enum adc_status adc_init(void) {
    // Enable ADC module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Wait for module to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
        ;


    // Enable ADC clock. Must be 16 MHz, which can be provided by PIOSC
    ADCClockConfigSet(ADC0_BASE, adc_clock_config, adc_clock_div);
    // Enable GPIO clock if not enabled
    // Set GPIO AFSEL bit
    // Clear GPIO DEN bit
    // Configure averaging
    ADCHardwareOversampleConfigure(ADC0_BASE, adc_hw_oversampling);
    // Register interrupt handler?
    return ADC_FAILURE;
}
    

// External functions

// initialises a pin for adc capture
extern enum adc_status adc_init_pin(adc_pin pin) {
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
