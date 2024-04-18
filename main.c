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
#include "RTCI2C.h"
#include "I2C.h"
#include "uart.h"

char String[25];

int drdy;	// 0 if data not ready, 1 if data ready

ISR(PCINT0_vect) {
	drdy ^= 1;
}

int main(void)
{
	UART_init(BAUD_PRESCALER); 
	// Set DRDY pin as a pin change
	DDRB &= ~(1 << DDB4);
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT4);
	
	drdy = 0;
	
	uint8_t buffer;
	
	rtc* my_rtc = malloc(sizeof(rtc));
	
	begin();
	
	RTC_Init();
	
	RTC_getTime(my_rtc);

    while (1) 
    {
		RTC_getTime(my_rtc);
		sprintf(String, "%02d\n", my_rtc->seconds);
		UART_putstring(String);
		
		sprintf(String, "%02d\n", my_rtc->minutes);
		UART_putstring(String);
		
// 		sprintf(String, "%02d\n", my_rtc->hours);
// 		UART_putstring(String);
		_delay_ms(5);
    }
}