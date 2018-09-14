#ifndef HX711_H
#define HX711_H

/**
 * This device is not i2c compliant so it should not share a bus network
 * with other devices
 */

#include <stdbool.h>
#include <stdint.h>

enum port_t {
    /* TM4C123GH6PM only supports up to PORTF */
    PORTA = 0x40004000,  // GPIO Port A
    PORTB = 0x40005000,  // GPIO Port B
    PORTC = 0x40006000,  // GPIO Port C
    PORTD = 0x40007000,  // GPIO Port D
    PORTE = 0x40024000,  // GPIO Port E
    PORTF = 0x40025000,  // GPIO PORT F
    /*
    #ifdef PART_TM4C123GH6PGE
    PORTG = 0x40026000,  // GPIO Port G
    PORTH = 0x40027000,  // GPIO Port H
    PORTJ = 0x4003D000,  // GPIO Port J
    PORTK = 0x40061000,  // GPIO Port K
    PORTL = 0x40062000,  // GPIO Port L
    PORTM = 0x40063000,  // GPIO Port M
    PORTN = 0x40064000,  // GPIO Port N
    PORTP = 0x40065000  // GPIO Port P
    #endif*/
};

enum pinNum_t {
    PIN_0 = 0x00000001,  // GPIO pin 0
    PIN_1 = 0x00000002,  // GPIO pin 1
    PIN_2 = 0x00000004,  // GPIO pin 2
    PIN_3 = 0x00000008,  // GPIO pin 3
    PIN_4 = 0x00000010,  // GPIO pin 4
    PIN_5 = 0x00000020,  // GPIO pin 5
    PIN_6 = 0x00000040,  // GPIO pin 6
    PIN_7 = 0x00000080   // GPIO pin 7
};

class HX711 {
    public:
        enum gain_t {
            CHANNEL_A_128, // channel A with a gain of 128
            CHANNEL_A_64,  // channel A with a gain of 64
            CHANNEL_B_32   // channel B has fixed gain of 32
        };

        /**
         * Constructor takes in the ports and pins connected to DOUT and PD_SCK
         * of the device.
         */
        HX711(port_t dout_port, pinNum_t dout_pin, port_t sck_port, pinNum_t sck_pin);
        /* Initialises the SCK and DOUT pins, gain defaults to the maximum 128 */
        void init(gain_t gain = CHANNEL_A_128);
        void set_gain(gain_t gain);
        /* Calibrate tare weight based on number of samples */
        void tare(uint8_t num_samples = 30);
        /* Set tare value manually */
        void set_tare_offset(int32_t offset);
        int32_t get_tare_offset(void);

        /* Read raw A/D value */
        int32_t read(void);
        int32_t read_avg(uint8_t num_samples);
        /* Read tare calibrated and scaled A/D value */
        double read_scaled(void);
        double read_scaled_avg(uint8_t num_samples);
        /* Set the scaling factor for the read_scaled() function */
        void set_scale(float scaling_factor);
        float get_scale(void);

        void power_up(void);
        void power_down(void);
    private:
        bool is_ready(void);
        void enable_gpio(port_t port);
        port_t DOUT_PORT;
        uint32_t DOUT_PIN;
        port_t SCK_PORT;
        uint32_t SCK_PIN;
        uint8_t GAIN_CYCLES;
        int32_t tare_offset;
        float scale;
};

#endif
