#include <math.h>
#include "TCS34725.h"
#include "i2c.h"

/* Constructor */
TCS34725::TCS34725(i2cModule_t i2c_module, integrationTime_t it, gain_t gain) {
    module = i2c_module;
    tcs34725_integration_time = it;
    tcs34725_gain = gain;
}


void TCS34725::init(void) {
    i2c_init(module, FAST);
    set_integration_time(tcs34725_integration_time);
    set_gain(tcs34725_gain);
    enable();
}

void TCS34725::set_integration_time(integrationTime_t it) {
    write8(TCS34725_ATIME, it); // update timing register
    tcs34725_integration_time = it; // update value placeholders
}

void TCS34725::set_gain(gain_t gain) {
    write8(TCS34725_CONTROL, gain);
    tcs34725_gain = gain;
}

void TCS34725::read_raw_data(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {
    *c = read16(TCS34725_CDATAL);
    *r = read16(TCS34725_RDATAL);
    *g = read16(TCS34725_GDATAL);
    *b = read16(TCS34725_BDATAL);

    /* Set a delay for the integration time */
    switch (tcs34725_integration_time) {
    case TCS34725_INTEGRATIONTIME_2_4MS:
        delay(3);
        break;
    case TCS34725_INTEGRATIONTIME_24MS:
        delay(24);
        break;
    case TCS34725_INTEGRATIONTIME_50MS:
        delay(50);
        break;
    case TCS34725_INTEGRATIONTIME_101MS:
        delay(101);
        break;
    case TCS34725_INTEGRATIONTIME_154MS:
        delay(154);
        break;
    case TCS34725_INTEGRATIONTIME_700MS:
        delay(700);
        break;
    }
}

uint16_t TCS34725::calculate_colour_temperature(uint16_t r, uint16_t g, uint16_t b) {
    float X, Y, Z;      /* RGB to XYZ correlation      */
    float xc, yc;       /* Chromaticity co-ordinates   */
    float n;            /* McCamy's formula            */
    float cct;

    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
    Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);

    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);

    /* Calculate the final CCT */
    cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

    /* Return the results in degrees Kelvin */
    return (uint16_t) cct;
}

uint16_t TCS34725::calculate_lux(uint16_t r, uint16_t g, uint16_t b)
{
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return (uint16_t) illuminance;
}


void TCS34725::enable(void) {
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    // delay 3 ms
    delay(3);
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

void TCS34725::disable(void) {
    // power down device
    uint8_t reg = 0;
    reg = read8(TCS34725_ENABLE);
    write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

void TCS34725::write8(uint8_t reg, uint8_t data) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_write(module, TCS34725_COMMAND_BIT | data, I2C_CMD_SEND_FINISH);
    i2c_stop(module);
}

uint8_t TCS34725::read8(uint8_t reg) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, TCS34725_ADDRESS, true);
    uint8_t data;
    i2c_read(module, &data, I2C_CMD_RECEIVE_NACK_START);
    i2c_stop(module);
    return data;
}

uint16_t TCS34725::read16(uint8_t reg) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, TCS34725_ADDRESS, true);
    uint16_t data;
    uint8_t tmp;
    i2c_read(module, &tmp, I2C_CMD_RECEIVE_START);
    data = tmp;
    i2c_read(module, &tmp, I2C_CMD_RECEIVE_FINISH);
    data = (data << 8) | tmp;
    return data;
}

void TCS34725::set_interrupt(bool flag) {
    uint8_t r = read8(TCS34725_ENABLE);
    if (flag) {
        r |= TCS34725_ENABLE_AIEN;
    } else {
        r &= ~TCS34725_ENABLE_AIEN;
    }
    write8(TCS34725_ENABLE, r);
}

void TCS34725::clear_interrupt(void) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | 0x66, I2C_CMD_SINGLE_SEND);
}


void TCS34725::set_int_limits(uint16_t low, uint16_t high) {
    write8(0x04, low & 0xFF);
    write8(0x05, low >> 8);
    write8(0x06, high & 0xFF);
    write8(0x07, high >> 8);
}

float powf(const float x, const float y) {
  return (float)(pow((double)x, (double)y));
}