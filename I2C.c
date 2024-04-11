#include "i2c.h"

// Source: https://ww1.microchip.com/downloads/en/DeviceDoc/40001906A.pdf 

void begin() {
	// Set up PC4 and Pc5 pins
	DDRC |= (1 << DDC4) | (1 << DDC5);
	PORTC |= (1 << PINC4) | (1 << PINC5);
	DDRC &= ~(1 << DDC4) | ~(1 << DDC5);
	
    // Set the clock
    // 16,000,000 / (16 + 2(150) + 1) = 50,473 Hz
    TWBR0 = 150;
}

void start() {
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR0 & (1<<TWINT)));
}

void stop() {
    TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void start_read(uint8_t device_address) {
    TWDR0 = device_address + 0x01;
    TWCR0 |= (1 << TWINT);
    TWCR0 |= (1 << TWEN);

    while (!(TWCR0 & (1 << TWINT)));    // Wait for acknowledge

    // // Write reg_addres
    // TWDR1 = reg_address;
    // TWCR1 |= (1 << TWINT);
    // TWCR1 |= (1 << TWEN);

    // while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

    // // Resend start
    // TWCR1 |= (1 << TWINT);   // Enable TWI Interrupt Flag
    // TWCR1 |= (1 << TWSTA);   // Set MCU as Controller
    // TWCR1 |= (1 << TWEN);    // Enable TWI Operation

    // TWDR1 = device_address + 0x01;

    // while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

    return;
}

void stop_read() {
    // Send a NACK
    TWCR0 &= ~(1 << TWEA);
}

uint8_t read() {
    uint8_t buffer = TWDR0;

    // Send acknowledge to peripheral
    TWCR0 |= (1 << TWEA);

    return buffer;
}

void write(uint8_t device_address, uint8_t reg_address, uint8_t data) {
    start();

    // Write the device address
    TWDR0 = device_address;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT)));    // Wait for acknowledge

    // Write reg address
    TWDR0 = reg_address;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT)));    // Wait for acknowledge

    // Write the data
    TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));

    // Generate stop
    stop();

}

