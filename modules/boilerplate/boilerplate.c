#include "boilerplate.h"

void nmi() { for(;;); }

void hardfault() { for(;;); }

void memmanage() { for(;;); }

void busfault() { for(;;); }

void usagefault() { for(;;); }

void __error__( char *pcFilename, uint32_t ui32Line ) {
    for(;;);
}

void fatal(const uint8_t error_id) {
    UARTprintf("FATAL ERROR: 0x%x\n", error_id);
    for (;;);
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
