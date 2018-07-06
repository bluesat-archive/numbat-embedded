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


class SI7021 {
    public:
        /* Control registers to set the configuration of SI7021 */
        typedef enum {
            USER_REG_1,
            HEATER_CTRL_REG
        } controlReg_t;

        SI7021(i2cModule_t = I2C0);
        void init(void);
        float read_temperature(void);
        float read_humidity(void);
        void reset(void);
        void read_serial_number(uint32_t *ser_hi, uint32_t *ser_lo);
        void write_register(controlReg_t reg, uint8_t data);
        uint8_t read_register(controlReg_t reg);
    
    private:
        int module;

        uint32_t read_serial32(uint32_t cmd);
}



