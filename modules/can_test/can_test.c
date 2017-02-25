#include "boilerplate.h"
#include "rtos-kochab.h"

#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000

bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

void can0_int_handler(void) {

}

void task_can_fn(void) {

    UARTprintf("Entered CAN task. Initializing...\n");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, ROM_SysCtlClockGet(), CAN_BITRATE);

    // Enable interrupts here for faster acknowledgement of TX'd packets
    // (must also update the vector table etc in .prx file)
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    SysCtlIntEnable(INT_CAN0);

    CANEnable(CAN0_BASE);

    UARTprintf("Done. Initialized CAN peripheral on PE4/5\n");

    tCANMsgObject can_message;
    can_message.ui32MsgID = 1; // This is the sender ID
    can_message.ui32MsgIDMask = 0; // Used for ID filtering for RX (not using here)
    can_message.ui32Flags = MSG_OBJ_TX_INT_ENABLE; // TODO: enable TX INT flag for interrupt-driven CAN tx
    unsigned char message_data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    can_message.ui32MsgLen = 4;
    can_message.pui8MsgData = message_data;

    UARTprintf("Starting to send messages...\n");

    while(1) {

        UARTprintf("Sending msg: 0x%02X %02X %02X %02X...\n",
                   can_message.pui8MsgData[0],
                   can_message.pui8MsgData[1],
                   can_message.pui8MsgData[2],
                   can_message.pui8MsgData[3]);

        // Send the CAN message using object number 1 (not the same thing as
        // CAN ID, which is also 1 in this example).  This function will cause
        // the message to be transmitted right away.
        CANMessageSet(CAN0_BASE, 1, &can_message, MSG_OBJ_TYPE_TX);

        // 100 ticks (in .prx) = 1 second delay
        rtos_signal_wait( RTOS_SIGNAL_ID_CAN_DELAY );

        uint32_t ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        UARTprintf("Err: %x\n", ui32Status);
    }
}

int main(void) {

    // Initialize the floating-point unit.
    InitializeFPU();

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
