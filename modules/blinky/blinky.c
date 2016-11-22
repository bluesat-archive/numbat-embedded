#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "rtos-kochab.h"
#include "machine-timer.h"
#include "debug.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

bool tick_irq(void);
void fatal(RtosErrorId error_id);
void fn_a(void);
void fn_b(void);

bool tick_irq(void) {
    machine_timer_tick_isr();
    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);
    return true;
}

void fatal(const RtosErrorId error_id) {
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;);
}

void task_blink_fn(void) {
    volatile uint32_t ui32Loop;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Loop forever.
    while(1)
    {
        // Turn on the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++);

        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++);
    }
}

int main(void) {
    machine_timer_start((void (*)(void))tick_irq);
    debug_println("Starting RTOS");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
