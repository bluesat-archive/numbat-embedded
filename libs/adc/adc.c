//------------------------------INCLUDES------------------------------//
#include "adc.h"

//---------------------------ERROR_HANDLING---------------------------//
#define c_assert(e)    ((e) ? (true) : (tst_debugging(\
    "%s,%d: assertion '%s' failed\n", __FILE__, __LINE__, #e), false)) 
#define adc_assert(e)    if (c_assert(e) == false) return ADC_FAILURE

void tst_debugging(char *frmt_str, char *file, int line, char *err) {
#ifdef DEBUG
#include "utils/uartstdio.h"
    UARTprintf(frmt_str, file, line, err);
#endif
}

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
static const uint32_t adc_clock_div = 1;
static const uint32_t adc_clock_config = 
    ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL;
static const uint32_t sequence_num = 0;
static const uint32_t adc_sc_module = SYSCTL_PERIPH_ADC0;


//----------------------BOARD_SPECIFIC_CONSTANTS----------------------//
#ifdef PART_TM4C123GH6PM
static const uint32_t adc_reference = ADC_REF_INT;
    
static const struct gpio_pin gpio_lut[PC] = {
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
static const uint32_t adc_reference = ADC_REF_EXT_3V;

static const struct gpio_pin gpio_lut[PC] = {
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
static int status = PRE_INIT;
static uint8_t active_pins = 0;
static uint32_t *adc_buffer = (void *)0;
static void (*adc_callback)(void) = (void *)0;


//--------------------------LOCAL_FUNCTIONS---------------------------//
static void adc_irq_handler(void) {
    /* Clears and disables interrupt to allow for polling based usage */
    ADCIntClear(ADC0_BASE, sequence_num);
    ADCIntDisable(ADC0_BASE, sequence_num);

    /* copy captured values into buffer */
    ADCSequenceDataGet(ADC0_BASE, sequence_num, adc_buffer);

    /* call the callback */
    adc_callback();
}

/* If a module is not already enabled: enables it, then waits until it
 * is responding as ready. */
static void init_module(uint32_t sysctl_module) {
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        SysCtlPeripheralEnable(sysctl_module);
        while (SysCtlPeripheralReady(sysctl_module) == false)
            ;
    }
}


//-------------------------EXTERNAL_FUNCTIONS-------------------------//
enum adc_return adc_init_pins(enum adc_pin *pins, uint8_t num_pins) {
        adc_assert(status == PRE_INIT);
        adc_assert(num_pins < 9);
        adc_assert(pins != (void *)0);

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
            GPIOPinTypeADC(gpio_lut[pins[i]].port.base, 
                gpio_lut[pins[i]].pin);
        }

        /* register interrupt handler */
        ADCIntRegister(ADC0_BASE, sequence_num, adc_irq_handler);

        /* setup ADC sample sequencer */
        ADCSequenceConfigure(ADC0_BASE, sequence_num,
            ADC_TRIGGER_PROCESSOR, 0);

        /* ready for capture */
        status = POST_INIT;
        ADCSequenceEnable(ADC0_BASE, sequence_num);

        return ADC_SUCCESS;
}

enum adc_return adc_capture_interrupt(uint32_t *buffer, void (*callback)(void)) {
    adc_assert(status == POST_INIT);
    adc_assert(buffer != (void *)0);
    adc_assert(callback !=(void *)0);

    adc_buffer = buffer;
    adc_callback = callback;

    ADCIntEnable(ADC0_BASE, sequence_num);
    ADCProcessorTrigger(ADC0_BASE, sequence_num);

    return ADC_SUCCESS;
}

enum adc_return adc_capture_polling() {
    adc_assert(status == POST_INIT);

    ADCProcessorTrigger(ADC0_BASE, sequence_num);
    
    return ADC_SUCCESS;
}

enum adc_status adc_capture_status() {
    adc_assert(status == POST_INIT);

    if (ADCBusy(ADC0_BASE)) return ADC_BUSY;
    else return ADC_COMPLETE;
}

enum adc_status adc_get_capture(uint32_t *buffer) {
    adc_assert(status == POST_INIT);
    adc_assert(!ADCBusy(ADC0_BASE));

    /* copy captured values into buffer */
    ADCSequenceDataGet(ADC0_BASE, sequence_num, buffer);

    return ADC_SUCCESS;
}
