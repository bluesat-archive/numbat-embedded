#include "boilerplate.h"

extern "C" void nmi() { for(;;); }

extern "C" void hardfault() { for(;;); }

extern "C" void memmanage() { for(;;); }

extern "C" void busfault() { for(;;); }

extern "C" void usagefault() { for(;;); }

extern "C" void __error__( char *pcFilename, uint32_t ui32Line ) {
    for(;;);
}

extern "C" void fatal(const uint8_t error_id) {
    UARTprintf("FATAL ERROR: 0x%x\n", error_id);
    for (;;);
}

/*class Test_Class {
    public:
        Test_Class() {

        }
};*/

void InitializeFPU(void) {

    // Magic taken from vector tables from TI examples
    // This turns on the FPU and initializes it.
    HWREG(NVIC_CPAC) = ((HWREG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                        NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPULazyStackingEnable();
}

void InitializeUARTStdio(void) {
    //Test_Class();
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
