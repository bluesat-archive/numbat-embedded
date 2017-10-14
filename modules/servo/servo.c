#include "boilerplate.h"
#include "rtos-kochab.h"
#include "pwmlib.h"
  

double atof(char* num)
 {
     if (!num || !*num)
         return 0; 
     double integerPart = 0;
     double fractionPart = 0;
     int divisorForFraction = 1;
     bool inFraction = false;

     while (*num != '\0')
     {
         if (*num >= '0' && *num <= '9')
         {
             if (inFraction)
             {
                 /*See how are we converting a character to integer*/
                 fractionPart = fractionPart*10 + (*num - '0');
                 divisorForFraction *= 10;
             }
             else
             {
                 integerPart = integerPart*10 + (*num - '0');
             }
         }
         else if (*num == '.')
         {
             if (inFraction) {

                 UARTprintf("%d %d %d\n", integerPart, fractionPart, divisorForFraction);
                 return (integerPart + fractionPart/divisorForFraction);
		}
             else
                 inFraction = true;
         }
         else
         {
             UARTprintf("%d %d %d\n", integerPart, fractionPart, divisorForFraction);
             return (integerPart + fractionPart/divisorForFraction);
         }
         ++num;
     }

     UARTprintf("%d %d %d\n", (int)(*(double*)(&integerPart)), (int)(*(double*)(&fractionPart)), (int)(*(double*)(&divisorForFraction)));
     return (integerPart + fractionPart/divisorForFraction);
 }

void task_pwm_test_fn(void) {

    UARTprintf("Entered Servo Test task. Initializing...\n");

    pwm_init(PWM_0);

    while(true) {
    UARTprintf("Enter duty > "); 
    char dutyc[5];
    UARTgets(dutyc, 5);
    float duty = atof(dutyc); 

    pwm_set_period(PWM_PAIR_0, 10.0);

    pwm_set_duty(PWM_0, duty);

    pwm_enable(PWM_0);
    }

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
