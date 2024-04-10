#include <avr/io.h>
#include <avr/io.h>
#include <avr/twi.h>
#include <stdbool.h>

/*
Initialize function to begin I2C Communication
*/
void begin();

/*
Stop I2C Communication
*/
void stop();

/*
Reads data from peripheral denoted by device_address and stores it into buffer
*/
bool read(uint8_t device_address, int num_bytes);
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
int get_status();
