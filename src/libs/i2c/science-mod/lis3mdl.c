#include "lis3mdl.h"
#include "i2c.h"

static int i2c_module = 0;

/*
void read_magnet(int slave, int16_t *x, int16_t *y, int16_t *z) {
    i2c_read(address);
    uint8_t xlm = i2c_read();
    uint8_t xhm = i2c_read();
    uint8_t ylm = i2c_read();
    uint8_t yhm = i2c_read();
    uint8_t zlm = i2c_read();
    uint8_t zhm = i2c_read();
    *x = (int16_t) (xhm << 8 | xlm);
    *y = (int16_t) (yhm << 8 | ylm);
    *z = (int16_t) (zhm << 8 | zlm);
}*/