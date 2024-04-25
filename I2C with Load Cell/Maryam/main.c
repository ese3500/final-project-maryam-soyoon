/*
 * GccApplication1.c
 *
 * Created: 4/15/2024 3:24:55 PM
 * Author : maryam1
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <String.h>
#include "ADC.h"
#include "RTCI2C.h"
#include "I2C.h"
#include "uart.h"

char datalog_info[30];
uint8_t package_down;
uint8_t authorized;
rtc* my_rtc;

void Initialize() {
	// Initialize UART
	UART_init(BAUD_PRESCALER);

	// Set PB4 pin as a pin change
	DDRB &= ~(1 << DDB4);
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT4);
	
	int32_t buffer;
	
	begin();
	
	ADC_Init();

	RTC_Init();

	my_rtc = malloc(sizeof(rtc));

}

ISR(PCINT0_vect) {
	// Do Pin change stuff
}

int main(void)
{
	
    while (1) 
    {
		buffer = ADC_getValue();

		if (buffer < -10000) {
			if (!package_down) {
				RTC_getTime(my_rtc);
				package_down = 1;
				// TO DO, DRIVE PIN HIGH TO SHOW THAT PACKAGE IS DOWN

				sprintf(datalog_info, "%02d:%02d:%02d %02d/%02d%01d%01d", my_rtc->hours, my_rtc->minutes, my_rtc->seconds, my_rtc->month, my_rtc->date, authorized, package_down);
				UART_putstring(datalog_info);
				_delay_ms(5000);
			}
		} else {
			if (package_down) {
				RTC_getTime(my_rtc);
				package_down = 0;
				// TO DO, DRIVE PIN LOW TO SHOW THAT PACKAGE IS UP

				sprintf(datalog_info, "%02d:%02d:%02d %02d/%02d%01d%01d", my_rtc->hours, my_rtc->minutes, my_rtc->seconds, my_rtc->month, my_rtc->date, authorized, package_down);
				UART_putstring(datalog_info);
				_delay_ms(5000);
			}
		}
    }
}