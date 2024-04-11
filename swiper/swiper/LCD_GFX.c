/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <math.h>
#include <stdlib.h>

/******************************************************************************
* Local Functions
******************************************************************************/
void plotLineLow(short x0, short y0, short x1, short y1, uint16_t color) {
	short dx = x1 - x0;
	short dy = y1 - y0;
	short yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	short D = (2 * dy) - dx;
	short y = y0;

	for (short x = x0; x <= x1; x++) {
		LCD_drawPixel(x, y, color);
		if (D > 0) {
			y = y + yi;
			D = D + (2 * (dy - dx));
		} else {
			D = D + 2*dy;
		}
	}
}

void plotLineHigh(short x0, short y0, short x1, short y1, uint16_t color) {
	short dx = x1 - x0;
	short dy = y1 - y0;
	short xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	short D = (2 * dx) - dy;
	short x = x0;
	for (short y = y0; y <= y1; y++) {
		LCD_drawPixel(x, y, color);
		if (D > 0) {
			x = x + xi;
			D = D + (2 * (dx - dy));
		} else {
			D = D + 2*dx;
		}
	}
}
/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&1==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{	
	// accounts for edge
	if ((x0 < radius) || (y0 < radius) || (LCD_WIDTH < x0 + radius) || 
			(LCD_HEIGHT < y0 + radius)) { // circle does not fit into screen
		return; // draw nothing
	}

	uint8_t x_start = x0 - radius;
	uint8_t x_end = x0 + radius;
	uint8_t x, x_right, y_top, y_bot;
	double y_top_real, y_bot_real, y0_real;

	// Middle vertical line
	LCD_drawBlock(x0, y0 - radius, x0, y0 + radius, color);

	for (uint8_t i = 0; i < radius; i++) {
		x = x_start + i;
		x_right = x_end - i;
		y0_real = y0 + 0.5;
		
		y_top_real = sqrt(pow(radius, 2) - pow((x - x0), 2)) + y0_real;
		y_bot_real = -sqrt(pow(radius, 2) - pow((x - x0), 2)) + y0_real;
	
		y_top = (uint8_t) floor(y_top_real);
		y_bot = (uint8_t) floor(y_bot_real);
		
		LCD_drawBlock(x, y_bot, x, y_top, color);
		LCD_drawBlock(x_right, y_bot, x_right, y_top, color);
	}
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color	
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	if ((x0 < 0) || (y0 < 0) || (x1 < 0) || (y1 < 0) || (x0 > LCD_WIDTH) || 
			(x1 > LCD_WIDTH) || (y0 > LCD_HEIGHT) || (y1 > LCD_HEIGHT)) { 
			// line does not fit into screen
		return; // draw nothing
	}
	
	if (x0 == x1) { // vertical line
		
		uint8_t y0_new = y0;
		uint8_t y1_new = y1;
		if (y1 < y0) { // swap two
		//uint8_t y_temp = y1;
		y0_new = y1;
		y1_new = y0;
	}		
		LCD_drawBlock(x0, y0_new, x1, y1_new, c);
	} else { // Bresenham's line algorithm
		if (abs(y1 - y0) < abs(x1 - x0)) {
			if (x0 > x1) {
					plotLineLow(x1, y1, x0, y0, c);
			} else {
					plotLineLow(x0, y0, x1, y1, c);
			}
		} else {
			if (y0 > y1) {
				plotLineHigh(x1, y1, x0, y0, c);
			} else {
				plotLineHigh(x0, y0, x1, y1,c);
			}
		}
	}
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	
	if ((x0 < 0) || (y0 < 0) || (x1 < 0) || (y1 < 0) || (x0 > LCD_WIDTH) || 
			(x1 > LCD_WIDTH) || (y0 > LCD_HEIGHT) || (y1 > LCD_HEIGHT)) { 
			// line does not fit into screen
		return; // draw nothing
	}

	LCD_setAddr(x0,y0,x1,y1);
	
	int num_pixels = abs((x1 - x0 + 1) * (y1 - y0 + 1));

	for (uint8_t i = 0; i < num_pixels; i++) {
		SPI_ControllerTx_16bit(color);
	}
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{

	/*
	for (uint8_t x = 0; x < LCD_WIDTH; x++) {
		for (uint8_t y = 0; y < LCD_HEIGHT; y++) {
			LCD_drawPixel(x,y,color);
		}
	}
	*/

	LCD_setAddr(0, 0, 79, LCD_HEIGHT - 1);
	
	for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT / 2; i++) {
		SPI_ControllerTx_16bit(color);
	}

	LCD_setAddr(80, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	
	for (int j = 0; j < LCD_WIDTH * LCD_HEIGHT / 2; j++) {
		SPI_ControllerTx_16bit(color);
	}
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	int8_t x_curr, y_curr;
	// assume y coordinate stays the same
	x_curr = x;
	y_curr = y;
	for (int i = 0; str[i] != '\0'; i++) {
		x_curr += 5;
		LCD_drawChar(x_curr, y_curr, (uint16_t) str[i], fg, bg);
	}
}