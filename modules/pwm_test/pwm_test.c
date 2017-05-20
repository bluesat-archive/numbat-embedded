#include "boilerplate.h"
#include "rtos-kochab.h"
#include "pwmlib.h"

void task_pwm_test_fn(void) {

    UARTprintf("Entered PWM Test task. Initializing...\n");

    pwm_init(PWM_0);
    pwm_init(PWM_1);
    pwm_init(PWM_2);
    pwm_init(PWM_3);
    pwm_init(PWM_4);
    pwm_init(PWM_5);
    pwm_init(PWM_6);
    pwm_init(PWM_7);

    pwm_set_period(PWM_PAIR_0, 1.0);
    pwm_set_period(PWM_PAIR_1, 1.0);
    pwm_set_period(PWM_PAIR_2, 1.0);
    pwm_set_period(PWM_PAIR_3, 1.0);

    pwm_set_duty(PWM_0, 0.0);
    pwm_set_duty(PWM_1, 100.0 / 7.0);
    pwm_set_duty(PWM_2, 100.0 / 7.0 * 2.0);
    pwm_set_duty(PWM_3, 100.0 / 7.0 * 3.0);
    pwm_set_duty(PWM_4, 100.0 / 7.0 * 4.0);
    pwm_set_duty(PWM_5, 100.0 / 7.0 * 5.0);
    pwm_set_duty(PWM_6, 100.0 / 7.0 * 6.0);
    //pwm_set_duty(PWM_7, 100.0);

    pwm_enable(PWM_0);
    pwm_enable(PWM_1);
    pwm_enable(PWM_2);
    pwm_enable(PWM_3);
    pwm_enable(PWM_4);
    pwm_enable(PWM_5);
    pwm_enable(PWM_6);
    pwm_enable(PWM_7);

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
