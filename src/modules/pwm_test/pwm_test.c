#include "boilerplate.h"
#include "rtos-kochab.h"
#include "pwm.h"

void task_pwm_test_fn(void) {

    UARTprintf("Entered PWM Test task. Initializing...\n");
    
    pwm_init(50);

    pwm_init(PWM0);
    pwm_init(PWM1);
    pwm_init(PWM2);
    pwm_init(PWM3);
    pwm_init(PWM4);
    pwm_init(PWM5);
    pwm_init(PWM6);
    pwm_init(PWM7);

    pwm_set_prescaler(DIV8);
    pwm_set_prescaler(50);

    pwm_set_period(PWM_PAIR0, 10.0);
    pwm_set_period(PWM_PAIR1, 10.0);
    pwm_set_period(PWM_PAIR2, 10.0);
    pwm_set_period(PWM_PAIR3, 10.0);
    pwm_set_period(50, 10.0);
    pwm_set_period(PWM_PAIR0, 200.0);

    pwm_get_period(PWM_PAIR0);

    pwm_set_duty(PWM0, 33.3);
    pwm_set_duty(PWM1, 66.6);

    pwm_set_duty(PWM2, 100.0);
    pwm_set_duty(PWM2, 0.0);

    pwm_set_duty(PWM3, 0.0);
    pwm_set_duty(PWM3, 100.0);

    pwm_set_duty(PWM4, 25.0);
    pwm_set_duty(PWM4, 75.0);

    pwm_set_duty(PWM5, 75.0);
    pwm_set_duty(PWM5, 25.0);

    pwm_set_duty(PWM6, 99.9);
    pwm_set_duty(PWM7, 0.1);

    pwm_set_duty(50, 50.0);
    pwm_set_duty(PWM0, 200.0);

    for (int i = PWM0; i <= PWM7; i++) {
        double duty = pwm_get_duty(i);
        char *dutyc = (char *)(&duty);
        UARTprintf("duty%d: ", i);
        for (int j = sizeof(double) - 1; j >= 0; j--) {
            UARTprintf("%x", dutyc[j]);
        }
        UARTprintf("\n");
    }

    pwm_enable(PWM0);
    pwm_enable(PWM1);
    pwm_enable(PWM2);
    pwm_enable(PWM3);
    pwm_enable(PWM4);
    pwm_enable(PWM5);
    pwm_enable(PWM6);
    pwm_enable(PWM7);
    pwm_enable(40);

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
