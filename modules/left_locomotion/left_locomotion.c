// Module for left locomotion of the NUMBAT rover
// Author: Elliott Smith
// Date: 2/4/18


#include "pwm.h"
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "std_msgs::Float64.h"

#define frontLeftDrivePin PWM0
#define frontLeftRotatePin PWM1
#define backLeftDrivePin PWM2
#define backLeftRotatePin PWM3

#define maxSpeed 3 // max speed in m/s

int main(void) {
  
    // Create the subscribers
    std_msgs::Float64 front_left_drive_buffer_in[5];
    Subscriber<std_msgs::Float64> frontLeftDriveSub("front_left_wheel_axel_controller/command", front_left_drive_buffer_in, 5, frontLeftDriveCallback);
    std_msgs::Float64 front_left_rotate_buffer_in[5];
    Subscriber<std_msgs::Float64> frontLeftRotateSub("front_left_swerve_controller/command", front_left_rotate_buffer_in, 5, frontLeftRotateCallback);
    std_msgs::Float64 back_left_drive_buffer_in[5];
    Subscriber<std_msgs::Float64> backLeftDriveSub("back_left_wheel_axel_controller/command", back_left_drive_buffer_in, 5, backLeftDriveCallback);
    std_msgs::Float64 back_left_rotate_buffer_in[5];
    Subscriber<std_msgs::Float64> backLeftRotateSub("back_left_swerve_controller/command", back_left_rotate_buffer_in, 5, backLeftRotateCallback);

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

void frontLeftDriveCallback(const std_msgs::Float64 & msg) {
  pwm_init(frontLeftDrivePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR0,20)
  float duty = (msg/maxSpeed)*2;
  pwm_set_duty(frontLeftDrivePin,duty);
}
  

void frontLeftRotateCallback(const std_msgs::Float64 & msg) {
  pwm_init(fronLeftRotatePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR0,20);
  float duty = ((msg+PI)/2*PI)*1.8 + 0.6;
  pwm_set_duty(frontLeftRotatePin,duty);
}
    
    
void backLeftDriveCallback(const std_msgs::Float64 & msg) {
  pwm_init(backLeftDrivePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR1,20);
  float duty = (msg/maxSpeed)*2;
  pwm_set_duty(backLeftDrivePin,duty);
}
    
void backLeftRotateCallback(const std_msgs::Float64 & msg) {
  pwm_init(backLeftRotatePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR1,20);
  float duty = ((msg+PI)/2*PI)*1.8 + 0.6;
  pwm_set_duty(backLeftRotatePin,duty);
}