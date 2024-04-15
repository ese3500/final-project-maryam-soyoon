#include "i2c.h"
#include "util/delay.h"

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
	
	if ((TWSR0 & 0xF8) != TW_START && (TWSR0 & 0xF8) != TW_REP_START) {
		while(1);
	}
}

void stop() {
    TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	// while (!(TWCR0 & (1 << TWINT)));
}

void send_address(uint8_t device_address, int rw) {
    TWDR0 = (device_address << 1) + rw;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT)));    // Wait for acknowledge
	
// 	if ((TWSR0 & 0xF8) != TW_MT_SLA_ACK) {
// 		while(1);
// 	}
	
	if (rw == 1) {
		DDRC &= ~(1 << DDC4);
	} else {
		DDRC |= (1 << DDC4);
	}
	
    return;
}

void send_reg_address(uint8_t address) {
	TWDR0 = address;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
	
	if ((TWSR0 & 0xF8) != TW_MT_DATA_ACK) {
		while(1);
	}
	
	return;
}

void ack() {
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR0 & (1 << TWINT)));
}

void nack() {
    // Send a NACK
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
}

uint8_t read_byte(uint8_t device_address, uint8_t register_address) {
	// Send start condition
	start();

	// Send device address
	send_address(device_address, 0);

	// Send register address
	send_reg_address(register_address);
	
// 	stop();
// 	_delay_ms(10);

	// Send repeated start
	start();

	// Send device address
	send_address(device_address, 1);
	
 	nack();

 	uint8_t buffer = TWDR0;
	
	stop();

	return buffer;
}

uint8_t read() {
	uint8_t buffer = TWDR0;

    // Send acknowledge to peripheral
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR0 & (1 << TWINT)));    // Wait for completion
	
// 	if ((TWSR0 & 0xF8) != TW_MR_DATA_ACK) {
// 		while(1);
// 	}
	
    return buffer;
}

void write(uint8_t device_address, uint8_t reg_address, uint8_t data) {
    start();

    // Write the device address
    send_address(device_address, 0);

    // Write reg address
    send_reg_address(reg_address);

    // Write the data
    TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
	
	if ((TWSR0 & 0xF8) != TW_MT_DATA_ACK) {
		while(1);
	}

    // Generate stop
    stop();

}

void burst_write(uint8_t reg_address, uint8_t data) {
	// Send register address
	send_reg_address(reg_address);

	// Write data
	TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));

	if ((TWSR0 & 0xF8) != TW_MT_DATA_ACK) {
		while(1);
	}
}

