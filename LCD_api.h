
#ifndef LCD_API_H
#define LCD_API_H

#include "RA8876.h"
#include "FT5316.h"


#define MODE_8BPP 0
#define MODE_16BPP 1

#define FONT_SIZE_8X16 1
#define FONT_SIZE_12X24 2
#define FONT_SIZE_16X32 3

#define FONT_WIDTH_X1 1
#define FONT_WIDTH_X2 2
#define FONT_WIDTH_X3 3
#define FONT_WIDTH_X4 4

#define FONT_HEIGHT_X1 1
#define FONT_HEIGHT_X2 2
#define FONT_HEIGHT_X3 3
#define FONT_HEIGHT_X4 4


void LCD_init(void);

void LCD_setPage(unsigned long page);
void LCD_showPage(unsigned long page);
void LCD_cleanCurrentPage(unsigned short color);

void LCD_printString(char* string,
                     unsigned short x, unsigned short y,
                     unsigned short size, unsigned short wScaler, unsigned short hScaler,
                     unsigned short backgroundColor, unsigned short textColor);

void LCD_drawSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawFilledSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color);
void LCD_drawCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawFilledCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color);
void LCD_drawEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);
void LCD_drawFilledEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color);

void LCD_drawBitmap(unsigned char* pixels, unsigned short pictureBpp,
                    unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void LCD_copyArea(unsigned long sourcePage, unsigned short sourcePageBpp,
                  unsigned long destPage, unsigned short destPageBpp,
                  unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void LCD_drawBitmapPageBuf(unsigned char* pixels, unsigned short pictureBpp,
                           unsigned long bufPage, unsigned long destPage, unsigned short destPageBpp,
                           unsigned short x, unsigned short y, unsigned short w, unsigned short h);

void Touch_init(void);
void Touch_handler(void);
bool Touch_sampleTouch(unsigned short* x, unsigned short* y);


#endif
