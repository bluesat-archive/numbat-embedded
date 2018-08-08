//#include <math.h>
#include "TCS34725.h"
#include "driverlib/sysctl.h"

#define TCS34725_ADDRESS          (0x29)
#define TCS34725_COMMAND_BIT      (0x80)

#define TCS34725_ENABLE           (0x00)
#define TCS34725_ENABLE_AIEN      (0x10)    /* RGBC Interrupt Enable */
#define TCS34725_ENABLE_WEN       (0x08)    /* Wait enable */
#define TCS34725_ENABLE_AEN       (0x02)    /* RGBC Enable */
#define TCS34725_ENABLE_PON       (0x01)    /* Power on */

#define TCS34725_WTIME            (0x03)    /* Wait time (if WEN is set) */
#define TCS34725_CONFIG           (0x0D)
#define TCS34725_CONFIG_WLONG     (0x02)    /* Multiply wait times by 12x */

#define TCS34725_CDATA            (0x14)    /* Clear channel data */
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATA            (0x16)    /* Red channel data */
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATA            (0x18)    /* Green channel data */
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATA            (0x1A)    /* Blue channel data */
#define TCS34725_BDATAH           (0x1B)
#define TCS34725_AILTL            (0x04)    /* Clear channel lower threshold */
#define TCS34725_AILTH            (0x05)
#define TCS34725_AIHTL            (0x06)    /* Clear channel upper threshold */
#define TCS34725_AIHTH            (0x07)

#define TCS34725_PERS             (0x0C)    /* Persistence register */

#define TCS34725_STATUS           (0x13)
#define TCS34725_STATUS_AINT      (0x10)    /* Clear channel interrupt state */
#define TCS34725_STATUS_AVALID    (0x01)    /* Integration cycle complete */

#define TCS34725_ID               0x12

namespace {
    static void delay(uint32_t ms) {
        for (uint32_t i=0; i < ms; i++) {
            SysCtlDelay(25000); // 1 ms delay
        }
    }
}

/*
float powf(const float x, const float y) {
  return (float)(pow((double)x, (double)y));
}*/

/* Constructor */
TCS34725::TCS34725(i2cModule_t i2c_module, integrationTime_t it, gain_t gain) {
    module = i2c_module;
    tcs34725_integration_time = it;
    tcs34725_gain = gain;
    interrupt_enabled = false;
}


bool TCS34725::init(void) {
    i2c_init(module, FAST);
    uint8_t id = read8(TCS34725_ID);
    if (id != 0x44) {
        return false;
    }
    set_integration_time(tcs34725_integration_time);
    set_gain(tcs34725_gain);
    power_on();
    return true;
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
    // if interrupts are not enabled, we have to poll the status register
    // until the integration cycle is complete
    if (!interrupt_enabled) {
        uint8_t status = 0;
        while (!(status & TCS34725_STATUS_AVALID)) {
            status = read8(TCS34725_STATUS);
        }

    }
    *c = read16(TCS34725_CDATA);
    *r = read16(TCS34725_RDATA);
    *g = read16(TCS34725_GDATA);
    *b = read16(TCS34725_BDATA);
}

uint16_t TCS34725::calculate_colour_temperature(uint16_t r, uint16_t g, uint16_t b) {
    float X, Y, Z;      /* RGB to XYZ correlation      */
    float xc, yc;       /* Chromaticity co-ordinates   */
    double n;            /* McCamy's formula            */
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
    //cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;
    cct = (449.0F * n*n*n) + (3525.0F * n*n) + (6823.3F * n) + 5520.33F;
    /* Return the results in degrees Kelvin */
    return (uint16_t) cct;
}

uint16_t TCS34725::calculate_lux(uint16_t r, uint16_t g, uint16_t b){
    float illuminance;

    /* This only uses RGB ... how can we integrate clear or calculate lux */
    /* based exclusively on clear since this might be more reliable?      */
    illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    return (uint16_t) illuminance;
}

void TCS34725::set_wait_time(waitTime_t wt, bool wlong) {
    if (wlong) {
        write8(TCS34725_CONFIG, TCS34725_CONFIG_WLONG);
    } else {
        write8(TCS34725_CONFIG, 0x00);
    }
    write8(TCS34725_WTIME, wt);
}

void TCS34725::enable_wait(void) {
    // read current enable register settings and set the wait enable bit
    uint8_t state = read8(TCS34725_ENABLE);
    write8(TCS34725_ENABLE, state | TCS34725_ENABLE_WEN);
}

void TCS34725::disable_wait(void) {
    uint8_t state = read8(TCS34725_ENABLE);
    // unset the wait enable bit
    write8(TCS34725_ENABLE, state & ~TCS34725_ENABLE_WEN);
}

void TCS34725::power_on(void) {
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    // delay 3 ms
    delay(3);
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

void TCS34725::power_down(void) {
    // power down device
    uint8_t reg = 0;
    reg = read8(TCS34725_ENABLE);
    write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

void TCS34725::write8(uint8_t reg, uint8_t data) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_write(module, data, I2C_CMD_SEND_FINISH);
}

uint8_t TCS34725::read8(uint8_t reg) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, TCS34725_ADDRESS, true);
    uint8_t data = 0;
    i2c_read(module, &data, I2C_CMD_RECEIVE_NACK_START);
    i2c_stop(module);
    return data;
}

uint16_t TCS34725::read16(uint8_t reg) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | reg, I2C_CMD_SEND_START);
    i2c_set_slave_addr(module, TCS34725_ADDRESS, true);
    uint16_t data = 0;
    uint8_t lsb = 0;
    uint8_t msb = 0;
    // lsb is read first
    i2c_read(module, &lsb, I2C_CMD_RECEIVE_START);
    i2c_read(module, &msb, I2C_CMD_RECEIVE_FINISH);
    data = (msb << 8) | lsb;
    return data;
}


void TCS34725::set_interrupt(bool enable) {
    uint8_t r = read8(TCS34725_ENABLE);
    if (enable) {
        r |= TCS34725_ENABLE_AIEN;
    } else {
        r &= ~TCS34725_ENABLE_AIEN;
    }
    write8(TCS34725_ENABLE, r);
    interrupt_enabled = enable;
}


void TCS34725::clear_interrupt(void) {
    i2c_set_slave_addr(module, TCS34725_ADDRESS, false);
    i2c_write(module, TCS34725_COMMAND_BIT | 0x66, I2C_CMD_SINGLE_SEND);
}

void TCS34725::set_interrupt_limits(uint16_t low, uint16_t high) {
    write8(TCS34725_AILTL, low & 0xFF);
    write8(TCS34725_AILTH, low >> 8);
    write8(TCS34725_AIHTL, high & 0xFF);
    write8(TCS34725_AIHTH, high >> 8);
}

void TCS34725::set_interrupt_persistence(uint8_t value) {
    write8(TCS34725_PERS, value);
}
