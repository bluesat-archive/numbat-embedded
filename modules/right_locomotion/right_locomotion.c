// Module for right locomotion of the NUMBAT rover
// Author: Elliott Smith
// Date: 2/4/18


#include "pwm.h"
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "std_msgs::Float64.h"

#define frontRightDrivePin PWM0
#define frontRightRotatePin PWM1
#define backRightDrivePin PWM2
#define backRightRotatePin PWM3

#define maxSpeed 3 // max speed in m/s

int main(void) {

    // Create the subscribers
    std_msgs::Float64 front_right_drive_buffer_in[5];
    Subscriber<std_msgs::Float64> frontRightDriveSub("front_right_wheel_axel_controller/command", front_right_drive_buffer_in, 5, frontRightDriveCallback);
    std_msgs::Float64 front_right_rotate_buffer_in[5];
    Subscriber<std_msgs::Float64> frontRightRotateSub("front_right_swerve_controller/command", front_right_rotate_buffer_in, 5, frontRightRotateCallback);
    std_msgs::Float64 back_right_drive_buffer_in[5];
    Subscriber<std_msgs::Float64> backRightDriveSub("back_right_wheel_axel_controller/command", back_right_drive_buffer_in, 5, backRightDriveCallback);
    std_msgs::Float64 back_right_rotate_buffer_in[5];
    Subscriber<std_msgs::Float64> backRightRotateSub("back_right_swerve_controller/command", back_right_rotate_buffer_in, 5, backRightRotateCallback);

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

void frontRightDriveCallback(const std_msgs::Float64 & msg) {
  pwm_init(frontRightDrivePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR0,20)
  float duty = (msg/maxSpeed)*2;
  pwm_set_duty(frontRightDrivePin,duty);
}


void frontRightRotateCallback(const std_msgs::Float64 & msg) {
  pwm_init(fronRightRotatePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR0,20);
  float duty = ((msg+PI)/2*PI)*1.8 + 0.6;
  pwm_set_duty(frontRightRotatePin,duty);
}


void backRightDriveCallback(const std_msgs::Float64 & msg) {
  pwm_init(backRightDrivePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR1,20);
  float duty = (msg/maxSpeed)*2;
  pwm_set_duty(backRightDrivePin,duty);
}

void backRightRotateCallback(const std_msgs::Float64 & msg) {
  pwm_init(backRightRotatePin);
  pwm_set_prescaler(DIV64);
  pwm_set_period(PWM_PAIR1,20);
  float duty = ((msg+PI)/2*PI)*1.8 + 0.6;
  pwm_set_duty(backRightRotatePin,duty);
}
