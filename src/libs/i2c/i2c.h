/*
 * Date Started: 3/7/18
 * Original Author: [Original Author's Name]
 * Editors: [Editor 1], [Editor 2]
 * Purpose: This is example code, that demonstrates how to write a header comment
 * in the appropriate format!
 * This code is released under the MIT [GPL for embeded] License. Copyright BLUEsat UNSW, 2017
 */

#ifndef I2C_H
#define I2C_H

/* I2C master commands */
#define I2C_CMD_SINGLE_SEND		0x00000007 // start -> transmit -> stop
#define I2C_CMD_SINGLE_RECEIVE 	0x00000007 // start -> receive -> stop
#define I2C_CMD_STOP 			0x00000004 // stop
#define I2C_CMD_SEND_START 		0x00000003 // (rep) start -> transmit
#define I2C_CMD_RECEIVE_START 	0x0000000b // (rep) start -> receive
/* Must currently be in transmit state for these commands to function properly */
#define I2C_CMD_SEND_CONT		0x00000001 // transmit
#define I2C_CMD_SEND_FINISH 	0x00000005 // transmit -> stop
/* Must currently be in receive state for these commands to function properly */
#define I2C_CMD_RECEIVE_CONT 	0x00000009 // receive
#define I2C_CMD_RECEIVE_FINISH 	0x00000005 // receive -> stop

typedef enum {
	NORMAL,
	FAST,
	FAST_PLUS
} mode;

typedef enum {
	I2C0,
	I2C1,
	I2C2,
	I2C3
} module;


/** Initialises an I2C module with a bus speed mode. 
 * This must be called if the module has not been initialised yet
 * 
 * @param module one of four possible modules: I2C0, I2C1, I2C2, I2C3
 * @param mode one of three modes defining the bus speed:
 * 			   NORMAL 	 - 100 Kbps
 * 			   FAST   	 - 400 Kbps
 * 			   FAST_PLUS - 1 Mbps
 */
void i2c_init(int module, mode mode);

/* Sets the address of the slave to communicate with, and signals 
 * whether the master will write or read from the slave.  
 * This must be called once before a new transmission sequence.
 * A transmission is not started until i2c_read() or i2c_write() is run.
 * 
 * A repeated call to this before stopping a transmission allows
 * the master to communicate with a different slave and/or change the 
 * read/write mode while maintaining control of the bus. 
 * 
 * @param read if true, data is to be read from the slave
 */
void i2c_set_slave_addr(int module, uint8_t slave_addr, bool read);

/* Stops a transmission initiated by i2c_write() or i2c_read(), freeing
 * the bus. This must be called after communication is complete, unless
 * an appropriate command sequence is used in i2c_cmd_write_byte() or
 * i2c_cmd_read_byte().
 */
void i2c_stop(int module);

/**
 * Writes a byte to the current slave with a specified send command.
 * The command specifies a sequence of commands to execute automatically
 * in hardware.   
 * 
 * The first call to i2c_write() must have a command that generates a start
 * condition.
 * 
 * @param command an appropriate send command sequence defined in the header 
 */
void i2c_write(int module, uint8_t data, uint32_t command);

/**
 * Reads a byte from the current slave with a specified receive command.
 * 
 * The first call to i2c_read() must have a command that generates a start
 * condition.
 * 
 * @param command an appropriate receive command sequence defined in the header 
 * @return int 0 if no error occurs, otherwise an error code
 */
int i2c_read(int module, uint8_t *data, uint32_t command);


/**
 * Starts a transmission and writes an array of bytes to the slave address
 * set by i2c_set_slave_addr().
 * 
 * @param msg array of bytes to send
 * @param length number of bytes to send
 */
//void i2c_write(int module, uint8_t msg[], size_t length);



/**
 * Writes a byte to the current slave with a specified send command.
 * The command specifies a sequence of commands to execute automatically
 * in hardware.
 * 
 * @param command an appropriate send command sequence defined in the header 
 */
//void i2c_cmd_write_byte(int module, uint8_t data, uint32_t command);

/**
 * Reads a byte from the current slave with a specified receive command.
 * 
 * @param command an appropriate receive command sequence defined in the header 
 * @return byte read from slave
 */
//uint8_t i2c_cmd_read_byte(int module, uint32_t command);