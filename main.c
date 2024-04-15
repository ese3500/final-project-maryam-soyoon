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
#include <stdio.h>
#include "ADC.h"
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
	
	begin();
	
	// start();
	
 	ADC_Init();
	 
// 	uint32_t temp = ADC_getValue();
//  	sprintf(String, "Value is: %ld\n", temp);
//  	UART_putstring(String);
	
	while(1) {
		uint32_t temp = ADC_getValue();
		sprintf(String, "Value is: %ld\n", temp);
		UART_putstring(String);
	}
// 	
// 	while (drdy) {
// 		sprintf(String, "Here\n");
// 		UART_putstring(String);
// 		uint32_t temp = ADC_getValue();	
// 	}
    /* Replace with your application code */
    while (1) 
    {
    }
}

