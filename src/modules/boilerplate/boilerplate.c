#include "boilerplate.h"
#define CAN_BITRATE 500000



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


/**
 * CPP Handlers
 * TODO: move these to their own file
 */
void __cxa_pure_virtual() {
    UARTprintf("Bad call to pure virtual!\n");
    fatal(1);
}
void exit() { fatal(1); }
//this is called when C++ wants to run deconstructors on exit
int __aeabi_atexit (void *arg, void (*fun) (void *), void *d) {
    UARTprintf("Attempt to add exist function!\n");
    //TODO: we should probably call these on halt, but we currently don't have functionality to halt so hey...
    return 0;
}

void *__dso_handle;

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
//    UARTStdioConfig(0, 57600, 16000000);
#ifdef UART_BUFFERED
    UARTEchoSet(false);
#endif
}

void uart0_int_handler(void) {
#ifdef UART_BUFFERED
    UARTStdioIntHandler();
#endif
}

// apparently memset is missing
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58203
void * memset(void * dst, int c, size_t n) {//__attribute__((used)) {
    uint8_t * ptr = (uint8_t*) dst;
    for(int i =0; i < n; ++i) {
        ptr[i] = c;
    }
    return ptr;
}

void init_can_common(void) {
    // We enable GPIO E - E4 for RX and E5 for TX
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // enables the can function we have just configured on those pins
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //enable and initalise CAN0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);

    //TODO: change this to use the eChronos clock
    // Set the bitrate for the CAN BUS. It uses the system clock
    CANBitRateSet(CAN0_BASE, ROM_SysCtlClockGet(), CAN_BITRATE);

    // enable can interupts
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR); //| CAN_INT_STATUS);
    IntEnable(INT_CAN0);

    //start CAN
    CANEnable(CAN0_BASE);

}
