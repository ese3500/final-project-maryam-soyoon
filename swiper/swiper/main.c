/*
 * swiper.c
 *
 * Created: 2024-04-09 오후 5:56:45
 * Author : SOYOON
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"

#include "ST7735.h"
#include "LCD_GFX.h"

static const short NUM[10][7] = {
	// 0, 1, 2, 3, 4, 5, 6
		{1, 1, 1, 0, 1, 1, 1}, // 0
		{0, 0, 1, 0, 0, 1, 0}, // 1
		{1, 0, 1, 1, 1, 0, 1}, // 2
		{1, 0, 1, 1, 0, 1, 1}, // 3
		{0, 1, 1, 1, 0, 1, 0}, // 4
		{1, 1, 0, 1, 0, 1, 1}, // 5
		{1, 1, 0, 1, 1, 1, 1}, // 6
		{1, 1, 1, 0, 0, 1, 0}, // 7
		{1, 1, 1, 1, 1, 1, 1}, // 8
		{1, 1, 1, 1, 0, 1, 0}  // 9
};

static const short NUM_POS[7][8] = {
	// x0    y0    x1    y1
	// l, w, l, w, l, w, l, w
	// 0, 1, 2, 3, 4, 5, 6, 7		// rectangles
		{0, 1, 0, 0, 1, 1, 0, 1}, // 0
		{0, 0, 0, 1, 0, 1, 1, 1}, // 1
		{1, 1, 0, 1, 1, 2, 1, 1}, // 2
		{0, 1, 1, 1, 1, 1, 1, 2}, // 3
		{0, 0, 1, 2, 0, 1, 2, 2}, // 4
		{1, 1, 1, 2, 1, 2, 2, 2}, // 5
		{0, 1, 2, 2, 1, 1, 2, 3}, // 6
};

volatile int key_C = 0; // column pin pulled down in ISR

volatile int num_count = 0; // keep track of numbers printed on screen
volatile int nums[4];
int pw[4] = {1,2,3,4};
volatile int incorrect_count = 0; /// TODO: count number of incorrect submission
volatile int overflow_count = 0;
volatile int disabled = 0; // flag to indicate if alarm is disabled
volatile int buzz = 0; // keeps track of buzzer on/off

void buzz_on() { // buzzer on (50% duty cycle)
	OCR2B = OCR2A / 2;
	buzz = 1;
}

void buzz_off() { // buzzer off
	OCR2B = 0;
	buzz = 0;
}

void start_LCD() {
	lcd_init();

	///set background color to black
	LCD_setScreen(BLACK);
	
	/// TODO: check if string fits in one line
	LCD_drawString(0, 2, "# submit, * clear", WHITE, BLACK);
	LCD_drawLine(1,12,158,12,65535);
}

void Initialize() {
	cli();
	
	DDRC &= ~(1<<DDC1); //PC1 Keypad C1 input
	DDRC &= ~(1<<DDC2); //PC2 Keypad C2 input
	DDRC &= ~(1<<DDC3); //PC3 Keypad C3 input
	///TODO: why can't we set each row as output and just pull them high or low?
	
	DDRC &= ~(1<<DDC4); //PC4 Keypad R1 input
	DDRC &= ~(1<<DDC5); //PC5 Keypad R2 input
	
	DDRD &= ~(1<<DDD2); //PD2 Keypad R3 input
	DDRD &= ~(1<<DDD4); //PD4 Keypad R4 input

	DDRB &= ~(1<<DDB4); //PB4 "lifted" input
	/// TODO: "lifted" output from the other Atmega should be pulled high by default
	
	DDRD |= (1 << DDD7); // PD7 "disabled" output
	PORTD &= ~(1<<PORTD7); // pull PD7 low
	DDRD |= (1 << DDD3); // PD3 buzzer output (OC2B)
	
	PCICR |= (1 << PCIE0);	 // Enable Pin Change Interrupt for PCINT0-7
	//PCICR |= (1 << PCIE1);	 // Enable Pin Change Interrupt for PCINT8-14
	//PCICR |= (1 << PCIE2);	 // Enable Pin Change Interrupt for PCINT16-23
	
	// Enable Pin Change Interrupt for:
	PCMSK0 |= (1 << PCINT4); // PB4

	/*
	PCMSK1 |= (1 << PCINT9); // PC1
	PCMSK1 |= (1 << PCINT10); // PC2
	PCMSK1 |= (1 << PCINT11); // PC3
	*/
	
	//PCMSK1 |= (1 << PCINT12); // PC4
	//PCMSK1 |= (1 << PCINT13); // PC5
	
	//PCMSK2 |= (1 << PCINT18); // PD2
	//PCMSK2 |= (1 << PCINT19); // PD3
	
	// enable pull-up for all keypad inputs
	PORTC |= (1<<PORTC1);
	PORTC |= (1<<PORTC2);
	PORTC |= (1<<PORTC3);
	PORTC |= (1<<PORTC4);
	PORTC |= (1<<PORTC5);
	
	PORTD |= (1<<PORTD2);
	//PORTD |= (1<<PORTD3);
	PORTD |= (1<<PORTD4);
	
	//// Timer2 setup (for buzzer)
	// Set Timer 2 clock to be 250 kHz (prescale 16 MHz / 64)
	TCCR2B |= (1<<CS20);
	TCCR2B |= (1<<CS21);
	TCCR2B &= ~(1<<CS22);
	
	// set timer 2 to Fast PWM mode (Mode 7)
	TCCR2A |= (1<<WGM20);
	TCCR2A |= (1<<WGM21);
	TCCR2B |= (1<<WGM22);
	
	//Clear OC2B on Compare Match, set OC2B at BOTTOM (non-inverting mode)
	TCCR2A &= ~(1<<COM2B0);
	TCCR2A |= (1<<COM2B1);

	OCR2A = 158; // sounds G
	buzz_off(); //OCR2B = 0;
	
	/// TODO: erase below after integration. for testing.
	//OCR2B = OCR2A / 2; // buzzer on (50% duty cycle)
	//buzz_on();

	//// Timer1 setup (for measuring 5 minutes)
	// Set Timer 1 clock to be 15625 Hz (prescale 16 MHz / 1024)
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	// set timer 1 to normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);

	// clear interrupt flags
	TIFR1 |= (1<<TOV1);

	// enable overflow interrupt
	TIMSK1 |= (1 << TOIE1);
	
	// enable global interrupts
	sei();
	
	start_LCD();
}

void clear_num() {
	///set background color to black
	LCD_setScreen(BLACK);
	
	LCD_drawString(0, 2, "# submit, * clear", WHITE, BLACK);
	LCD_drawLine(1,12,158,12,65535);

	if (buzz && !disabled && (PINB & (1<<PINB6))) {
		LCD_drawString(0, 2, "UNAUTHORIZED PICKUP", RED, BLACK);
		} else if (incorrect_count >= 3) {
		/// TODO: check if too long
		LCD_drawString(0, 120, "Incorrect PW 3 or more times!", RED, BLACK);
		} else if (!(PINB & (1<<PINB6))) { // NOT lifted
		/// TODO: check if the position does not mess up
		LCD_drawString(0, 120, "PACKAGE PUT DOWN", WHITE, BLACK);
	}


	// erase all numbers
	//LCD_drawBlock(1, 42, 158, 126, BLACK);
	num_count = 0;

}

ISR(TIMER1_OVF_vect) {
  /// TODO: debugging
	//UART_putstring("Timer 1 overflow\n");
	
	/// TODO: decide if we want to increment ovf counter only when disabled
	overflow_count += 1;
	/// NOTE: 65535/15625 = 4.194 s per overflow (under 15625 Hz clock)

	// 5 minutes: 5*60/4.19424 = 71.52 overflows
	//if (disabled && (overflow_count >= 72)) {
		// 5 minutes passed

	// 1 minute: 60/4.19424 = 14.19 overflows
	if (disabled && (overflow_count >= 15)) {
		// 1 minute passed
		disabled = 0;
		PORTD &= ~(1<<PORTD7); // pull PD7 low
		/// TODO: for testing. remove after confirming.
		//buzz_on();
	}
}

ISR(PCINT0_vect) { //PB4 changed
	if (PINB & (1<<PINB4)) { // PB4 HIGH, "lifted"
		if (!disabled) {
			buzz_on();

			// indicate on screen
			//LCD_setScreen(BLACK);	
			//LCD_drawString(0, 2, "UNAUTHORIZED PICKUP", RED, BLACK);
			// --> will be handled in "clear_num()" function
		}
	} else { // package put down
		clear_num();	
	}
}

/*
ISR(PCINT1_vect) {
	if (!(PINC & (1<<PINC1))) { // C1 is LOW
		key_C = 1;
	} else if (!(PINC & (1<<PINC2))) { // C2 is LOW
		key_C = 2;
	} else if (!(PINC & (1<<PINC3))) { // C3 is LOW
		key_C = 3;
	}
}
*/

/*
void restore_rows() {
	DDRC &= ~(1<<DDC4); //PC4 Keypad R1 input
	DDRC &= ~(1<<DDC5); //PC5 Keypad R2 input
	
	DDRD &= ~(1<<DDD2); //PD2 Keypad R3 input
	DDRD &= ~(1<<DDD3); //PD3 Keypad R4 input
	
	// pull-up
	PORTC |= (1<<PORTC4);
	PORTC |= (1<<PORTC5);
	
	PORTD |= (1<<PORTD2);
	PORTD |= (1<<PORTD3);
}
*/

short read_keypad() {
	// Loop through each row in keypad
	
	//key_C = 0;
	short return_val = -1;
	
	/// R1
	DDRC |= (1<<DDC4); // R1 output
	PORTC &= ~(1<<PORTC4); // pull R1 low
	
	// check which column pin is pulled down
	
	if (!(PINC & (1<<PINC1))) { // C1 is LOW
		UART_putstring("1\n");
		return_val = 1;
	} else if (!(PINC & (1<<PINC2))) {
		UART_putstring("2\n");
		return_val = 2;
	} else if (!(PINC & (1<<PINC3))) {
		UART_putstring("3\n");
		return_val = 3;
	}
	
	/*
	if (key_C == 1) {
		return_val = 1;
	} else if (key_C == 2) {
		return_val = 2;
	} else if (key_C == 3) {
		return_val = 3;
	}
	*/
	
	DDRC &= ~(1<<DDC4); // R1 input
	PORTC &= ~(1<<PORTC4); // pull R1 high
	
	if (return_val >= 0) {
		return return_val;
	}
	
	/// R2
	DDRC |= (1<<DDC5); // R2 output
	PORTC &= ~(1<<PORTC5); // pull R2 low
	
	if (!(PINC & (1<<PINC1))) { // C1 is LOW
		UART_putstring("4\n");
		return_val = 4;
		} else if (!(PINC & (1<<PINC2))) {
		UART_putstring("5\n");
		return_val = 5;
		} else if (!(PINC & (1<<PINC3))) {
		UART_putstring("6\n");
		return_val = 6;
	}
	
	// check which column pin is pulled down
	/*
	if (key_C == 1) {
		return_val = 4;
		} else if (key_C == 2) {
		return_val = 5;
		} else if (key_C == 3) {
		return_val = 6;
	}
	*/
	
	DDRC &= ~(1<<DDC5); // R2 input
	PORTC &= ~(1<<PORTC5); // pull R2 high
	
	if (return_val >= 0) {
		return return_val;
	}
	
	/// R3
	DDRD |= (1<<DDD2); // R3 output
	PORTD &= ~(1<<PORTD2); // pull R3 low
	
	if (!(PINC & (1<<PINC1))) { // C1 is LOW
		UART_putstring("7\n");
		return_val = 7;
		} else if (!(PINC & (1<<PINC2))) {
		UART_putstring("8\n");
		return_val = 8;
		} else if (!(PINC & (1<<PINC3))) {
		UART_putstring("9\n");
		return_val = 9;
	}

	// check which column pin is pulled down
	/*
	if (key_C == 1) {
		return_val = 7;
		} else if (key_C == 2) {
		return_val = 8;
		} else if (key_C == 3) {
		return_val = 9;
	}
	*/
	
	DDRD &= ~(1<<DDD2); // R3 input
	PORTD &= ~(1<<PORTD2); // pull R3 high
	
	if (return_val >= 0) {
		return return_val;
	}
	
	/// R4
	//DDRD |= (1<<DDD3); // R4 output
	//PORTD &= ~(1<<PORTD3); // pull R4 low
	
	DDRD |= (1<<DDD4); // R4 output
	PORTD &= ~(1<<PORTD4); // pull R4 low
	
	if (!(PINC & (1<<PINC1))) { // C1 is LOW
		UART_putstring("*\n");
		return_val = 10;
		} else if (!(PINC & (1<<PINC2))) {
		UART_putstring("0\n");
		return_val = 0;
		} else if (!(PINC & (1<<PINC3))) {
		UART_putstring("#\n");
		return_val = 11;
	}
	
	// check which column pin is pulled down
	/*
	if (key_C == 1) {
		return_val = 10; // *
		} else if (key_C == 2) {
		return_val = 0;
		} else if (key_C == 3) {
		return_val = 11; // #
	}
	*/
	
	//DDRD &= ~(1<<DDD3); // R4 input
	//PORTD &= ~(1<<PORTD3); // pull R4 high
	
	DDRD &= ~(1<<DDD4); // R4 input
	PORTD &= ~(1<<PORTD4); // pull R4 high
	
	return return_val; // -1 if no key was pressed
}

void draw_num(short num) {
	// x spacing = 8, y spacing = 30
	int l = 20; // pixel length of each rectangle 
	int w= 5; // pixel width of each rectangle

	short curr_x0 = 8*num_count + (num_count - 1) * (l + 2*w);
	short curr_y0 = 42; // 12 + 30 = 42	
	
	int i;
	for (i = 0; i<7; i++) {
		if (NUM[num][i]) { // draw corresponding rectangle
			short x0_l = NUM_POS[i][0];
			short x0_w = NUM_POS[i][1];
			short x0 = l * x0_l + w * x0_w + curr_x0;
			
			short y0_l = NUM_POS[i][2];
			short y0_w = NUM_POS[i][3];
			short y0 = l * y0_l + w * y0_w + curr_y0;
			
			short x1_l = NUM_POS[i][4];
			short x1_w = NUM_POS[i][5];
			short x1 = l * x1_l + w * x1_w + curr_x0;
			
			short y1_l = NUM_POS[i][6];
			short y1_w = NUM_POS[i][7];
			short y1 = l * y1_l + w * y1_w + curr_y0;

			LCD_drawBlock(x0, y0, x1, y1, WHITE);
		}
	}
	
}

void submit_num() {
	/// TODO: implement
	
	// if not 4 digits, simply clear the password. 
	if (num_count < 4) {
		clear_num();
	} else {
		/// TODO: check if password is correct, compare with pre-determined value
		for (short i = 0; i < 4; i++) {
			if (nums[i] != pw[i]) { // incorrect password
				/// TODO: potentially indicate "incorrect password" on screen				
				incorrect_count++;
				if (incorrect_count >= 3) {
					// set off the alarm
					buzz_on();
					//LCD_setScreen(BLACK);	

					//LCD_drawString(0, 56, "Incorrect PW 5 times!", WHITE, BLACK);

					/// TODO: check that inputting correct pw will turn it off
					//LCD_drawString(0, 2, "Incorrect PW 5 or more times!", RED, BLACK);
				} //else {
				clear_num();
				//}
				return;
			}
		}
		
		/// Correct password ///
		disabled = 1;
		PORTD |= (1<<PORTD7); // pull PD7 high

		// Write to the screen 
		LCD_setScreen(BLACK);	
		LCD_drawString(0, 56, "Correct PW! Alarm disabled.", WHITE, BLACK);
		
		/// TODO: turn off alarm for 1 minute
		///       After 1 minute, screen returns to initial and alarm back on.
		buzz_off();
		overflow_count = 0;		
		incorrect_count = 0; // reset incorrect password input count
		clear_num();
	}
	
	
}

int main(void)
{
  Initialize();
	
	/// TODO: debugging
	UART_init(BAUD_PRESCALER);
	UART_putstring("Hello World\n");
	
    while (1) 
    {
		// don't read keypad while alarm is disabled.
		if (!disabled) {
			short key = read_keypad();
			
			if (key >= 0) { // a key was read
				if (key == 10) { // *
					clear_num();
				} else if (key == 11) { // #
					submit_num();
				} else if (num_count < 4){
					nums[num_count] = key; // store password number
					num_count++;
					draw_num(key);
				}
				/// NOTE: don't draw anything if num_count == 4 
				// 				and a number was pressed

				//char num[10];
				//sprintf(num, "%d", key);
				//UART_putstring(num);
				//UART_putstring("\n");
			}
		}

		_delay_ms(250);	
  }
}

