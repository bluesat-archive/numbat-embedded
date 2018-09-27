#include "boilerplate.h"
#include "rtos-kochab.h"
#include "pid.h"
#include <string.h>

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

double atof(char* num) {
    if (!num || !*num)
        return 0; 

    bool neg = false;
    double integerPart = 0;
    double fractionPart = 0;
    int divisorForFraction = 1;
    bool inFraction = false;

    while (*num != '\0') {
        if (*num >= '0' && *num <= '9') {
            if (inFraction) {
                /*See how are we converting a character to integer*/
                fractionPart = fractionPart*10 + (*num - '0');
                divisorForFraction *= 10;
            } else {
                integerPart = integerPart*10 + (*num - '0');
            }
        } else if (*num == '-') {
            neg = true;
        } else if (*num == '.') {
            if (inFraction) {
                return (integerPart + fractionPart/divisorForFraction);
            } else {
                inFraction = true;
            }
        } else {
            integerPart = 0;
            fractionPart = 0;
        }
        ++num;
    }

    double ret = integerPart + fractionPart/divisorForFraction;

    if (neg) {
        return -1.0 * ret;
    } else {
        return ret;
    }
}

double output = 0;
double setpoint = 0;
double voltage = 0;
PID *pid_ptr;

extern "C" void task_pid_test_fn(void) {
    double Kp = 1.2;
    double Ki = 0.3;
    double Kd = 0.05;
    double Ts = 0.01;
    double N = 10;
    PID pid(&output, &setpoint, &voltage, Ts, Kp, Ki, Kd, N);
    pid_ptr = &pid;
    pid.set_saturation_limits(-10.0, 10.0);
    rtos_timer_signal_set(RTOS_TIMER_ID_PID, RTOS_TASK_ID_PID, RTOS_SIGNAL_SET);
    rtos_timer_reload_set(RTOS_TIMER_ID_PID, 10);
    rtos_timer_enable(RTOS_TIMER_ID_PID);
    while(true) {
        pid.compute_output();
        pwm_write(voltage_to_pwm(output));
        
        // rtos_sleep(1);
        rtos_signal_wait(RTOS_SIGNAL_ID_PID);
    }
}

extern "C" void task_uart_receive_fn(void) {
    char val[10];
    UARTprintf("Usage: <Kp|Ki|Kd|N> <new value (float)>\n");
    while(true) {
        UARTprintf("Enter a PID parameter to change and the updated value:"); 
        UARTgets(val, 10);
        // returns zero if both partial strings match
        double new_val = atof(val);
        char param ;
        if (strncmp(val, "Kp", 2) == 0) {
            pid_ptr->set_Kp(new_val);
            UARTprintf("Set new Kp = %s\n", &val[4]);
        } else if (strncmp(val, "Ki", 2) == 0) {
            pid_ptr->set_Ki(new_val);
            UARTprintf("Set new Ki = %s\n", &val[4]);
        } else if (strncmp(val, "Kd", 2) == 0) {
            pid_ptr->set_Kd(new_val);
            UARTprintf("Set new Kd = %s\n", &val[4]);
        } else if (strncmp(val, "N", 1) == 0) {
            pid_ptr->set_N(new_val);
            UARTprintf("Set new N = %s\n", &val[3]);
        }
        
    }
}

void voltage_to_pwm(double voltage) {
    
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
