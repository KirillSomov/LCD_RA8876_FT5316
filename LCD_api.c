
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


void LCD_setPage(unsigned long layer)
{
  Select_Main_Window_16bpp();
  Main_Image_Start_Address(layer);
  Main_Image_Width(1024);
  Main_Window_Start_XY(0,0);
  Canvas_Image_Start_address(layer);
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


void LCD_printString1(char* string, unsigned short x, unsigned short y, unsigned short backgroundColor, unsigned short textColor)
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


void LCD_drawBitmap(uint16_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  unsigned long im=1;

  //DMA initial setting
  Enable_SFlash_SPI();
    Select_SFI_1();
    Select_SFI_DMA_Mode();
    Select_SFI_24bit_Address();

    //Select_SFI_Waveform_Mode_0();
    Select_SFI_Waveform_Mode_3();

    //Select_SFI_0_DummyRead();	//normal read mode
    Select_SFI_8_DummyRead(); //1byte dummy cycle
    //Select_SFI_16_DummyRead();
    //Select_SFI_24_DummyRead();

    Select_SFI_Single_Mode();
    //Select_SFI_Dual_Mode0();
    //Select_SFI_Dual_Mode1();

    SPI_Clock_Period(0);


  SFI_DMA_Destination_Upper_Left_Corner(x, y); // x, y
    SFI_DMA_Transfer_Width_Height(w, h);
    SFI_DMA_Source_Width(1024);//

  SFI_DMA_Source_Start_Address(im*1024*600*2*1);//
  Start_SFI_DMA();
    Check_Busy_SFI_DMA();


  //write 16x16 pattern to sdram
  //Pattern_Format_16X16();	
  Canvas_Image_Start_address(PAGE5_START_ADDR);//any layer 
    Canvas_image_width(1024);
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    Show_picture(w*h, pixels); 

  Canvas_Image_Start_address(PAGE0_START_ADDR);//
    Canvas_image_width(1024);
    Active_Window_XY(0,0);
    Active_Window_WH(1024,600);

    
  BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(PAGE5_START_ADDR);
    BTE_S0_Image_Width(w);
 
    BTE_S1_Color_16bpp();
    BTE_S1_Memory_Start_Address(0);
    BTE_S1_Image_Width(1024);

    BTE_Destination_Color_16bpp();  
    BTE_Destination_Memory_Start_Address(PAGE0_START_ADDR);
    BTE_Destination_Image_Width(1024);

    BTE_ROP_Code(0x0C);//(0xc);
    BTE_Operation_Code(0x02);//0x06);//pattern fill	

  BTE_S1_Window_Start_XY(x, y);      
    BTE_Destination_Window_Start_XY(x, y); // x, y
    BTE_Window_Size(w, h);

    BTE_Enable();   
    Check_BTE_Busy();  
}


void LCD_drawBitmap2(uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  Canvas_Image_Start_address(PAGE1_START_ADDR);//Layer 2
  Active_Window_XY(x, y);
  Active_Window_WH(w, h);
  Goto_Pixel_XY(x, y);
  Show_picture(w*h, pixels); 
  //Active_Window_XY(0,0);
  //Active_Window_WH(1024,600);
}


void LCD_drawBitmap3(uint16_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  //creat grid 500*400 to layer2 used geometric draw 
  //Canvas_Image_Start_address(PAGE1_START_ADDR);//Layer 2
  // draw picture
  //LCD_drawBitmap2(pixels, x, y, w, h);

  //BTE memory(move) grid to layer1
  BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(PAGE1_START_ADDR);
    BTE_S0_Image_Width(1024);
    BTE_S0_Window_Start_XY(x, y);

    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(PAGE0_START_ADDR);
    BTE_Destination_Image_Width(1024);
    BTE_Destination_Window_Start_XY(x, y);
    BTE_Window_Size(w, h);

  //move with ROP 0 
    BTE_ROP_Code(12); //memory copy s0(layer2)grid to layer1  
    BTE_Operation_Code(2); //BTE move
    BTE_Enable();
    Check_BTE_Busy();

  //Canvas_Image_Start_address(PAGE0_START_ADDR);//Layer 1
}
