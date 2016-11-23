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

#define SYSTICKS_PER_SECOND     100

void fatal(const RtosErrorId error_id) {
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;);
}

bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

void task_blink_fn(void) {

    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Loop forever.
    while(1) {
        // Turn on the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

        // Delay for a bit.
        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

        // Delay for a bit.
        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );
    }
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

    // Actually start the RTOS
    debug_println("Starting RTOS");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
