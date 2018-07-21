/*
 * Date Started: 6/7/18
 * Original Author: [Original Author's Name]
 * Editors: [Editor 1], [Editor 2]
 * Purpose: This is example code, that demonstrates how to write a header comment
 * in the appropriate format!
 * This code is released under the MIT [GPL for embeded] License. Copyright BLUEsat UNSW, 2017
 */

#ifndef SI7021_H
#define SI7021_H

#include "i2c.h"

class SI7021 {
    public:
        /* Control registers to set the configuration of SI7021 */
        enum controlReg_t {
            USER_REG_1,
            // used to modify the heater current
            // reduces the likelihood of offset accumulation of humidty sensor
            HEATER_CTRL_REG 
        };

        SI7021(i2cModule_t);
        void init(void);
        uint32_t read_temperature(void);
        uint32_t read_humidity(void);
        void write_register(controlReg_t reg, uint8_t data);
        uint8_t read_register(controlReg_t reg);
        void reset(void);
        void read_serial_number(uint32_t *ser_hi, uint32_t *ser_lo);
        uint8_t read_firmware_revision(void);

    private:
        uint32_t read_serial32(uint32_t cmd);

        i2cModule_t module;     
};

#endif


