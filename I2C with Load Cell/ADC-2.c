#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

// YOU ARE SO GREAT MARYAM

#include "ADC.h"
#include "I2C.h"
#include "stdbool.h"
#include "uart.h"
#include <util/delay.h>
#include <avr/io.h>

#define ADC_ADDR 0x2A
char String[25];

void ADC_Init() {
	UART_init(BAUD_PRESCALER);
	
	// Reset
	write(ADC_ADDR, 0x00, 0x01);
	
	_delay_ms(10);
	
	write(ADC_ADDR, 0x00, 0x02);
	
	_delay_ms(1);
	
	
	// Enable
	write(ADC_ADDR, 0x00, 0x06);
	
	_delay_ms(100);
	
	write(ADC_ADDR, 0x00, 0x16);
	
	uint8_t buffer = read_byte(ADC_ADDR, 0x1F);
	if ((buffer & 0xF) != 0xF) {
		while(1);
	}
	
	
	// Set LDO
	write(ADC_ADDR, 0x00, 0x96);
	
	write(ADC_ADDR, 0x01, 0x07);
	
	
	// Set Rate
	write(ADC_ADDR, 0x02, 0x00);
	
	
	// Write to ADC reg
	write(ADC_ADDR, 0x15, 0x30);
	
	// Write to PGA reg
	write(ADC_ADDR, 0x1B, 0x00);
	
	// Write to PWR reg
	write(ADC_ADDR, 0x1C, 0x80);
	
	
	
	// Flush out 10 readings
//  	for (int i = 0; i < 10; i++) {
//  		read_byte(ADC_ADDR, 0x12);
//  		read_byte(ADC_ADDR, 0x13);
//  		read_byte(ADC_ADDR, 0x14);
//  	}

	// Register 2 (SPS set to 10, offset internal calibration)
	write(ADC_ADDR, 0x02, 0x14);

	// Register 2 (SPS set to 10, offset system calibration)
	write(ADC_ADDR, 0x02, 0x16);

}

int32_t ADC_getValue() {
	uint8_t buffer = 0;
	int32_t value;
	uint32_t temp_value;
	
	while (buffer != 1) {
		// Read from DRDY register (register 0x00)
		buffer = read_byte(ADC_ADDR, 0x00);
		// Extract DRDY
		buffer = (buffer >> 5) & 1;
	}
	
 	temp_value = read_byte(ADC_ADDR, 0x12);
	
	value = temp_value << 16;
	
	temp_value = read_byte(ADC_ADDR, 0x13);
	
	value |= (temp_value << 8);
	
	temp_value = read_byte(ADC_ADDR, 0x14);
	
	value |= temp_value;
	
	if (value & 0x800000) {
		value |= 0xFF000000;
	}

	// value = burst_read(ADC_ADDR, 0x12);

// 	sprintf(String, "%ld\n", value);
// 	UART_putstring(String);
	
	return value;
	
	// return buffer;
}