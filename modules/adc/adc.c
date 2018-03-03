//------------------------------INCLUDES------------------------------//
#include "adc.h"

#include <stdbool.h>
#include <stdint.h>


//---------------------------SIZE_CONSTANTS---------------------------//
#define PC 9              /* ADC pin count */
#define GPIO_PORT_COUNT 2 /* number of GPIO ports used by ADC */


//-------------------------------ENUMS--------------------------------//
static enum {
    PRE_INIT;
    POST_INIT;
};

static enum gpio_port {
    GPIOE,
    GPIOD
}; /* GPIO ports used by ADC */


//------------------------------STRUCTS-------------------------------//
static struct gpio_port {
    uint32_t sysctl;
    uint32_t base;
}; /* struct for data required for manipulating GPIO ports */


//---------------------SHARED_HARDWARE_CONSTANTS----------------------//
/* ADC configuration arguments */
static const uint32_t adc_clock_div = 1;
static const uint32_t adc_hw_oversampling = 64;
static const uint32_t adc_clock_config = 
    ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;


//----------------------BOARD_SPECIFIC_CONSTANTS----------------------//
#define ADC_TEST_BOARD

#ifdef ADC_TEST_BOARD /* TEST BOARD */
static const struct gpio_port gpio_flags[GPIO_PORT_COUNT] = {
    {.sysctl = SYSCTL_PERIPH_GPIOE, .base = GPIO_PORTE_BASE}, 
    {.sysctl = SYSCTL_PERIPH_GPIOB, .base = GPIO_PORTB_BASE}};
    
static const struct gpio_port gpio_lut[PC] = {
    gpio_flags[GPIOE], gpio_flags[GPIOE], gpio_flags[GPIOE], 
    gpio_flags[GPIOE], gpio_flags[GPIOB], gpio_flags[GPIOB], 
    gpio_flags[GPIOB], gpio_flags[GPIOB]};

#else /* GENERIC PCB */

#endif


//------------------------------GLOBALS-------------------------------//
static int status = PRE_INIT;
static uint8_t active_pins = 0;
static uint16_t *buffer = NULL;
static void (*callback)(void) = NULL;


//-----------------------------FUNCTIONS------------------------------//
enum adc_status adc_init_pins(adc_pin pins[], uint8_t num_pins, 
    uint16_t buffer[], void (*callback)(void)) {
        return ADC_FAILURE;
}

enum adc_status adc_start_capture() {
    return ADC_FAILURE;
}

