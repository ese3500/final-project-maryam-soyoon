#include <stdint.h>

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year; 	
} rtc;

uint8_t hex2bcd(uint8_t val);

uint8_t bcd2hex(uint8_t val);

void RTC_Init();

uint8_t bcd_to_decimal(uint8_t input);

void RTC_getTime(rtc* my_rtc);