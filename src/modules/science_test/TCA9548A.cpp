#include "i2c.h"
#include "TCA9548A.h"

#define TCA_ADDRESS 0x70

static int module = I2C0;

void i2c_select(uint8_t num) {
    i2c_set_slave_addr(module, TCA_ADDRESS, false);    
    i2c_write(module, 1 << num, I2C_CMD_SINGLE_SEND);
}

