#include "SI7021.h"

#define SI7021_ADDR                 0x40
#define HUMID_MEAS_HOLD_CMD         0xE5
#define HUMID_MEAS_NOHOLD_CMD       0xF5
#define TEMP_MEAS_HOLD_CMD          0xE3
#define TEMP_MEAS_NOHOLD_CMD        0xF3
#define SI7021_RESET_CMD            0xFE
#define SI7021_ID1_CMD              0xFA0F
#define SI7021_ID2_CMD              0xFCC9
#define NUM_READS_ID                4
#define USER_REG_1_READ_CMD         0xE7
#define USER_REG_1_WRITE_CMD        0xE6
#define HEATER_CTRL_REG_READ_CMD    0x51
#define HEATER_CTRL_REG_WRITE_CMD   0x11
#define FIRMWARE_REV_CMD            0x84B8

SI7021::SI7021(i2cModule_t i2c_module) {
    module = i2c_module;
}

void SI7021::init(void) {
    i2c_init(module, FAST);
}

uint32_t SI7021::read_temperature(void) {
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    // perform temperature measurement
    i2c_write(module, TEMP_MEAS_HOLD_CMD, I2C_CMD_SEND_START); 
    i2c_set_slave_addr(module, SI7021_ADDR, true);
    uint8_t msb;
    uint8_t lsb;
    //uint8_t checksum;
    // request temperature measurement
    i2c_read(module, &msb, I2C_CMD_RECEIVE_START); // read msb
    // read lsb, nack and stop transmission
    i2c_read(module, &lsb, I2C_CMD_RECEIVE_FINISH);
    //i2c_read(module, &checksum, I2C_CMD_RECEIVE_FINISH); // optional checksum
    uint16_t temp16 = (msb << 8) | lsb;
    float temp = 175.72 * (float) temp16 / 65536 - 46.85;
    return (uint32_t) temp;
}

uint32_t SI7021::read_humidity(void) {
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    // perform temperature measurement
    i2c_write(module, HUMID_MEAS_HOLD_CMD, I2C_CMD_SEND_START); 
    i2c_set_slave_addr(module, SI7021_ADDR, true);
    uint8_t msb;
    uint8_t lsb;
    //uint8_t checksum;
    // request temperature measurement
    i2c_read(module, &msb, I2C_CMD_RECEIVE_START);
    i2c_read(module, &lsb, I2C_CMD_RECEIVE_FINISH);
    uint16_t hum16 = (msb << 8) | lsb;
    float humidity = 125 * (float) hum16 / 65536 - 6;
    return (uint32_t) humidity;
}

void SI7021::reset(void) {
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    i2c_write(module, SI7021_RESET_CMD, I2C_CMD_SINGLE_SEND); 
} 

void SI7021::read_serial_number(uint32_t *ser_hi, uint32_t *ser_lo) {
    *ser_hi = read_serial32(SI7021_ID1_CMD);
    *ser_lo = read_serial32(SI7021_ID2_CMD);
}

uint32_t SI7021::read_serial32(uint32_t cmd) {
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    i2c_write(module, cmd >> 8, I2C_CMD_SEND_START);
    i2c_write(module, cmd & 0xFF, I2C_CMD_SEND_CONT);
    i2c_set_slave_addr(module, SI7021_ADDR, true);
    uint32_t ser;
    uint8_t tmp;
    uint8_t checksum;
    // start reading first upper byte
    i2c_read(module, &tmp, I2C_CMD_RECEIVE_START);
    ser = tmp;
    // loop and read the remaining upper bytes
    for (int i = 1; i < NUM_READS_ID; i++) {
        i2c_read(module, &checksum, I2C_CMD_RECEIVE_CONT);
        i2c_read(module, &tmp, I2C_CMD_RECEIVE_CONT);
        ser = (ser << 8 | tmp);
    }
    // nack last byte
    i2c_read(module, &checksum, I2C_CMD_RECEIVE_FINISH);
    return ser;
}

uint8_t SI7021::read_firmware_revision(void) {
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    i2c_write(module, FIRMWARE_REV_CMD >> 8, I2C_CMD_SEND_START);
    i2c_write(module, FIRMWARE_REV_CMD & 0xFF, I2C_CMD_SEND_CONT);
    i2c_set_slave_addr(module, SI7021_ADDR, true);
    uint8_t firmware;
    i2c_read(module, &firmware, I2C_CMD_RECEIVE_NACK_START);
    i2c_stop(module);
    return firmware;
}

void SI7021::write_register(controlReg_t reg, uint8_t data) {
    uint8_t cmd;
    if (reg == USER_REG_1) {
        cmd = USER_REG_1_WRITE_CMD;
    } else if (reg == HEATER_CTRL_REG) {
        cmd = HEATER_CTRL_REG_WRITE_CMD;
    }
    i2c_set_slave_addr(module, SI7021_ADDR, false);
    i2c_write(module, cmd, I2C_CMD_SEND_START); // write cmd
    i2c_write(module, data, I2C_CMD_SEND_FINISH); // write to reg
}

uint8_t SI7021::read_register(controlReg_t reg) {
    uint8_t cmd;
    if (reg == USER_REG_1) {
        cmd = USER_REG_1_READ_CMD;
    } else if (reg == HEATER_CTRL_REG) {
        cmd = HEATER_CTRL_REG_READ_CMD;
    }
    i2c_set_slave_addr(module, SI7021_ADDR, false); 
    i2c_write(module, cmd, I2C_CMD_SEND_START); // read cmd
    i2c_set_slave_addr(module, SI7021_ADDR, true); // switch to read
    uint8_t data;
    // read then nack 
    i2c_read(module, &data, I2C_CMD_RECEIVE_NACK_START); 
    i2c_stop(module);
    return data;
}
