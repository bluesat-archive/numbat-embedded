//------------------------------INCLUDES------------------------------//
#include "adc.h"

//---------------------------ERROR_HANDLING---------------------------//

//---------------------------SIZE_CONSTANTS---------------------------//
#define PC 8              /* ADC pin count */
#define GPIO_PORT_COUNT 2 /* number of GPIO ports used by ADC */


//-------------------------------ENUMS--------------------------------//
enum {
    PRE_INIT,
    POST_INIT
};

//------------------------------STRUCTS-------------------------------//
struct gpio_port {
    const uint32_t sysctl;
    const uint32_t base;
}; /* struct for data required for manipulating GPIO ports */

struct gpio_pin {
    const struct gpio_port port;
    const uint8_t pin;
};


//---------------------SHARED_HARDWARE_CONSTANTS----------------------//
/* ADC configuration arguments */
const uint32_t adc_clock_div = 1;
const uint32_t adc_clock_config =
    ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;
const uint32_t sequence_num = 0;
const uint32_t adc_sc_module = SYSCTL_PERIPH_ADC0;


//----------------------BOARD_SPECIFIC_CONSTANTS----------------------//
#ifdef PART_TM4C123GH6PM
const uint32_t adc_reference = ADC_REF_INT;

const struct gpio_pin gpio_lut[PC] = {
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_3},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_2},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_1},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_0},
    {{SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE}, GPIO_PIN_7},
    {{SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE}, GPIO_PIN_6},
    {{SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE}, GPIO_PIN_5},
    {{SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE}, GPIO_PIN_4}};
#endif
#ifdef PART_TM4C123GH6PGE
const uint32_t adc_reference = ADC_REF_EXT_3V;

const struct gpio_pin gpio_lut[PC] = {
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_3},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_2},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_1},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_0},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_7},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_6},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_5},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_4}};
#endif


//------------------------------GLOBALS-------------------------------//
int status = PRE_INIT;
uint8_t active_pins = 0;
uint32_t *adc_buffer = (void *)0;
void (*adc_callback)(void) = (void *)0;


//--------------------------LOCAL_FUNCTIONS---------------------------//
/* If a module is not already enabled: enables it, then waits until it
 * is responding as ready. */
void init_module(uint32_t sysctl_module) {
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        SysCtlPeripheralEnable(sysctl_module);
        while (SysCtlPeripheralReady(sysctl_module) == false)
            ;
    }
}


//-------------------------EXTERNAL_FUNCTIONS-------------------------//
/* Interrupt handler that needs to be configured in a module's prx file
 * if the interrupt mode is used.
 */
void adc_irq_handler(void) {
    /* Clear interrupt flag so we don't instantly return to the handler */
    ADCIntClear(ADC0_BASE, sequence_num);
    ADCSequenceDataGet(ADC0_BASE, sequence_num, adc_buffer);
    /* call the callback */
    adc_callback();
}

enum adc_return adc_init_pins(enum adc_pin *pins, uint8_t num_pins, bool interrupt_mode) {
//        adc_assert(status == PRE_INIT);
//        adc_assert(num_pins < 9);
//        adc_assert(pins != (void *)0);

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
            init_module(gpio_lut[pins[i]].port.sysctl);
            /* configure ADC/GPIO pins */
            GPIOPinTypeADC(gpio_lut[pins[i]].port.base, gpio_lut[pins[i]].pin);
        }

        /* disable before configuring sequencer */
        ADCSequenceDisable(ADC0_BASE, sequence_num);

        /* setup ADC sample sequencer, trigger manually in software */
        ADCSequenceConfigure(ADC0_BASE, sequence_num,
            ADC_TRIGGER_PROCESSOR, 0);

        /* configure each step of sequence to be an analog input source */
        int i = 0;
        for (i = 0; i < num_pins-1; i++) {
            ADCSequenceStepConfigure(ADC0_BASE, sequence_num, i, pins[i]);
        }
        /* last step is end of sequence, generate an interrupt here */
        ADCSequenceStepConfigure(ADC0_BASE, sequence_num, i,
            pins[i] | ADC_CTL_IE | ADC_CTL_END);
        /* never use this function, echronos handles irq registration */
        // ADCIntRegister(ADC0_BASE, sequence_num, adc_irq_handler);
        ADCSequenceEnable(ADC0_BASE, sequence_num);
        /* enable interrupts if in interrupt mode */
        if (interrupt_mode) {
            ADCIntEnable(ADC0_BASE, sequence_num);
            IntEnable(INT_ADC0SS0);
        }
        /* ready for capture */
        status = POST_INIT;
        return ADC_SUCCESS;
}

enum adc_return adc_capture_interrupt(uint32_t *buffer, void (*callback)(void)) {
//    adc_assert(status == POST_INIT);
//    adc_assert(buffer != (void *)0);
//    adc_assert(callback !=(void *)0);
    adc_buffer = buffer;
    adc_callback = callback;
    ADCProcessorTrigger(ADC0_BASE, sequence_num);
    return ADC_SUCCESS;
}



uint32_t adc_capture_polling(uint32_t *buffer) {
//    adc_assert(status == POST_INIT);
    ADCProcessorTrigger(ADC0_BASE, sequence_num);
    while (!ADCIntStatus(ADC0_BASE, sequence_num, false)) {};
    uint32_t num_samples = ADCSequenceDataGet(ADC0_BASE, sequence_num, buffer);
    return num_samples;
}


enum adc_return adc_interrupt_disable() {
    ADCIntDisable(ADC0_BASE, sequence_num);
    IntDisable(INT_ADC0SS0);
    return ADC_SUCCESS;
}

enum adc_return adc_interrupt_enable() {
    ADCIntEnable(ADC0_BASE, sequence_num);
    IntEnable(INT_ADC0SS0);
    return ADC_SUCCESS;
}
