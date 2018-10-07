#include "rtos-kochab.h"

#include "boilerplate.h"

class ParentThing {
    public:
        ParentThing();
        virtual int virtual_derp() = 0;
};

class Thing : public ParentThing {
    public:
        Thing();
        int derp();
        virtual int virtual_derp();

    private:
        int a;
};

ParentThing::ParentThing() {
    UARTprintf("ccc");
}

Thing::Thing() : ParentThing() {
    UARTprintf("aaa\n");
};

int Thing::derp() {
    UARTprintf("Derp\n");
    a = 2;
    return 0;
}

int Thing::virtual_derp()  {
    UARTprintf("Hello Derp\n");
    return 9;
};


#define SYSTICKS_PER_SECOND     100

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define ALL_LEDS (RED_LED|BLUE_LED|GREEN_LED)

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

extern "C" void task_blink_fn(void) {

    Thing a_thing;
    UARTprintf("Entered blinky task\n");

    // Enable the GPIO pin for the LEDs (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|GREEN_LED|BLUE_LED);

    UARTprintf("Initialized GPIOS\n");

    // Loop forever.
    while(1) {
        a_thing.derp();
        a_thing.virtual_derp();
        dynamic_cast<ParentThing*>(&a_thing)->virtual_derp();
        ((ParentThing*)&(a_thing))->virtual_derp();
        // Turn off all but the red LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, RED_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Turn off all but the green LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, GREEN_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Turn off all but the blue LED.
        GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, BLUE_LED);

        rtos_signal_wait( RTOS_SIGNAL_ID_BLINK_DELAY );

        // Print a dot so that we look alive on the console
        UARTprintf(".");
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

    Thing t;
    t.derp();

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
