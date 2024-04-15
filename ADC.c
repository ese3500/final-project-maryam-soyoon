#include "ADC.h"
#include "stdbool.h"
#include <util/delay.h>

#define ADC_ADDR 0x2A

void ADC_Init() {
    // Srt DRDY pin as a pin change
    DDRB &= ~(1 << DDB4);

    // Set up control regs
    send_address(ADC_ADDR, 0);

    // Register 0 (LDO set to 4.5 and gain select set to 4)
    burst_write(0x00, 0x02);

    // Register 1 (SPS set to 40, offset internal calibration)
    burst_write(0x01, 0x24);

    _delay_ms(1000);

    // Register 1 (SPS set to 40, offset system calibration)
    burst_write(0x01, 0x26);

    _delay_ms(1000);

    stop();
}

void ADC_getValue() {
    uint8_t buffer;
    
    // Read from DRDY register (register 0x00)
    buffer = read_byte(ADC_ADDR, 0x00);
    // Extract DRDY
    uint8_t temp = (buffer >> 6) & 0x01;

    start_read(ADC_ADDR);
}