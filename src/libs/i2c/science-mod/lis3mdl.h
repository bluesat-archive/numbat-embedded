/* Ported from Polulu Arduino implementation
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

class LIS3MDL {
    /* slave address is modified by the SDO/SA1 pin */
    typedef enum {
        SLAVE_LOW, // SDO/SA1 connected to ground
        SLAVE_HIGH, // SDO/SA1 connected to supply
        SLAVE_AUTO // automatically detect the state of SDO/SA1
    } slaveAddrState_t;

    
    typedef enum {
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
        INT_THS_H   = 0x33,
    } lis3mdlReg_t;

    public:
        /** 
         * Constructor that takes in the I2C module connected to the device and 
         * optionally the state of the SD0/SA1 pin.
         * 
         * The state determines the I2C slave address. If not specified, the
         * address will be detected automatically.
         */
        LIS3MDL(i2cModule_t i2c_module, slaveAddrState_t = SLAVE_AUTO);

        /* Initialises the I2C module obtains the device address if necessary.*/
        void init(void);

        /* Powers up the magnetometer with default settings */
        void enable(void);

        /* Reads the magnetism in the x,y,z directions */
        void read_magnetism(int16_t *x, int16_t *y, int16_t *z);
        void write_register(uint8_t reg, uint8_t data);


    private:
        i2cModule_t module;
        slaveAddrState_t state;
        uint8_t lis3mdl_addr;
        
        uint8_t test_device_addr(uint8_t addr);
}