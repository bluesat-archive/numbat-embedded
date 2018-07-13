#include "LIS3MDL.h"
#include "i2c.h"


#define LIS3MDL_HIGH_ADDR  0b0011110
#define LIS3MDL_LOW_ADDR   0b0011100

#define TEST_ADDR_ERROR    -1
#define LIS3MDL_WHO_ID     0x3D

LIS3MDL::LIS3MDL(i2cModule_t i2c_module, slaveAddrState_t slave_state) {
    module = i2c_module;
    state = slave_state;
}

// might want to return whether successful or not
void LIS3MDL::init(void) {
    i2c_init(module, FAST);
    if (state == SLAVE_AUTO) {
        if (testReg(LIS3MDL_HIGH_ADDRESS, WHO_AM_I) == LIS3MDL_WHO_ID) {
            lis3mdl_addr = LIS3MDL_HIGH_ADDR;
        } else if (testReg(LIS3MDL_LOW_ADDRESS, WHO_AM_I) == LIS3MDL_WHO_ID) {
            lis3mdl_addr = LIS3MDL_LOW_ADDR;
        }
    }
}

void LIS3MDL::enable(void) {
    // 0x70 = 0b01110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
    write_register(CTRL_REG1, 0x70);

    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    write_register(CTRL_REG2, 0x00);

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    write_register(CTRL_REG3, 0x00);

    // 0x0C = 0b00001100
    // OMZ = 11 (ultra-high-performance mode for Z)
    write_register(CTRL_REG4, 0x0C);
}

void LIS3MDL::read_magnetism(int16_t *x, int16_t *y, int16_t *z) {
    i2c_set_slave_addr(module, lis3mdl_addr, false);
    i2c_write(module, OUT_X_L | 0x80, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, lis3mdl_addr, true);
    i2c_read(module, &xlm, I2C_CMD_RECEIVE_START);
    i2c_read(module, &xhm, I2C_CMD_RECEIVE_CONT);
    i2c_read(module, &ylm, I2C_CMD_RECEIVE_CONT);
    i2c_read(module, &yhm, I2C_CMD_RECEIVE_CONT);
    i2c_read(module, &zlm, I2C_CMD_RECEIVE_CONT);
    i2c_read(module, &zhm, I2C_CMD_RECEIVE_FINISH);
    *x = (int16_t) (xhm << 8 | xlm);
    *y = (int16_t) (yhm << 8 | ylm);
    *z = (int16_t) (zhm << 8 | zlm);
}

void LIS3MDL::write_register(uint8_t reg, uint8_t data) {
    i2c_set_slave_addr(module, lis3mdl_addr, false);
    i2c_write(module, reg, I2C_CMD_SEND_START);
    i2c_write(module, data, I2C_CMD_SEND_FINISH);
}

uint8_t LIS3MDL::test_device_addr(uint8_t addr) {
    i2c_set_slave_addr(module, addr, false);
    i2c_write(module, WHO_AM_I, I2C_CMD_SEND_START);
    if (i2c_stop() != 0) {
        return TEST_ADDR_ERROR;
    } 
    i2c_set_slave_addr(module, addr, true);
    uint8_t data;
    if (i2c_read(module, &data, I2C_CMD_SINGLE_RECEIVE))
        return TEST_ADDR_ERROR;
    } else {
        return data;
    }
}