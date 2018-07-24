

/* LIBRARY PORTED FROM ARDUINO IMPLEMENTATION 
 * https://github.com/adafruit/Adafruit_Si7021/blob/master/Adafruit_Si7021.h */

/*
 * Date Started: 6/7/18
 * Original Author: [Original Author's Name]
 * Editors: [Editor 1], [Editor 2]
 * Purpose: 
 * This code is released under the MIT [GPL for embeded] License. Copyright BLUEsat UNSW, 2017
 */

#ifndef TCS34725_H
#define TCS34725_H

#include "i2c.h"

#define TCS34725_CONTROL    (0x0F)    /* Gain control register */
#define TCS34725_ATIME      (0x01)    /* Integration time control register */
#define TCS34725_PERS_NONE  (0b0000)  /* Every RGBC cycle generates an interrupt*/
#define TCS34725_PERS_1     (0b0001)  /* 1 clear channel value outisde limits */
#define TCS34725_PERS_2     (0b0010)  /* 2 clean channel values outside limits  */
#define TCS34725_PERS_3     (0b0011)  /* 3 clean channel values outside limits  */
#define TCS34725_PERS_5     (0b0100)  /* 5 clean channel values outside limits  */
#define TCS34725_PERS_10    (0b0101)  /* 10 clean channel values outside limits */
#define TCS34725_PERS_15    (0b0110)  /* 15 clean channel values outside limits */
#define TCS34725_PERS_20    (0b0111)  /* 20 clean channel values outside limits */
#define TCS34725_PERS_25    (0b1000)  /* 25 clean channel values outside limits */
#define TCS34725_PERS_30    (0b1001)  /* 30 clean channel values outside limits */
#define TCS34725_PERS_35    (0b1010)  /* 35 clean channel values outside limits */
#define TCS34725_PERS_40    (0b1011)  /* 40 clean channel values outside limits */
#define TCS34725_PERS_45    (0b1100)  /* 45 clean channel values outside limits */
#define TCS34725_PERS_50    (0b1101)  /* 50 clean channel values outside limits */
#define TCS34725_PERS_55    (0b1110)  /* 55 clean channel values outside limits */
#define TCS34725_PERS_60    (0b1111)  /* 60 clean channel values outside limits */


class TCS34725 {
    public:
        /* The integration time is not limited to these values. It can be
         * controlled in increments of 2.4ms by manually setting the TCS34725_ATIME
         * register. 
         */
        enum integrationTime_t {
            INTEGRATIONTIME_2_4MS  = 0xFF,   /*  2.4m  */
            INTEGRATIONTIME_24MS   = 0xF6,   /*  24ms  */
            INTEGRATIONTIME_50MS   = 0xEB,   /*  50ms  */
            INTEGRATIONTIME_101MS  = 0xD5,   /*  101ms */
            INTEGRATIONTIME_154MS  = 0xC0,   /*  154ms */
            INTEGRATIONTIME_700MS  = 0x00    /*  700ms */
        };

        /* Gain scaling for the RGBC values */
        enum gain_t {
            GAIN_1X                = 0x00,   /*  No gain  */
            GAIN_4X                = 0x01,   /*  4x gain  */
            GAIN_16X               = 0x02,   /*  16x gain */
            GAIN_60X               = 0x03    /*  60x gain */
        };

        /* Wait times are 12x longer if the WLONG bit is asserted */
        enum waitTime_t {
            WTIME_2_4MS = 0xFF,
            WTIME_204MS = 0xAB,
            WTIME_614MS = 0x00
        };

        TCS34725(i2cModule_t, integrationTime_t = INTEGRATIONTIME_2_4MS, gain_t = GAIN_1X);
        /* 
         * Initialises the I2C module and powers on the device. The wait cycle
         * (power saving mode) and interrupt generation are disabled by default.
         */
        void init(void);
        void set_integration_time(integrationTime_t it);
        void set_gain(gain_t gain);
        void read_raw_data(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

        /*
         * Calculates the temperature in degrees Kelvin from the given R/G/B values 
         * @return colour temperature in degrees Kelvin 
         */
        uint16_t calculate_colour_temperature(uint16_t r, uint16_t g, uint16_t b);

        /* Calculates the illuminance from the R/G/B values */
        uint16_t calculate_lux(uint16_t r, uint16_t g, uint16_t b);

        /* Enables the low power wait state */
        void enable_wait(void);

        /* Sets the wait time between each RGBC integration cycle. Wait state
         * must be enabled for this to take into effect.
         * @param wlong if set, wait time is multipled by 12
         */
        void set_wait_time(waitTime_t wt, bool wlong);
        void disable_wait(void);

        void power_on(void);
        /* Transitions the state into low power sleep mode */
        void power_down(void);

        void write8(uint8_t reg, uint8_t data);
        uint8_t read8(uint8_t reg);
        uint16_t read16(uint8_t reg);

        /* Enables or disables interrupts depending on the specified boolean value */
        void set_interrupt(bool enable);
        /* Clears pending interrupts */
        void clear_interrupt(void);
        /* Sets the high and low thresholds for interrupt generation. An interrupt
         * is generated if the clear channel value is outisde the specified limits.
         */ 
        void set_interrupt_limits(uint16_t low, uint16_t high);
        /* Sets the number of consecutive values the clear channel must have 
         * outside the limits before an interrupt is generated.
         */
        void set_interrupt_persistence(uint8_t value);

    private:
        i2cModule_t module;
        gain_t tcs34725_gain;
        integrationTime_t tcs34725_integration_time;
        bool interrupt_enabled;
};

#endif
