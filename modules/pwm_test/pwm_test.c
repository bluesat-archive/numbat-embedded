#include "boilerplate.h"
#include "rtos-kochab.h"
#include "pwmlib.h"

void task_pwm_test_fn(void) {

    UARTprintf("Entered PWM Test task. Initializing...\n");

    pwm_init(PWM_0);
    pwm_set_period(PWM_PAIR_0, 1.0);
    pwm_set_duty(PWM_0, 50.0);
    pwm_enable(PWM_0);

    UARTprintf("PWM initialisation complete...\n");
    
    while (1);                           
}

int main(void) {

    // Initialize the floating-point unit.
    InitializeFPU();

    // Set the clocking to run from the PLL at 50 MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
