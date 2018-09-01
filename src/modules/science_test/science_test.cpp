#include "boilerplate.h"
#include "rtos-kochab.h"
#include "science-mod/LIS3MDL.h"
#include "science-mod/SI7021.h"
#include "science-mod/TCS34725.h"
#include "science-mod/HX711.h"
#include "TCA9548A.h"
#include "adc.h"

#define NUM_PINS 1 

void ftoa(float f,char *buf) {
    int pos=0,ix,dp,num;
    if (f<0) {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    while (f>=10.0) {
        f=f/10.0;
        dp++;
    }
    for (ix=1;ix<8;ix++) {
            num = (int)f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }
}

extern "C" void task_science_test_fn(void) {
    HX711 hx711(PORTA, PIN_7, PORTA, PIN_6);
    hx711.init(); // init to default gain = 128
    UARTprintf("HX711 initialised\n");
    hx711.tare(50);
    UARTprintf("Zero out HX711 with 20 samples\n");
    
    float scale = -67.29; // change this to scale raw value to appropriate units
    hx711.set_scale(scale); 
    UARTprintf("HX711 scaling factor set to %d\n", (int) scale); 
    // 47184    -942
    int32_t weight_raw;
    float weight;
    char weight_buf[10];
    while (1) {
        weight_raw = hx711.read();
        UARTprintf("Raw weight value = %d\n", weight_raw);
        weight_raw = hx711.read_avg(50);
        UARTprintf("Raw weight value averaged (10 samples) = %d\n", weight_raw);
        weight = (float) hx711.read_scaled_avg(50);
        ftoa(weight, weight_buf);
        UARTprintf("Tare calibrated and scaled weight (10 samples) = %s\n", weight_buf);
        // delay 1 s
        for (uint32_t i=0; i < 1000; i++) {
            SysCtlDelay(25000); // 1 ms delay
        }
    }                           
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
