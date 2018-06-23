#include "boilerplate.h"
#include "rtos-kochab.h"

#include "adc.h"
    
#define NUM_PINS 8

uint32_t out[NUM_PINS] = {0};
bool capture = true;

extern "C" void adc_callback_fn(void) {
    capture = true;
}

extern "C" void task_adc_test_fn(void) {
    UARTprintf("Entered ADC Test task. Initializing...\n");

    enum adc_pin pins[NUM_PINS] = {AIN0, AIN1, AIN2, AIN3, AIN4, AIN5, AIN6, AIN7};
    adc_init_pins(pins, NUM_PINS);

    while(1) {
        if (capture) {
            capture = false;
            adc_capture_interrupt(out, adc_callback_fn);
        }
    }
}

int main(void) {

    // Initialize the floating-point unit.
    InitializeFPU();

    // Set the clocking to run from the PLL at 50 MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
