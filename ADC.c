#include "ADC.h"
#include "I2C.h"
#include "stdbool.h"
#include <util/delay.h>
#include <avr/io.h>

#define ADC_ADDR 0x2A

void ADC_Init() {
	
// 	start();
// 
//     // Set up control regs
//     send_address(ADC_ADDR, 0);

	// Register 1 (LDO set to 4.5 and gain select set to 4)
	write(ADC_ADDR, 0x00, 0x06);

    // Register 1 (LDO set to 4.5 and gain select set to 4)
    write(ADC_ADDR, 0x01, 0x02);

    // Register 2 (SPS set to 40, offset internal calibration)
    write(ADC_ADDR, 0x02, 0x24);

    _delay_ms(1000);

    // Register 2 (SPS set to 40, offset system calibration)
    write(ADC_ADDR, 0x02, 0x26);

    _delay_ms(1000);

    stop();
}

uint32_t ADC_getValue() {
    uint32_t buffer = 0;
	uint32_t value;
	
	while (buffer == 0) {
		// Read from DRDY register (register 0x00)
		buffer = read_byte(ADC_ADDR, 0x00);
		// Extract DRDY
		buffer = (buffer >> 5) & 0x01;
	}
	
    buffer = read_byte(ADC_ADDR, 0x14);
	
	value = buffer;
	
	buffer = read_byte(ADC_ADDR, 0x13);
	
	value |= (buffer << 8);
	
	buffer = read_byte(ADC_ADDR, 0x12);
	
	value |= (buffer << 16);
	
	if (value & 0x800000) {
		value |= 0xFF000000;
	}
	
	return value;
	
	// return buffer;
}