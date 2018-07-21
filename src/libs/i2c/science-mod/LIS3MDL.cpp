#include "LIS3MDL.h"

#define LIS3MDL_HIGH_ADDR  0x1E
#define LIS3MDL_LOW_ADDR   0x1C

#define TEST_ADDR_ERROR    -1
#define LIS3MDL_WHO_ID     0x3D


#define LIS3MDL_SENSITIVITY_SCALE_4G   6842  /* Sensitivity for 4 gauss scale [LSB/gauss] */
#define LIS3MDL_SENSITIVITY_SCALE_8G   3421  /* Sensitivity for 8 gauss scale [LSB/gauss] */
#define LIS3MDL_SENSITIVITY_SCALE_12G  2281  /* Sensitivity for 12 gauss scale [LSB/gauss] */
#define LIS3MDL_SENSITIVITY_SCALE_16G  1711  /* Sensitivity for 16 gauss scale [LSB/gauss] */

LIS3MDL::LIS3MDL(i2cModule_t i2c_module, slaveAddrState_t slave_state) {
    module = i2c_module;
    switch (slave_state) {
        case SLAVE_LOW:
            lis3mdl_addr = LIS3MDL_LOW_ADDR;
        case SLAVE_HIGH:
            lis3mdl_addr = LIS3MDL_HIGH_ADDR;
        default:
            lis3mdl_addr = SLAVE_AUTO;
    }
    sensitivity = 0;
}

bool LIS3MDL::init(void) {
    i2c_init(module, FAST);
    if (lis3mdl_addr == SLAVE_AUTO) {
        if (test_device_addr(LIS3MDL_HIGH_ADDR) == LIS3MDL_WHO_ID) {
            lis3mdl_addr = LIS3MDL_HIGH_ADDR;
        } else if (test_device_addr(LIS3MDL_LOW_ADDR) == LIS3MDL_WHO_ID) {
            lis3mdl_addr = LIS3MDL_LOW_ADDR;
        }
    }
    // couldn't determine slave address, failed to initialise
    if (lis3mdl_addr == SLAVE_AUTO) {
        return false;
    }
    enable_default();
    return true;
}

void LIS3MDL::enable_default(void) {
    // 0x70 = 0b01110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
    write_register(CTRL_REG1, 0x70);

    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    write_register(CTRL_REG2, 0x00);
    sensitivity = (float) LIS3MDL_SENSITIVITY_SCALE_4G;

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    write_register(CTRL_REG3, 0x00);

    // 0x0C = 0b00001100
    // OMZ = 11 (ultra-high-performance mode for Z)
    write_register(CTRL_REG4, 0x0C);
}

void LIS3MDL::read_raw_magnetism(int16_t *x, int16_t *y, int16_t *z) {
    i2c_set_slave_addr(module, lis3mdl_addr, false);
    i2c_write(module, OUT_X_L | 0x80, I2C_CMD_SEND_START);
    uint8_t xlm, xhm, ylm, yhm, zlm, zhm;
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

void LIS3MDL::read_magnetism(float *x, float *y, float *z) {
    int16_t xr, yr, zr;
    read_raw_magnetism(&xr, &yr, &zr);
    *x = xr / sensitivity;
    *y = yr / sensitivity;
    *z = zr / sensitivity;
}

void LIS3MDL::set_scale(lis3mdlScale_t scale) {
    write_register(CTRL_REG2, scale);
    switch (scale) {
        case LIS3MDL_SCALE_4G:
            sensitivity = LIS3MDL_SENSITIVITY_SCALE_4G;
            break;
        case LIS3MDL_SCALE_8G: 
            sensitivity = LIS3MDL_SENSITIVITY_SCALE_8G;
            break;
        case LIS3MDL_SCALE_12G:
            sensitivity = LIS3MDL_SENSITIVITY_SCALE_12G;
            break;
        case LIS3MDL_SCALE_16G:
            sensitivity = LIS3MDL_SENSITIVITY_SCALE_16G;
            break;
    }
}


void LIS3MDL::write_register(lis3mdlReg_t reg, uint8_t data) {
    i2c_set_slave_addr(module, lis3mdl_addr, false);
    i2c_write(module, reg, I2C_CMD_SEND_START);
    i2c_write(module, data, I2C_CMD_SEND_FINISH);
}

uint8_t LIS3MDL::read_register(lis3mdlReg_t reg) {
    i2c_set_slave_addr(module, lis3mdl_addr, false);
    i2c_write(module, reg, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, lis3mdl_addr, true);
    uint8_t data;
    // read then nack 
    i2c_read(module, &data, I2C_CMD_RECEIVE_NACK_START); 
    i2c_stop(module);
    return data;
}


uint8_t LIS3MDL::test_device_addr(uint8_t addr) {
    i2c_set_slave_addr(module, addr, false);
    i2c_write(module, WHO_AM_I, I2C_CMD_SEND_START);
    if (i2c_stop(module) != 0) {
        return TEST_ADDR_ERROR;
    } 
    i2c_set_slave_addr(module, addr, true);
    uint8_t data;
    if (i2c_read(module, &data, I2C_CMD_SINGLE_RECEIVE)) {
        return TEST_ADDR_ERROR;
    } else {
        return data;
    }
}