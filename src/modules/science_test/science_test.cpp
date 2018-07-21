#include "boilerplate.h"
#include "rtos-kochab.h"
#include "LIS3MDL.h"
#include "SI7021.h"
#include "TCS34725.h"

void task_science_test_fn(void) {

    UARTprintf("Entered science test task\n");
    TCS34725 tcs34725(I2C0);
    LIS3MDL lis3mdl(I2C0);
    SI7021 si7021(I2C0);
    UARTprintf("Initialising tcs34725\n");
    
    uint8_t it = tcs34725.read8(TCS34725_ATIME); 
    UARTprintf("Check integration time set\n");
    assert (it == TCS34725_INTEGRATIONTIME_2_4MS);
    uint8_t gain = tcs34725.read8(TCS34725_CONTROL); 
    UARTprintf("Check gain set\n");
    assert (gain == TCS34725_GAIN_1X);
    UARTprintf("Initialising lis3mdl\n");
    bool result = lis3mdl.init();
    assert (result == true);
    lis3mdl.enable_default();
    UARTprintf("Initialising SI7021\n");
    si7021.init();
    UARTprintf("Checking SI7021 serial number\n");
    uint32_t ser_hi, ser_lo;
    si7021.read_serial_number(&ser_hi, &ser_lo);
    assert((ser_lo & (0xFF << 24)) == 0x15);
    UARTprintf("Devices initialised successfully\n");
    uint32_t temp, humidity;
    float mx, my, mz;
    uint16_t r, g, b, c;
    uint16_t colour_temp, illuminance;

    while (1) {
        temp = si7021.read_temperature();
        UARTprintf("Temperature = %d Kelvin\n", temp);
        humidity = si7021.read_humidity();
        UARTprintf("Humidity = %d\n", humidity);
        lis3mdl.read_magnetism(&mx, &my, &mz);
        UARTprintf("Magnetic strength: x=%d, y=%d, z=%d gauss\n", (int) mx, (int) my, (int) mz);
        tcs34725.read_raw_data(&r, &g, &b, &c);
        UARTprintf("Raw light sensor values: r=%d, g=%d, b=%d, c=%d\n", r, g, b, c);
        colour_temp = tcs34725.calculate_colour_temperature(r, g, b);
        UARTprintf("Colour temperature = %d Kelvin\n", colour_temp);
        illuminance = tcs34725.calculate_lux(r, g, b);
        UARTprintf("Illuminance = %d lux\n", illuminance);
        rtos_sleep(2000);
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
