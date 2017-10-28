#include "boilerplate.h"
#include "rtos-kochab.h"

#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

uint32_t adc_val[8];
uint32_t irqd;

void adc_irq(void) {
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, adc_val);
    irqd = 1;
}

void task_adc_test_fn(void) {

    UARTprintf("Entered ADC Test task. Initializing...\n");

    // TEST CODE GOES HERE
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
        ;

    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_EIGHTH, 1);
    ADCHardwareOversampleConfigure(ADC0_BASE, 64);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
        ;

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_ANALOG);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    
    ADCIntEnableEx(ADC0_BASE, ADC_INT_SS0);
    ADCIntEnable(ADC0_BASE, 0);

    ADCSequenceEnable(ADC0_BASE, 0);

    IntEnable(INT_ADC0SS0);

    UARTprintf("ADC initialisation complete...\n");
    
    uint32_t prev = 0;
    while (1) {
        if (adc_val[0] != prev)
            UARTprintf("%d\n", adc_val[0]);
        prev = adc_val[0];
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
