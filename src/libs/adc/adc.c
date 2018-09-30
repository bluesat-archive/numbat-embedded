/**
 * Date Started: 05/2018
 *
 * @author: wmpmiles [William Miles]
 *
 * @description: This is a simplified interface for the ADC input peripheral
 * on the TM4C123GH6GPE and TM4C123GH6PM ARM microcontrollers that utilises the
 * Tiva libraries provided by TI. It covers only a small subset of the features
 * of the ADC peripheral, more fine-tuned use should be done directly with the 
 * Tiva libraries or via direct hardware register manipulation.
 *
 * @copyright: This code is released under the BSD and AGPL Licenses. Copyright 
 * BLUEsat UNSW, 2018
 */
#include "adc.h"

// error handling/debugging macros & functions
#define c_assert(e)    ((e) ? (true) : (tst_debugging("%s,%d: assertion '%s' failed\n", __FILE__, __LINE__, #e), false)) 
#define adc_assert(e)    if (c_assert(e) == false) return ADC_FAILURE

void tst_debugging(char *frmt_str, char *file, char *line, char *err) {
#ifdef DEBUG
    UARTprintf(frmt_str, file, line, err);
#endif
}

#define PC 8              /* ADC PIN count */
#define GPIO_PORT_COUNT 2 /* number of GPIO ports used by ADC */


enum {
    PRE_INIT,
    POST_INIT
}; // adc state

struct gpio_port {
    const uint32_t SYSCTL;
    const uint32_t BASE;
}; /* struct for data required for manipulating GPIO ports */

struct gpio_pin {
    const struct gpio_port PORT;
    const uint8_t PIN;
};


/* ADC configuration arguments */
const uint32_t ADC_CLOCK_DIV = 1;
const uint32_t ADC_CLOCK_CONFIG = ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;
const uint32_t SEQUENCE_NUM = 0;
const uint32_t ADC_SC_MODULE = SYSCTL_PERIPH_ADC0;


#ifdef PART_TM4C123GH6PM
const uint32_t ADC_REFERENCE = ADC_REF_INT;

const struct gpio_pin GPIO_LUT[PC] = {
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
const uint32_t ADC_REFERENCE = ADC_REF_EXT_3V;

const struct gpio_pin GPIO_LUT[PC] = {
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_3},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_2},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_1},
    {{SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE}, GPIO_PIN_0},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_7},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_6},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_5},
    {{SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE}, GPIO_PIN_4}};
#endif


int status = PRE_INIT;
uint8_t active_pins = 0;
uint32_t *adc_buffer = (void *)0;
void (*adc_callback)(void) = (void *)0;


/* If a module is not already enabled: enables it, then waits until it
 * is responding as ready. */
void init_module(uint32_t sysctl_module) {
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        SysCtlPeripheralEnable(sysctl_module);
        while (SysCtlPeripheralReady(sysctl_module) == false)
            ;
    }
}


void adc_irq_handler(void) {
    /* Clear interrupt flag so we don't instantly return to the handler */
    ADCIntClear(ADC0_BASE, SEQUENCE_NUM);

    ADCSequenceDataGet(ADC0_BASE, SEQUENCE_NUM, adc_buffer);

    adc_callback();
}

enum adc_return adc_init_pins(enum adc_pin *pins, uint8_t num_pins, bool interrupt_mode) {
        adc_assert(status == PRE_INIT);
        adc_assert(num_pins < 9);
        adc_assert(pins != (void *)0);

        active_pins = num_pins;

        init_module(ADC_SC_MODULE);
        ADCIntDisable(ADC0_BASE, SEQUENCE_NUM);

        ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_CONFIG, ADC_CLOCK_DIV);
        ADCReferenceSet(ADC0_BASE, ADC_REFERENCE);

        for (int i = 0; i < num_pins; i++) {
            /* initialise GPIO module */
            init_module(GPIO_LUT[pins[i]].PORT.SYSCTL);

            GPIOPinTypeADC(GPIO_LUT[pins[i]].PORT.BASE, GPIO_LUT[pins[i]].PIN);
        }

        /* disable before configuring sequencer */
        ADCSequenceDisable(ADC0_BASE, SEQUENCE_NUM);

        /* setup ADC sample sequencer, trigger manually in software */
        ADCSequenceConfigure(ADC0_BASE, SEQUENCE_NUM, ADC_TRIGGER_PROCESSOR, 0);

        /* configure each step of sequence to be an analog input source */
        for (int i = 0; i < num_pins-1; i++) {
            ADCSequenceStepConfigure(ADC0_BASE, SEQUENCE_NUM, i, pins[i]);
        }

        /* last step is end of sequence, generate an interrupt here */
        ADCSequenceStepConfigure(ADC0_BASE, SEQUENCE_NUM, num_pins, pins[num_pins] | ADC_CTL_IE | ADC_CTL_END);

        ADCSequenceEnable(ADC0_BASE, SEQUENCE_NUM);

        /* enable interrupts if in interrupt mode */
        if (interrupt_mode) {
            ADCIntEnable(ADC0_BASE, SEQUENCE_NUM);
            IntEnable(INT_ADC0SS0);
        }
        /* ready for capture */
        status = POST_INIT;
        return ADC_SUCCESS;
}

enum adc_return adc_capture_interrupt(uint32_t *buffer, void (*callback)(void)) {
    adc_assert(status == POST_INIT);
    adc_assert(buffer != (void *)0);
    adc_assert(callback !=(void *)0);

    adc_buffer = buffer;
    adc_callback = callback;
    ADCProcessorTrigger(ADC0_BASE, SEQUENCE_NUM);

    return ADC_SUCCESS;
}


uint32_t adc_capture_polling(uint32_t *buffer) {
    adc_assert(status == POST_INIT);

    ADCProcessorTrigger(ADC0_BASE, SEQUENCE_NUM);

    // Waits until a sample has been captured.
    while (!ADCIntStatus(ADC0_BASE, SEQUENCE_NUM, false)) {};

    uint32_t num_samples = ADCSequenceDataGet(ADC0_BASE, SEQUENCE_NUM, buffer);

    return num_samples;
}


enum adc_return adc_interrupt_disable() {
    ADCIntDisable(ADC0_BASE, SEQUENCE_NUM);
    IntDisable(INT_ADC0SS0);

    return ADC_SUCCESS;
}

enum adc_return adc_interrupt_enable() {
    ADCIntEnable(ADC0_BASE, SEQUENCE_NUM);
    IntEnable(INT_ADC0SS0);

    return ADC_SUCCESS;
}
