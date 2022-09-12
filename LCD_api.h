
#ifndef LCD_API_H
#define LCD_API_H

#include "RA8876.h"
#include "FT5316.h"

/*

service
  init
  
page tools
  set page
  clear page

color tools
  set color

text tools
  print text

geometry tools

*/


void LCD_init(void);

void LCD_setPage(unsigned long layer);
void LCD_cleanCurrentPage(unsigned short color);

void LCD_printString1(char* string, unsigned short x, unsigned short y, unsigned short backgroundColor, unsigned short textColor);

void LCD_drawSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawFilledSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawFilledCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);


void LCD_drawBitmap(uint16_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_drawBitmap2(uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_drawBitmap3(uint16_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);


#endif
