#include "boilerplate.h"
#include "rtos-kochab.h"
#include "science-mod/LIS3MDL.h"
#include "science-mod/SI7021.h"
#include "science-mod/TCS34725.h"

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
    UARTprintf("Entered science test task\n");
    bool success = false;
    LIS3MDL lis3mdl(I2C0);
    success = lis3mdl.init();
    if (success) {
        UARTprintf("LIS3MDL successfully initialised\n");
    } else {
        UARTprintf("LIS3MDL failed to initialise\n");
    }
    TCS34725 tcs34725(I2C0);
    success = tcs34725.init();
    if (success) {
        UARTprintf("TCS34725 successfully initialised\n");
    } else {
        UARTprintf("TCS34725 failed to initialise\n");
    }
    uint8_t it = tcs34725.read8(TCS34725_ATIME);    
    UARTprintf("Check integration time set %d\n", it);
    uint8_t gain = tcs34725.read8(TCS34725_CONTROL); 
    UARTprintf("Check gain set %d\n", gain); 
    
    SI7021 si7021(I2C0);    
    success = si7021.init();
    if (success) {
        UARTprintf("SI7021 successfully initialised\n");
    } else {
        UARTprintf("SI7021 failed to initialise\n");
    }
    UARTprintf("Checking SI7021 serial number\n");
    uint32_t ser_hi = 0;
    uint32_t ser_lo = 0;
    si7021.read_serial_number(&ser_hi, &ser_lo);
    UARTprintf("ser = %x %x\n", ser_hi, ser_lo);
    uint16_t r, g, b, c;
    uint16_t colour_temp, illuminance;  
    uint32_t temp, humidity;
    float mx, my, mz;
    char mx_buf[10];
    char my_buf[10];
    char mz_buf[10];
    while (1) {
        lis3mdl.read_magnetism(&mx, &my, &mz);
        ftoa(mx, mx_buf);
        ftoa(my, my_buf);
        ftoa(mz, mz_buf);
        UARTprintf("Scaled magnetism: x = %s y = %s z = %s gauss\n", mx_buf, my_buf, mz_buf);

        temp = si7021.read_temperature();
        UARTprintf("Temperature = %d degrees Celsius\n", temp);
        humidity = si7021.read_humidity();
        UARTprintf("Humidity = %d percent\n", humidity);

        tcs34725.read_raw_data(&r, &g, &b, &c);
        UARTprintf("Raw light sensor values: r=%d, g=%d, b=%d, c=%d\n", r, g, b, c);
        colour_temp = tcs34725.calculate_colour_temperature(r, g, b);
        UARTprintf("Colour temperature = %d Kelvin\n", colour_temp);
        illuminance = tcs34725.calculate_lux(r, g, b);
        UARTprintf("Illuminance = %d lux\n", illuminance);
  
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
