#include "i2c.h"

// Source: https://ww1.microchip.com/downloads/en/DeviceDoc/40001906A.pdf 

void begin() {
    TWCR1 |= (1 << TWINT);   // Enable TWI Interrupt Flag
    TWCR1 |= (1 << TWSTA);   // Set MCU as Controller
    TWCR1 |= (1 << TWEN);    // Enable TWI Operation

    // Set the clock
    // 16,000,000 / (16 + 2(150) + 1) = 50,473 Hz
    TWBR1 = 150;

    while (!(TWCR1 & (1<<TWINT)));
}

void end() {
    TWCR1 |= (1<<TWINT);
    TWCR1 |= (1<<TWEN);
    TWCR1 |= (1<<TWSTO);
}

void start_read(uint8_t device_address) {
    TWDR1 = device_address + 0x01;
    TWCR1 |= (1 << TWINT);
    TWCR1 |= (1 << TWEN);

    while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

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
    TWCR1 &= ~(1 << TWEA);
}

uint8_t read(uint8_t reg_address) {
    uint8_t buffer = TWDR1;

    // Send acknowledge to peripheral
    TWCR1 |= (1 << TWEA);

    return buffer;
}

// void start_write(uint8_t device_address) {
//     TWDR1 = device_address;
//     TWCR1 |= (1 << TWINT);
//     TWCR1 |= (1 << TWEN);

//     while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge
// }

void write(uint8_t device_address, uint8_t reg_address, uint8_t data) {
    // Start the I2C
    begin();

    // Write the device address
    TWDR1 = device_address;
    TWCR1 |= (1 << TWINT);
    TWCR1 |= (1 << TWEN);

    while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

    // Write reg address
    TWDR1 = reg_address;
    TWCR1 |= (1 << TWINT);
    TWCR1 |= (1 << TWEN);

    while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

    // Write the data
    TWDR1 = data;
    TWCR1 |= (1 << TWINT);
    TWCR1 |= (1 << TWEN);

    while (!(TWCR1 & (1 << TWINT)));    // Wait for acknowledge

    // Generate stop
    end();

}

