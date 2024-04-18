#include "ADC.h"
#include "I2C.h"
#include "stdbool.h"
#include <util/delay.h>
#include <avr/io.h>

#define ADC_ADDR 0x2A

void ADC_Init() {

	// Register 1 (LDO set to 4.5 and gain select set to 4)
	write(ADC_ADDR, 0x00, 0x06);
	
	// write(ADC_ADDR, 0x11, 0x80);

	// Register 1 (LDO set to 4.5 and gain select set to 4)
	write(ADC_ADDR, 0x01, 0x00);

	// Register 2 (SPS set to 10, offset internal calibration)
	//write(ADC_ADDR, 0x02, 0x04);

	_delay_ms(1);

	// Register 2 (SPS set to 10, offset system calibration)
	write(ADC_ADDR, 0x02, 0x06);

	_delay_ms(1);

	stop();
}

uint32_t ADC_getValue() {
	uint8_t buffer = 0;
	uint32_t value;
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
	
	return value;
	
	// return buffer;
}