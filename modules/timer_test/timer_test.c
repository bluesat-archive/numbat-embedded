#include "boilerplate.h"
#include "rtos-kochab.h"

#define SYSTICKS_PER_SECOND     100

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define ALL_LEDS (RED_LED|BLUE_LED|GREEN_LED)

bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

void task_delay_fn(void) {

    UARTprintf("Entered delay task\n");

    // Initialize timer 0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC_UP);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, 500e6);
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

    // Loop forever.
    while(1) {
        rtos_signal_wait( RTOS_SIGNAL_ID_SECOND_DELAY );

        uint32_t timer_value = ROM_TimerValueGet(TIMER0_BASE, TIMER_A)/50;

        // Print the hardware timer value
        UARTprintf("TIMER0: A: %u.%u\n", timer_value/1000000, timer_value % 1000000);
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

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
