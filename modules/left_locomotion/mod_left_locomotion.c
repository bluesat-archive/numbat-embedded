#include "pwm.h"
#include "boilerplate.h"
#include "rtos-kochab.h"


int main(void) {
  
    // Create the subscribers
    std_msgs::float64 front_left_drive_buffer_in[5];
    Subscriber<std_msgs::float64> frontLeftDriveSub("topic name", front_left_drive_buffer_in, 5, frontLeftDriveCallback);
    std_msgs::float64 front_left_rotate_buffer_in[5];
    Subscriber<std_msgs::float64> frontLeftRotateSub("topic name", front_left_rotate_buffer_in, 5, frontLeftRotateCallback);
    std_msgs::float64 back_left_drive_buffer_in[5];
    Subscriber<std_msgs::float64> backLeftDriveSub("topic name", back_left_drive_buffer_in, 5, backLeftDriveCallback);
    std_msgs::float64 back_left_rotate_buffer_in[5];
    Subscriber<std_msgs::float64> backLeftRotateSub("topic name", back_left_rotate_buffer_in, 5, backLeftRotateCallback);

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