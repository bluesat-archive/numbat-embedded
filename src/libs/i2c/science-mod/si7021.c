#include "si7021.h"
#include "i2c.h" // need to include in path

#define HUMID_MEAS_HOLD_CMD     0xE5
#define HUMID_MEAS_NOHOLD_CMD   0xF5
#define TEMP_MEAS_HOLD_CMD      0xE3
#define TEMP_MEAS_NOHOLD_CMD    0xF3
#define SI7021_RESET_CMD        0xFE
#define SI7021_ID1_CMD          0xFA0F
#define SI7021_ID2_CMD          0xFCC9

static module module = I2C0;
const uint8_t slave_addr = 0x40;

void si7021_init(int i2c_module) {
    module = i2c_module;
    i2c_init(module, FAST);
}

float read_temperature(void) {
    i2c_set_slave_addr(I2C0, slave_addr, false);
    // perform temperature measurement
    i2c_write(I2C0, TEMP_MEAS_HOLD_CMD, I2C_CMD_SEND_START); 
    i2c_set_slave_addr(I2C0, slave_addr, true);
    uint8_t msb;
    uint8_t lsb;
    //uint8_t checksum;
    // request temperature measurement
    i2c_read(I2C0, &msb, I2C_CMD_RECEIVE_START);
    i2c_read(I2C0, &lsb, I2C_CMD_RECEIVE_FINISH);
    //i2c_read(I2C0, &checksum, I2C_CMD_RECEIVE_FINISH); // optional checksum
    uint16_t temp16 = (msb << 8) | lsb;
    float temp = 175.72 * (float) temp16 / 65536 - 46.85;
    return temp;
}

float read_humidity(void) {
    i2c_set_slave_addr(I2C0, slave_addr, false);
    // perform temperature measurement
    i2c_write(I2C0, HUMID_MEAS_HOLD_CMD, I2C_CMD_SEND_START); 
    i2c_set_slave_addr(I2C0, slave_addr, true);
    uint8_t msb;
    uint8_t lsb;
    //uint8_t checksum;
    // request temperature measurement
    i2c_read(I2C0, &msb, I2C_CMD_RECEIVE_START);
    i2c_read(I2C0, &lsb, I2C_CMD_RECEIVE_FINISH);
    uint16_t hum16 = (msb << 8) | lsb;
    float humidity = 125 * (float) hum16 / 65536 - 6;
    return humidity;
}

void reset(void) {
    i2c_set_slave_addr(I2C0, slave_addr, false);
    i2c_write(I2C0, SI7021_RESET_CMD, I2C_CMD_SINGLE_SEND); 
} 
