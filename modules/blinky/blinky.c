#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "rtos-kochab.h"
#include "debug.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

#define SYSTICKS_PER_SECOND     100

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define ALL_LEDS (RED_LED|BLUE_LED|GREEN_LED)

void fatal(const RtosErrorId error_id) {
    UARTprintf("FATAL ERROR: 0x%x\n", error_id);
    for (;;);
}

bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

void task_blink_fn(void) {

    UARTprintf("Entered blinky task\n");

    // Enable the GPIO pin for the LEDs (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    UARTprintf("Initialized GPIOS\n");

    // Loop forever.
    while(1) {
        // Turn off all but the red LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, RED_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Turn off all but the green LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, GREEN_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Turn off all but the blue LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, BLUE_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );
    }
}

void InitializeUARTStdio(void) {
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}

int main(void) {

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPULazyStackingEnable();

    // Set the clocking to run from the PLL at 50 MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Set up the systick interrupt used by the RTOS
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
