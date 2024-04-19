#include <avr/io.h>
#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

// YOU ARE SO GREAT MARYAM

/*
Begin I2C Communication
*/
void begin();

/*
Send start signal
*/
void start();

/*
End I2C Communication
*/
void stop();

/*
Starts the read by writing the device address
*/
void send_address(uint8_t device_address, int rw);

void send_reg_address(uint8_t address);

/*
Stop the read by writing a NACK
*/
void stop_read();

/*
Read from the TWDR and send an acknowledge bit
Returns what was sent by peripheral
*/
uint8_t read();

uint8_t read_byte(uint8_t device_address, uint8_t register_address);

/*
Write a byte into a peripheral
*/
void write(uint8_t device_address, uint8_t reg_address, uint8_t data);

// bool read(uint8_t device_address, uint8_t *buffer, size_t len);

/*
Writes data in buffer to peripheral denoted by device_address
*/
// bool write(int device_address, uint8_t *buffer);

/*
Returns status of the peripheral

1 - Peripheral busy

0 - Peripheral free
*/
// int get_status();

uint32_t burst_read(uint8_t device_address, uint8_t reg_address);
