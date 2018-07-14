#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define SIZE_OF_BYTE 8
#define NUM_DATA_BITS 24

HX711::HX711(port_t dout_port, pinNum_t dout_pin, port_t sck_port, pinNum_t sck_pin) {
    DOUT_PORT = dout_port;
    DOUT_PIN = dout_pin;
    SCK_PORT = sck_port;
    SCK_PIN = sck_pin;
    scale = 1;
}

void HX711::init(void) {
    GPIOPinTypeGPIOInput(DOUT_PORT, DOUT_PIN);
    GPIOPinTypeGPIOOutput(SCK_PORT, SCK_PIN);
    set_gain(CHANNEL_A_128);
}

void HX711::init(gain_t gain) {
    GPIOPinTypeGPIOInput(DOUT_PORT, DOUT_PIN);
    GPIOPinTypeGPIOOutput(SCK_PORT, SCK_PIN);
    set_gain(gain);
}

void HX711::set_gain(gain_t gain) {
    switch(gain) {
        case CHANNEL_A_128:
            GAIN_CYCLES = 1;
            break;
        case CHANNEL_A_64:
            GAIN_CYCLES = 2;
            break;
        case CHANNEL_B_32:
            GAIN_CYCLES = 3;
            break;
    }
    GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
    read(); // update gain conversion
}


bool HX711::is_ready(void) {
    // if high, data is not ready for retrieval
    return (GPIOPinRead(DOUT_PORT, DOUT_PIN) == 0);
}

int32_t HX711::read(void) {
    while(!is_ready()); // poll until the device is ready
    /*
	uint8_t data[3] = {0};
	uint8_t filler = 0x00;
    
	// pulse the clock pin 24 times to read the data
	data[2] = read_byte();
	data[1] = read_byte();;
	data[0] = read_byte();;
	
	for (unsigned int i = 0; i < GAIN_CYCLES; i++) {
		GPIOPinWrite(SCK_PORT, SCK_PIN, SCK_PIN);
        SysCtlDelay(25); // approximately 1us delay
        GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
        SysCtlDelay(25); 
	}*/

    int32_t value = 0;
    // read each bit by toggling the clock for a total of 24 bits
    for (uint8_t i = 0; i < NUM_DATA_BITS; i++) {
        GPIOPinWrite(SCK_PORT, SCK_PIN, SCK_PIN);
        SysCtlDelay(25); // approximately 1us delay
        GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
        value <<= 1;
        if (GPIOPinRead(DOUT_PORT, DOUT_PIN)) {
            value++;
        }
        SysCtlDelay(25); 
    }

    // set the channel and the gain factor for the next conversion
    for (uint8_t i = 0; i < GAIN_CYCLES; i++) {
		GPIOPinWrite(SCK_PORT, SCK_PIN, SCK_PIN);
        SysCtlDelay(25); // approximately 1us delay
        GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
        SysCtlDelay(25); 
	}

    if (value & 0x800000) { // if msb is set, then it is negative so extend sign
        value |= (0xFF << NUM_DATA_BITS); 
    }

	return value;
}

int32_t HX711::read_avg(uint8_t num_samples) {
	int32_t sum = 0;
	for (uint8_t i = 0; i < num_samples; i++) {
		sum += read();
	}
	return sum / num_samples;
}

double HX711::read_scaled(void) {
	return (double ) (read() - offset) / scale;
}

double HX711::read_scaled_avg(void) {
	return (double ) (read() - offset) / scale;
}

void HX711::tare(uint8_t num_samples) {
    tare_offset = read_avg(num_samples);
}


int32_t HX711::get_tare_offset(void) {
	return tare_offset;
}

void HX711::set_tare_offset(int32_t offset) {
	return tare_offset;
}

float HX711::get_scale(void) {
	return scale;
}

void HX711::set_scale(float scaling_factor) {
    scale = scaling_factor;
}

/* PD_SCK high time min 0.2  typ 1 max 50 µs
   PD_SCK low time min 0.2 typ 1 µs */
/* Clock speed = 80 MHz */
/*
uint8_t HX711::read_byte(void) {
    uint8_t result = 0;
    uint8_t value;
    // read byte one bit at a time, msb first
    for (int i = 0; i < SIZE_OF_BYTE; i++) {
        // toggle next bit
        GPIOPinWrite(SCK_PORT, SCK_PIN, SCK_PIN);
        SysCtlDelay(25); // approximately 1us delay
        GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
        value = GPIOPinRead(DOUT_PORT, DOUT_PIN);
        result |= (value << (SIZE_OF_BYTE-1-i));
        SysCtlDelay(25);
    }
    return result;
}*/

void HX711::power_down(void) {
    // transition SCK from low to high to power down
    GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
    GPIOPinWrite(SCK_PORT, SCK_PIN, SCK_PIN); 
    // gain defaults to ch A 128 after power down
    set_gain(CHANNEL_A_128);
}

void HX711::power_up(void) {
    GPIOPinWrite(SCK_PORT, SCK_PIN, 0);
}

