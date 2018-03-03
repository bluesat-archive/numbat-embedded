//------------------------------INCLUDES------------------------------//
#include "adc.h"

#include <stdbool.h>
#include <stdint.h>


//---------------------------ERROR_HANDLING---------------------------//
#define c_assert(e)    ((e) ? (true) : (tst_debugging(\
    "%s,%d: assertion '%s' failed\n", __FILE__, __LINE__, #e), false)) 
#define adc_assert(e)    if (c_assert(e) == false) return ADC_FAILURE

void tst_debugging(char *frmt_str, char *file, char *line, char *err) {
    UARTprintf(frmt_str, file, line, err);
}

//---------------------------SIZE_CONSTANTS---------------------------//
#define PC 8              /* ADC pin count */
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

static struct gpio_pin {
    struct gpio_port port;
    uint8_t pin;
}


//---------------------SHARED_HARDWARE_CONSTANTS----------------------//
/* ADC configuration arguments */
static const uint32_t adc_clock_div = 1;
static const uint32_t adc_clock_config = 
    ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;
static const uint32_t sequence_num = 0;


//----------------------BOARD_SPECIFIC_CONSTANTS----------------------//
#define ADC_TEST_BOARD

#ifdef ADC_TEST_BOARD /* TEST BOARD */
static const uint32_t adc_sc_module = SYSCTL_PERIPH_ADC0;
static const uint32_t adc_reference = ADC_REF_INT;

static const struct gpio_port gpio_flags[GPIO_PORT_COUNT] = {
    {.sysctl = SYSCTL_PERIPH_GPIOE, .base = GPIO_PORTE_BASE}, 
    {.sysctl = SYSCTL_PERIPH_GPIOB, .base = GPIO_PORTB_BASE}};
    
static const struct gpio_pin gpio_lut[PC] = {
    {gpio_flags[GPIOE], GPIO_PIN_3}, {gpio_flags[GPIOE], GPIO_PIN_2}, 
    {gpio_flags[GPIOE], GPIO_PIN_1}, {gpio_flags[GPIOE], GPIO_PIN_0},
    {gpio_flags[GPIOB], GPIO_PIN_7}, {gpio_flags[GPIOB], GPIO_PIN_6},
    {gpio_flags[GPIOB], GPIO_PIN_5}, {gpio_flags[GPIOB], GPIO_PIN_4}};

#else /* GENERIC PCB */

#endif


//------------------------------GLOBALS-------------------------------//
static int status = PRE_INIT;
static uint8_t active_pins = 0;
static uint16_t *adc_buffer = NULL;
static void (*adc_callback)(void) = NULL;


//--------------------------LOCAL_FUNCTIONS---------------------------//
static void adc_irq_handler(void) {
    /* copy captured values into buffer */
    ADCSequenceGetData(ADC0_BASE, sequence_num, adc_buffer);

    /* call the callback */
    adc_callback();
}

static void init_module(uint32_t sysctl_module) {
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        SysCtlPeripheralEnable(sysctl_module)
        while (SysCtlPeripheralReady == false)
            ;
}


//-------------------------EXTERNAL_FUNCTIONS-------------------------//
enum adc_status adc_init_pins(adc_pin *pins, uint8_t num_pins, 
    uint16_t *buffer, void (*callback)(void)) {
        adc_assert(status == PRE_INIT);
        adc_assert(num_pins < 9);
        adc_assert(pins != NULL);
        adc_assert(buffer != NULL);
        adc_assert(callback != NULL);

        adc_buffer = buffer;
        adc_callback = callback;
        active_pins = num_pins;

        /* initialise ADC module */
        init_module(adc_sc_module);
        ADCIntDisable(ADC0_BASE, sequence_num);

        /* configure ADC module */
        ADCClockConfigSet(ADC0_BASE, adc_clock_config, adc_clock_div);
        ADCReferenceSet(ADC0_BASE, adc_reference);

        /* pin initialisation and configuration */
        for (int i = 0; i < num_pins; i++) {
            /* initialise GPIO module */
            init_module(gpio_lut[pins[i]].sysctl);

            /* configure ADC/GPIO pins */
            GPIOPinTypeADC(gpio_lut[pins[i]].port.base, 
                gpio_lut[pins[i]].pin);

        /* register interrupt handler */
        ADCIntRegister(ADC0_BASE, sequence_num, adc_irq_handler);

        /* setup ADC sample sequencer */
        ADCSequenceConfigure(ADC0_BASE, sequence_num,
            ADC_TRIGGER_PROCESSOR, 0);

        /* ready for capture */
        status = POST_INIT;
        ADCSequenceEnable(ADC0_BASE, sequence_num);
        ADCIntEnable(ADC0_BASE, sequence_num);

        return ADC_SUCCESS;
}

enum adc_status adc_start_capture() {
    adc_assert(status == POST_INIT);

    ADCProcessorTrigger(ADC0_BASE, sequence_num);

    return ADC_SUCCESS;
}

