#ifndef SI7021_H
#define SI7021_H


void si7021_init(int i2c_module);

float read_temperature(void);

float read_humidity(void);

float reset(void);