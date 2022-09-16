
#ifndef LCD_API_H
#define LCD_API_H

#include "RA8876.h"
#include "FT5316.h"


void LCD_init(void);

void LCD_setPage(unsigned long page);
void LCD_cleanCurrentPage(unsigned short color);

void LCD_printString(char* string, unsigned short x, unsigned short y, unsigned short backgroundColor, unsigned short textColor);

void LCD_drawSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawFilledSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawFilledCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);

void LCD_drawBitmap(unsigned short* pixels, unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void LCD_copyArea(unsigned long sourcePage, unsigned long destPage,
                  unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void LCD_drawBitmapPageBuf(unsigned short* pixels,
                           unsigned long bufPage, unsigned long destPage,
                           unsigned short x, unsigned short y, unsigned short w, unsigned short h);

void Touch_init(void);
void Touch_handler(void);
bool Touch_sampleTouch(unsigned short* x, unsigned short* y);


#endif
