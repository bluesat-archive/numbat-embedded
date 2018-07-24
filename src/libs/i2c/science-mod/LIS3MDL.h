/* Ported from Polulu implementation with some slight modifications
 * https://github.com/pololu/lis3mdl-arduino */

/*
 * Date Started: 6/7/18
 * Original Author: [Original Author's Name]
 * Editors: [Editor 1], [Editor 2]
 * Purpose: 
 * This code is released under the MIT [GPL for embeded] License. Copyright BLUEsat UNSW, 2017
 */

#ifndef LIS3MDL_H
#define LIS3MDL_H

#include "i2c.h"

class LIS3MDL {
    /* slave address is modified by the SDO/SA1 pin */
    enum slaveAddrState_t {
        SLAVE_LOW, // SDO/SA1 connected to ground
        SLAVE_HIGH, // SDO/SA1 connected to supply
        SLAVE_AUTO // automatically detect the state of SDO/SA1
    };

    enum lis3mdlReg_t {
        WHO_AM_I    = 0x0F,
        CTRL_REG1   = 0x20,
        CTRL_REG2   = 0x21,
        CTRL_REG3   = 0x22,
        CTRL_REG4   = 0x23,
        CTRL_REG5   = 0x24,
        STATUS_REG  = 0x27,
        OUT_X_L     = 0x28,
        OUT_X_H     = 0x29,
        OUT_Y_L     = 0x2A,
        OUT_Y_H     = 0x2B,
        OUT_Z_L     = 0x2C,
        OUT_Z_H     = 0x2D,
        TEMP_OUT_L  = 0x2E,
        TEMP_OUT_H  = 0x2F,
        INT_CFG     = 0x30,
        INT_SRC     = 0x31,
        INT_THS_L   = 0x32,
        INT_THS_H   = 0x33
    };

    enum lis3mdlScale_t {
        SCALE_4G   = 0x00, /* +-4 gauss full scale */
        SCALE_8G   = 0x20, /* +-8 gauss full scale */
        SCALE_12G  = 0x40, /* +-12 gauss full scale */
        SCALE_16G  = 0x60 /* +-16 gauss full scale */
    };

    public:
        /** 
         * Constructor that takes in the I2C module connected to the device and 
         * optionally the state of the SD0/SA1 pin.
         * 
         * The state determines the I2C slave address. If not specified, the
         * address will be detected automatically.
         */
        LIS3MDL(i2cModule_t i2c_module, slaveAddrState_t = SLAVE_AUTO);

        /* Initialises the I2C module and finds the device address if necessary.*/
        bool init(void);

        /* Powers up the magnetometer with default settings */
        void enable_default(void);

        /* Reads the magnetism scaled by the sensitivity in the x,y,z directions */
        void read_magnetism(float *x, float *y, float *z);

        /* Reads the raw magnetism in the x,y,z directions */
        void read_raw_magnetism(int16_t *x, int16_t *y, int16_t *z);

        /* Sets the full range scale */
        void set_scale(lis3mdlScale_t scale);

        void write_register(lis3mdlReg_t reg, uint8_t data);
        uint8_t read_register(lis3mdlReg_t reg);

    private:
        uint8_t test_device_addr(uint8_t addr);
        float sensitivity;
        i2cModule_t module;
        uint8_t lis3mdl_addr;
};

#endif