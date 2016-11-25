#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

#define SYSTICKS_PER_SECOND     2

void nmi() { for(;;); }
void hardfault() { for(;;); }
void memmanage() { for(;;); }
void busfault() { for(;;); }
void usagefault() { for(;;); }

void __error__( char *pcFilename, uint32_t ui32Line ) {
    for(;;);
}

void fatal(const RtosErrorId error_id) {
    UARTprintf("FATAL ERROR: 0x%x\n", error_id);
    for (;;);
}

bool tick_irq(void) {
    rtos_timer_tick();
    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);
    return true;
}

void fn_a(void) {
    uint8_t count;

    UARTprintf("task a: taking lock\n");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);

    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_M0)) {
        UARTprintf("unexpected mutex not locked.\n");
    }

    UARTprintf("task a: releasing lock\n");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    for (count = 0; count < 10; count++) {
        UARTprintf("task a\n");
        if (count % 5 == 0) {
            UARTprintf("unblocking b\n");
            rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
        }
    }

    UARTprintf("A now waiting for ticks\n");

    for (;;) {
        (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TIMER);
        UARTprintf("tick\n");
        rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
    }
}

void fn_b(void) {
    UARTprintf("task b: attempting lock\n");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    UARTprintf("task b: got lock\n");

    while (1) {
        if (rtos_signal_poll_set(RTOS_SIGNAL_SET_TEST)) {
            UARTprintf("task b blocking\n");
            (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TEST);
            UARTprintf("task b unblocked\n");
        }
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
