
#include "LCD_api.h"


void LCD_init(void)
{
  delay_ms(100);                     //delay for RA8876 power on
  RA8876_HW_Reset();
  delay_ms(100);
  while(LCD_StatusRead()&0x02) {;}   //Initial_Display_test	and  set SW2 pin2 = 1

  RA8876_initial();
  Display_ON();
  delay_ms(20);

  Foreground_color_65k(White);
  Line_Start_XY(0,0);
  Line_End_XY(1023,599);
  Start_Square_Fill();
}


void LCD_setPage(unsigned long page)
{
  Select_Main_Window_16bpp();
  Main_Image_Start_Address(page);
  Main_Image_Width(1024);
  Main_Window_Start_XY(0,0);
  Canvas_Image_Start_address(page);
  Canvas_image_width(1024);
  Active_Window_XY(0,0);
  Active_Window_WH(1024,600);
}


void LCD_cleanCurrentPage(unsigned short color)
{
  Foreground_color_65k(color);
  Line_Start_XY(0,0);
  Line_End_XY(1023,599);
  Start_Square_Fill();
}


void LCD_printString(char* string, unsigned short x, unsigned short y, unsigned short backgroundColor, unsigned short textColor)
{
  Foreground_color_65k(textColor);
  Background_color_65k(backgroundColor);
  CGROM_Select_Internal_CGROM();
  Font_Select_12x24_24x24();
  Goto_Text_XY(x, y);
  Show_String(string);
}


void LCD_drawSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color)
{
  Foreground_color_65k(color);
  Line_Start_XY(x0, y0);
  Line_End_XY(x1, y1);
  Start_Square();
}


void LCD_drawCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color)
{
  Foreground_color_65k(color);
  Line_Start_XY(x0, y0);
  Line_End_XY(x1, y1);
  Circle_Square_Radius_RxRy(rx, ry);
  Start_Circle_Square();
}

void LCD_drawFilledCircleSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short rx, unsigned short ry, unsigned short color)
{
  Foreground_color_65k(color);
  Line_Start_XY(x0, y0);
  Line_End_XY(x1, y1);
  Circle_Square_Radius_RxRy(rx, ry);
  Start_Circle_Square_Fill();
}


void LCD_drawFilledSquare(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short color)
{
  Foreground_color_65k(color);
  Line_Start_XY(x0, y0);
  Line_End_XY(x1, y1);
  Start_Square_Fill();
}


void LCD_drawCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color)
{
  Foreground_color_65k(color);
  Circle_Center_XY(x, y);
  Circle_Radius_R(r);
  Start_Circle_or_Ellipse();
}

void LCD_drawFilledCircle(unsigned short x, unsigned short y, unsigned short r, unsigned short color)
{
  Foreground_color_65k(color);
  Circle_Center_XY(x, y);
  Circle_Radius_R(r);
  Start_Circle_or_Ellipse_Fill();
}


void LCD_drawEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color)
{
  Foreground_color_65k(color);
  Circle_Center_XY(x, y);
  Ellipse_Radius_RxRy(rx, ry);
  Start_Circle_or_Ellipse();
}

void LCD_drawFilledEllipse(unsigned short x, unsigned short y, unsigned short rx, unsigned short ry, unsigned short color)
{
  Foreground_color_65k(color);
  Circle_Center_XY(x, y);
  Ellipse_Radius_RxRy(rx, ry);
  Start_Circle_or_Ellipse_Fill();
}


void LCD_drawBitmap(unsigned short* pixels, unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
  Active_Window_XY(x, y);
  Active_Window_WH(w, h);
  Goto_Pixel_XY(x, y);
  Show_picture(w*h, pixels);
  Active_Window_XY(0,0);
  Active_Window_WH(1024,600);
}


void LCD_copyArea(unsigned long sourcePage, unsigned long destPage,
                  unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
  //BTE memory(move) from source page to dest page
  BTE_S0_Color_16bpp();
  BTE_S0_Memory_Start_Address(sourcePage);
  BTE_S0_Image_Width(1024);
  BTE_S0_Window_Start_XY(x, y);

  BTE_Destination_Color_16bpp();
  BTE_Destination_Memory_Start_Address(destPage);
  BTE_Destination_Image_Width(1024);
  BTE_Destination_Window_Start_XY(x, y);
  BTE_Window_Size(w, h);

  //move with ROP 0 
  BTE_ROP_Code(12);       //memory copy s0(sourcePage) to destPage
  BTE_Operation_Code(2);  //BTE move
  BTE_Enable();
  Check_BTE_Busy();
}


void LCD_drawBitmapPageBuf(unsigned short* pixels,
                           unsigned long bufPage, unsigned long destPage,
                           unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
  Canvas_Image_Start_address(bufPage);
  LCD_drawBitmap(pixels, x, y, w, h);
  LCD_copyArea(bufPage, destPage, x, y, w, h);
}
