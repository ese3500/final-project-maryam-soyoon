/*
 * GccApplication1.c
 *
 * Created: 4/23/2024 5:16:31 PM
 * Author : maryam1
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "I2C.h"
#include "RTCI2C.h"

char temp[20];

int main(void)
{
    /* Replace with your application code */
	UART_init(BAUD_PRESCALER);
	
	rtc* my_rtc = malloc(sizeof(rtc));
	
	begin();
	
	RTC_getTime(my_rtc);
	
	sprintf(temp, "%d", 9);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->seconds);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->minutes);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->hours);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->day);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->date);
	UART_putstring(temp);
	
	sprintf(temp, "%02d", my_rtc->month);
	UART_putstring(temp);
	
    while (1) 
    {
    }
}

