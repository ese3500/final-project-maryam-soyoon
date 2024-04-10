#include "I2C.h"
#include "stdbool.h"

#define ADC_ADDR 0x2B // 0x2A is device address and a 1 is for read mode since we always read from our ADC

void get_value() {
    start_read(ADC_ADDR);
}