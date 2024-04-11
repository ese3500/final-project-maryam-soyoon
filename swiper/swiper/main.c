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
		{0, 1, 0, 0, 1, 0, 0}, // 1
		{1, 0, 1, 1, 1, 0, 1}, // 2
		{1, 0, 1, 1, 0, 1, 1}, // 3
		{0, 1, 1, 1, 0, 1, 0}, // 4
		{1, 1, 0, 1, 0, 1, 1}, // 5
		{1, 1, 0, 1, 1, 1, 1}, // 6
		{1, 1, 1, 0, 0, 1, 0}, // 7
		{1, 1, 1, 1, 1, 1, 1}, // 8
		{1, 1, 1, 1, 0, 1, 0}  // 9
}

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
}

volatile int key_C = 0; // column pin pulled down in ISR

volatile int num_count = 0; // keep track of numbers printed on screen

void start_LCD() {
	lcd_init();

	///set background color to black
  LCD_setScreen(BLACK);
	
	/// TODO: check if string fits in one line
	LCD_drawString(0, 2, "Enter passcode. # submit, * clear", WHITE, BLACK);
	LCD_drawLine(1,12,158,12,65535);
}

void Initialize() {
	DDRC &= ~(1<<DDC1); //PC1 Keypad C1 input
	DDRC &= ~(1<<DDC2); //PC2 Keypad C2 input
	DDRC &= ~(1<<DDC3); //PC3 Keypad C3 input
	///TODO: why can't we set each row as output and just pull them high or low?
	
	DDRC &= ~(1<<DDC4); //PC4 Keypad R1 input
	DDRC &= ~(1<<DDC5); //PC5 Keypad R2 input
	
	DDRD &= ~(1<<DDD2); //PD2 Keypad R3 input
	DDRD &= ~(1<<DDD3); //PD3 Keypad R4 input
	
	/*
	DDRC |= (1<<DDC4); //PC4 Keypad R1 output
	DDRC |= (1<<DDC5); //PC5 Keypad R2 output
	
	DDRD |= (1<<DDD2); //PD2 Keypad R3 output
	DDRD |= (1<<DDD3); //PD3 Keypad R4 output
	*/
	
	/// TODO: comment out / remove line below if we drop ISR
	PCICR |= (1 << PCIE1);	 // Enable Pin Change Interrupt for PCINT8-14
	//PCICR |= (1 << PCIE2);	 // Enable Pin Change Interrupt for PCINT16-23
	
	// Enable Pin Change Interrupt for:
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
	PORTD |= (1<<PORTD3);

	start_LCD();
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
	DDRD |= (1<<DDD3); // R4 output
	PORTD &= ~(1<<PORTD3); // pull R4 low
	
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
	
	DDRD &= ~(1<<DDD3); // R4 input
	PORTD &= ~(1<<PORTD3); // pull R4 high
	
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

void clear_num() {
	/// TODO: check this works

	// erase all numbers
  LCD_drawBlock(1, 42, 158, 126, BLACK);

}

void submit_num() {
	/// TODO: implement
	clear_num();
}

int main(void)
{
  Initialize();
	
	/// TODO: debugging
	UART_init(BAUD_PRESCALER);
	UART_putstring("Hello World\n");
	
    while (1) 
    {
		short key = read_keypad();
		
		if (key >= 0) { // a key was read
			if (key == 10) { // *
				clear_num();
			} else if (key == 11) { // #
				submit_num();
			} else if (num_count < 4){
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
}

