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
volatile uint8_t package_down;
volatile uint8_t authorized;
volatile int overflow_count = 0;
//volatile int detected = 0;
volatile int adc_count = 0;
//volatile int32_t adc_sum = 0;
rtc* my_rtc;
int32_t buffer;

void Initialize() {
	cli();

	DDRB &= ~(1 << DDB2); // PB2 "authorized" input
	// Enable pin change interrupt for PB2
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT2);

	DDRB |= (1<<DDB1); // PB1 "lifted" output
	PORTB |= (1<<PORTB1); // pull PB1 high

	/*
	//// Timer1 setup (for measuring 5 seconds)
	// Set Timer 1 clock to be 250 kHz (prescale 16 MHz / 64)
	TCCR1B |= (1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	// set timer 1 to normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);

	// clear interrupt flags
	TIFR1 |= (1<<TOV1);

	// enable overflow interrupt
	TIMSK1 |= (1 << TOIE1);
	*/

	/// TODO: check this doesn't break things
	sei(); 

	// Initialize UART
	UART_init(BAUD_PRESCALER);	
	begin();
	
	ADC_Init();

	RTC_Init();

	my_rtc = malloc(sizeof(rtc));
}

/*
ISR(TIMER1_OVF_vect) {
  /// TODO: debugging
	UART_putstring("Timer 1 overflow\n");

	/// NOTE: 65535/250(k) = 262.14 ms per overflow (under 250 kHz clock)	
	overflow_count += 1;
	
	// 5 seconds: 5000 ms / 262.14 ms = 19.07 overflows
	if (detected && (overflow_count >= 19)) { // 5 seconds passed
		/// TODO: check if 
		
		disabled = 0;
		PORTB &= ~(1<<PORTB7); // pull PB7 low
	}
}
*/

ISR(PCINT0_vect) { // PB2 "authorized" changed
	/// TODO: Pin change stuff
	if (PINB & (1<<PINB2)) { // authorized
		authorized = 1;
	} else { // not authorized
		authorized = 0;
	}
}


int main(void)
{
	Initialize();
	
    while (1) 
    {
			buffer = ADC_getValue();

			if (buffer < -10000) {
				/// TODO: check if certain plus delta value could be the case with different position
				// package should exceed threshold for 5 seconds
				if (!package_down) {
					adc_count++;
					if (adc_count >= 10) {
					RTC_getTime(my_rtc);
					package_down = 1;
					PORTB &= ~(1<<PORTB1); // pull PB1 low (Not lifted)

					sprintf(datalog_info, "%02d:%02d:%02d %02d/%02d%01d%01d", my_rtc->hours, my_rtc->minutes, my_rtc->seconds, my_rtc->month, my_rtc->date, authorized, package_down);
					UART_putstring(datalog_info);
					//_delay_ms(5000);
					//_delay_ms(500);
					}
				}
			} else {
				adc_count = 0; 
				/// NOTE: ADC range [-10000, 3000] is ignored either side
				if ((buffer > 3000) && package_down) {
					RTC_getTime(my_rtc);
					package_down = 0;
					PORTB |= (1<<PORTB1); // pull PB1 high (lifted)

					sprintf(datalog_info, "%02d:%02d:%02d %02d/%02d%01d%01d", my_rtc->hours, my_rtc->minutes, my_rtc->seconds, my_rtc->month, my_rtc->date, authorized, package_down);
					UART_putstring(datalog_info);
					//_delay_ms(5000);
					//_delay_ms(500);
				}
			}
			_delay_ms(500); // measure load cell every 0.5 seconds
    }
}