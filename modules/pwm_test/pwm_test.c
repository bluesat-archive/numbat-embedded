#include "boilerplate.h"
#include "rtos-kochab.h"
#include "driverlib/pwm.h"

void task_pwm_test_fn(void) {

    UARTprintf("Entered PWM Test task. Initializing...\n");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN |
                                          PWM_GEN_MODE_NO_SYNC |
                                          PWM_GEN_MODE_DBG_STOP |
                                          PWM_GEN_MODE_GEN_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 0xFFFF);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0x8FFF);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 1);

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
