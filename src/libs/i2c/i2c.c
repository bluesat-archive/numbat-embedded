#include "i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"

struct gpio_config_struct {
    uint32_t scl;
    uint32_t sda;
};

struct gpio_pin_struct {
    uint32_t base;
    uint32_t scl;
    uint32_t sda;
};

const uint32_t sysctl_module[] = {SYSCTL_PERIPH_I2C0, SYSCTL_PERIPH_I2C1,
								SYSCTL_PERIPH_I2C2, SYSCTL_PERIPH_I2C3};

const uint32_t i2c_module[] = {I2C0_BASE, I2C1_BASE, I2C2_BASE, I2C3_BASE};

const struct gpio_pin_struct gpio_pin[] = {
    {GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_3}, {GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_7},
	{GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_5}, {GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_1}};

const struct gpio_config_struct gpio_config[] = {
	{GPIO_PB2_I2C0SCL, GPIO_PB3_I2C0SDA}, {GPIO_PA6_I2C1SCL, GPIO_PA7_I2C1SDA}, 
	{GPIO_PE4_I2C2SCL, GPIO_PE5_I2C2SDA}, {GPIO_PD0_I2C3SCL, GPIO_PD1_I2C3SDA}};

// 0 - 100 Kbps, 1 - 400 Kbps, 2 - 1 Mbps (fast mode plus)
void i2c_init(int module, mode mode) {
	SysCtlPeripheralEnable(sysctl_module[module]);
	while (!SysCtlPeripheralReady(sysctl_module[module]));
	//GPIOIntRegister(i2c_module[module], i2cIntHandler) // might do interrupts later
	// only module 0 defaults to the i2c function, need to configure for the other gpios
	// set SCL push pull
	GPIOPinTypeI2CSCL(gpio_pin[module].base, (uint8_t) gpio_pin[module].scl); 
	// set SDA open drain, weak pullup
	GPIOPinTypeI2C(gpio_pin[module].base, (uint8_t) gpio_pin[module].sda; 
	// enable i2c functionality on the respective pins
	GPIOPinConfigure(gpio_config[module].scl);
	GPIOPinConfigure(gpio_config[module].sda);

	I2CMasterInitExpClk(i2c_module[module], SysCtlClockGet(), mode > 0); // initialise master
	if (mode == FAST_PLUS) {
		/*
		uint32_t SCLFreq = 100000000
		ui32TPR = ((ui32I2CClk + (2 * 10 * ui32SCLFreq) - 1) /
               (2 * 10 * ui32SCLFreq)) - 1;*/
		HWREG(i2c_module[module] + I2C_O_MTPR) /= 3; // divide timer period by 3 for fast plus mode
	}
	//I2CMasterSlaveAddrSet(I2C0_BASE + module, slave, false); // initialise slave
}

void i2c_set_slave_addr(int module, uint8_t slave_addr, bool read) {
	// set slave device to communicate with
	I2CMasterSlaveAddrSet(i2c_module[module], slave_addr, read);
}

int i2c_stop(int module) {
	I2CMasterControl(i2c_module[module], I2C_CMD_STOP);
	int error = I2CMasterErr(i2c_module[module]);
	return error;
}


void i2c_write(int module, uint8_t data, uint32_t command) {
	// put byte to transmit on bus
	I2CMasterDataPut(i2c_module[module], data);
	I2CMasterControl(i2c_module[module], command);
	while(I2CMasterBusBusy(i2c_module[module]));
}

int i2c_read(int module, uint8_t *data, uint32_t command) {
	I2CMasterControl(i2c_module[module], command);
	while(I2CMasterBusBusy(i2c_module[module]));
	int status = I2CMasterErr(i2c_module[module]);
	if (status) { // got an error, stop transmission 
		i2c_stop_transmission(i2c_module[module]);
	} else { // no error, read data
		*data = I2CMasterDataGet(i2c_module[module]);
	}
	return status;
}

/*
void i2c_write(int module, uint8_t msg[], size_t length) {
	if (length > 1) { // more than one byte to send
		// repeated start
		i2c_write_byte(i2c_module[module], msg[0], I2C_MASTER_CMD_BURST_SEND_START);
		for (size_t i = 1; i < (length - 1); i++) {
			// send byte, stay in transmit state
			i2c_write_byte(i2c_module[module], msg[i], I2C_MASTER_CMD_BURST_SEND_CONT);
		}
		// send last byte then signal stop condition
		i2c_write_byte(i2c_module[module], msg[length - 1], I2C_MASTER_CMD_BURST_SEND_FINISH);
	} else {
		// start, transmit one byte, stop
		i2c_write_byte(i2c_module[module], msg[0], I2C_MASTER_CMD_SINGLE_SEND);
	}

}*/
