
#include "RA8876.h"


void delay_us(unsigned int i)
{
  while(i--) {;}
}

void Delay_ms(uint i)
{
  delay_ms(i);
}


void Delay10ms(uint i)
{
  while(i--)
  {
    Delay_ms(10);
  }
}

void Delay100ms(uint i)
{
  while(i--)
  {
    Delay_ms(100);
  }
}


///********************************
void LCD_CmdWrite(unsigned char cmd)
{
  SPI3_CS_RESET;
  SPIx_rxtx(SPI3, 0x00);
  SPIx_rxtx(SPI3, cmd);
  SPI3_CS_SET;
}

//----------------------//
void LCD_DataWrite(unsigned char Data)
{
  SPI3_CS_RESET;
  SPIx_rxtx(SPI3, 0x80);
  SPIx_rxtx(SPI3, Data);
  SPI3_CS_SET;
}

//----------------------//
unsigned char LCD_DataRead(void)
{  
  SPI3_CS_RESET;
  SPIx_rxtx(SPI3, 0xC0);
  unsigned short data = SPIx_rxtx(SPI3, 0x00);
  SPI3_CS_SET;
  
  return data;
}  
//-----------------------//
unsigned char LCD_StatusRead(void)
{
  SPI3_CS_RESET;
  SPIx_rxtx(SPI3, 0x40);
  uint16_t data = (uint8_t)SPIx_rxtx(SPI3, 0x00);
  SPI3_CS_SET;
  
  return data;
}
//********************************/


//==============================================================================
void LCD_RegisterWrite(unsigned char Cmd,unsigned char Data)
{
  LCD_CmdWrite(Cmd);
  LCD_DataWrite(Data);
}  
//---------------------//
unsigned char LCD_RegisterRead(unsigned char Cmd)
{
  unsigned char temp;
  
  LCD_CmdWrite(Cmd);
  temp=LCD_DataRead();
  return temp;
}

 void LCD_WriteRAM_Prepare(void)
{
  LCD_CmdWrite(0x04); //
}

void RA8876_HW_Reset(void)
{
  GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  Delay_ms(2);
  GPIO_SetBits(GPIOD, GPIO_Pin_3);
  Delay_ms(20);
}


 void LCD_SetPoint(uint x,uint y,uint point)
{
  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();
  LCD_DataWrite(point);
  LCD_DataWrite(point>>8);
}


 void LCD_PutChar(uint x,uint y,uchar c,uint charColor,uint bkColor)
{
  uint i=0;
  uint j=0;
  
  uint tmp_char=0;

  for (i=0;i<24;i++)
  {
    tmp_char=ASCII_Table[((c-0x20)*24)+i];	 //??h32???????A?O?]???r?U??q?U?}?l???A???r?U??????`??
    for (j=0;j<16;j++)
    {
//      if ( (tmp_char >> 15-j) & 0x01 == 0x01)	????W???????A?r?UO??L????A??o????L??N??F
  if ( (tmp_char >> j) & 0x01 == 0x01)
      {
        LCD_SetPoint(x+j,y+i,charColor); //?r???C??
      }
      else
      {
        LCD_SetPoint(x+j,y+i,bkColor); // ?r???C??
      }
    }
  }
}


void LCD_DisplayString(uint X,uint Y, char *ptr, uint charColor, uint bkColor)
{
  ulong i = 0;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 64))
  {
    /* Display one character on LCD */
  LCD_PutChar(X, Y, *ptr, charColor, bkColor);
    /* Decrement the column position by 16 */
    X += 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}


void LCD_SetCursor(uint Xpos, uint Ypos)
{
  LCD_CmdWrite(0x5F);
  LCD_DataWrite(Xpos);  
  LCD_CmdWrite(0x60);	   
  LCD_DataWrite(Xpos>>8);
  LCD_CmdWrite(0x61);
  LCD_DataWrite(Ypos);
  LCD_CmdWrite(0x62);	   
  LCD_DataWrite(Ypos>>8);
}	


void LCD_Clear(uint Color)
{
  ulong index = 0;
  
  LCD_SetCursor(0,0); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 614400; index++)
  {
    LCD_DataWrite(Color);
    LCD_DataWrite(Color>>8);
  }
}


void RA8876_initial(void)
{
 
  RA8876_SW_Reset();
  RA8876_PLL_Initial(); 
  RA8876_SDRAM_initail();


   //**[01h]**//
  TFT_24bit();
  Enable_SFlash_SPI();
  Host_Bus_8bit();
//**[02h]**//
  RGB_8b_16bpp();
    //RGB_16b_24bpp_mode1();
  MemWrite_Left_Right_Top_Down();
//**[03h]**//

  Graphic_Mode();
  Memory_Select_SDRAM();   


  HSCAN_L_to_R();
  VSCAN_T_to_B();
  PDATA_Set_RGB();

  //PCLK_Rising();
  PCLK_Falling();
  DE_High_Active();
    //HSYNC_Low_Active();
  HSYNC_High_Active();
  //VSYNC_Low_Active();
  VSYNC_High_Active(); 
  LCD_HorizontalWidth_VerticalHeight(1024,600);
  
  LCD_Horizontal_Non_Display(160);		   //30
  LCD_HSYNC_Start_Position(160);
  LCD_HSYNC_Pulse_Width(70);
  LCD_Vertical_Non_Display(23);		   //16
  LCD_VSYNC_Start_Position(12);
  LCD_VSYNC_Pulse_Width(10);
  

  Select_Main_Window_16bpp();
  Main_Image_Start_Address(0);				
  Main_Image_Width(1024);							
  Main_Window_Start_XY(0,0);	
  Canvas_Image_Start_address(0);
  Canvas_image_width(1024);//
  Active_Window_XY(0,0);
  Active_Window_WH(1024,600);

  Memory_XY_Mode();
  Memory_16bpp_Mode();
  Select_Main_Window_16bpp();

}

//==============================================================================
 void RA8876_PLL_Initial(void) 
{
  // Set pixel clock
  if(SCAN_FREQ>=63)        //&&(SCAN_FREQ<=100))
  {
   LCD_RegisterWrite(0x05,0x04);    //PLL Divided by 4
   LCD_RegisterWrite(0x06,(SCAN_FREQ*4/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=32)&&(SCAN_FREQ<=62))
  {           
   LCD_RegisterWrite(0x05,0x06);    //PLL Divided by 8
   LCD_RegisterWrite(0x06,(SCAN_FREQ*8/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=16)&&(SCAN_FREQ<=31))
  {           
   LCD_RegisterWrite(0x05,0x16);    //PLL Divided by 16
   LCD_RegisterWrite(0x06,(SCAN_FREQ*16/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=8)&&(SCAN_FREQ<=15))
  {
   LCD_RegisterWrite(0x05,0x26);    //PLL Divided by 32
   LCD_RegisterWrite(0x06,(SCAN_FREQ*32/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>0)&&(SCAN_FREQ<=7))
  {
   LCD_RegisterWrite(0x05,0x36);    //PLL Divided by 64
   LCD_RegisterWrite(0x06,(SCAN_FREQ*64/OSC_FREQ)-1);
  }            
 
  
  // Set SDRAM clock
  if(DRAM_FREQ>=125)        //&&(DRAM_FREQ<=166))
  {
   LCD_RegisterWrite(0x07,0x02);    //PLL Divided by 2
   LCD_RegisterWrite(0x08,(DRAM_FREQ*2/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=63)&&(DRAM_FREQ<=124))   //&&(DRAM_FREQ<=166)
  {
   LCD_RegisterWrite(0x07,0x04);    //PLL Divided by 4
   LCD_RegisterWrite(0x08,(DRAM_FREQ*4/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=31)&&(DRAM_FREQ<=62))
  {           
   LCD_RegisterWrite(0x07,0x06);    //PLL Divided by 8
   LCD_RegisterWrite(0x08,(DRAM_FREQ*8/OSC_FREQ)-1);
  }
  if(DRAM_FREQ<=30)
  {
   LCD_RegisterWrite(0x07,0x06);    //PLL Divided by 8
   LCD_RegisterWrite(0x08,(30*8/OSC_FREQ)-1); //
  }
 

  // Set Core clock
  if(CORE_FREQ>=125)
  {
   LCD_RegisterWrite(0x09,0x02);    //PLL Divided by 2
   LCD_RegisterWrite(0x0A,(CORE_FREQ*2/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=63)&&(CORE_FREQ<=124))     
  {
   LCD_RegisterWrite(0x09,0x04);    //PLL Divided by 4
   LCD_RegisterWrite(0x0A,(CORE_FREQ*4/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=31)&&(CORE_FREQ<=62))
  {           
   LCD_RegisterWrite(0x09,0x06);    //PLL Divided by 8
   LCD_RegisterWrite(0x0A,(CORE_FREQ*8/OSC_FREQ)-1);
  }
  if(CORE_FREQ<=30)
  {
   LCD_RegisterWrite(0x09,0x06);    //PLL Divided by 8
   LCD_RegisterWrite(0x0A,(30*8/OSC_FREQ)-1); // 
  }

  LCD_CmdWrite(0x01);
  LCD_DataWrite(0x00);
  delay_us(10);
  LCD_DataWrite(0x80);
  //Enable_PLL();

  Delay_ms(1);	//??PLLi?w
}


//------------------------------------//----------------------------------*/

void RA8876_SDRAM_initail(void)
{
  unsigned short sdram_itv;

#ifdef IS42SM16160D
//Setting SDRAM (bank : 4?ARow : A0-A12?AColumn : A0-A9?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0xf9);	
  LCD_RegisterWrite(0xe1,0x02);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 8192) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);
  LCD_RegisterWrite(0xe4,0x09);
#endif

#ifdef IS42S16320B
//Setting SDRAM (bank : 4?ARow : A0-A12?AColumn : A0-A9?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0x32);	
  LCD_RegisterWrite(0xe1,0x02);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 8192) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif


#ifdef IS42S16400F
//Setting SDRAM (bank : 4?ARow : A0-A12?AColumn : A0-A9?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0x28);	
  LCD_RegisterWrite(0xe1,0x02);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 4096) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef M12L32162A
//Setting SDRAM (bank : 2?ARow : A0-A11?AColumn : A0-A7?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0x08);	
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 4096) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x09);
#endif

#ifdef M12L2561616A
//Setting SDRAM (bank : 4?ARow : A0-A12?AColumn : A0-A8?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0x31);	
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 8192) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef W9825G6JH
//Setting SDRAM (bank : 4?ARow : A0-A12?AColumn : A0-A8?ACAS : 2 & 3)
  LCD_RegisterWrite(0xe0,0x31);      
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 8192) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef W9812G6JH

  LCD_RegisterWrite(0xe0,0x29);      
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 8192) / (1000/DRAM_FREQ) ;

    sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef MT48LC4M16A
   LCD_RegisterWrite(0xe0,0x28);      
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 4096) / (1000/DRAM_FREQ) ;

  sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef K4S641632N
   LCD_RegisterWrite(0xe0,0x28);      
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 4096) / (1000/DRAM_FREQ) ;

  sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

#ifdef K4S281632K
   LCD_RegisterWrite(0xe0,0x29);      
  LCD_RegisterWrite(0xe1,0x03);	//CAS:2=0x02?ACAS:3=0x03

    sdram_itv = (64000000 / 4096) / (1000/DRAM_FREQ) ;

  sdram_itv-=2;

  LCD_RegisterWrite(0xe2,sdram_itv);
  LCD_RegisterWrite(0xe3,sdram_itv >>8);

  LCD_RegisterWrite(0xe4,0x01);
#endif

  Check_SDRAM_Ready();
 
Delay_ms(1);

}

/******************************************************************************/
/*Sub program area		  													  */
/******************************************************************************/
//==============================================================================
void Check_Mem_WR_FIFO_not_Full(void)
{
/*	0: Memory Write FIFO is not full.
  1: Memory Write FIFO is full.		*/
  do
  {
    
  }while( LCD_StatusRead()&0x80 );
}
void Check_Mem_WR_FIFO_Empty(void)
{
/*	0: Memory Write FIFO is not empty.
  1: Memory Write FIFO is empty.		*/	
  do
  {

  }while( (LCD_StatusRead()&0x40) == 0x00 );
}
void Check_Mem_RD_FIFO_not_Full(void)
{
/*	0: Memory Read FIFO is not full.
  1: Memory Read FIFO is full.		*/
  do
  {

  }while( LCD_StatusRead()&0x20 );
}
void Check_Mem_RD_FIFO_not_Empty(void)
{
/*	0: Memory Read FIFO is not empty.
  1: Memory Read FIFO is empty.
    */
  do
  {

  }while( LCD_StatusRead()&0x10 );
}
void Check_2D_Busy(void)
{
  do
 {
 }while( LCD_StatusRead()&0x08 );
     
}
void Check_SDRAM_Ready(void)
{
/*	0: SDRAM is not ready for access
  1: SDRAM is ready for access		*/	
  unsigned char temp; 	
  do
  {
    temp=LCD_StatusRead();
  }while( (temp&0x04) == 0x00 );
}


unsigned char Power_Saving_Status(void)
{
      unsigned char temp;

    if((LCD_StatusRead()&0x02)==0x02)
      temp = 1;
    else
      temp = 0;

    return temp;
}

 
void Check_Power_is_Normal(void)//?O Normal mode ?N???}
{
/*	0: Normal operation state
  1: Power saving state		*/	
  do
  {

  }while( LCD_StatusRead()&0x02 );
}
void Check_Power_is_Saving(void)//?O Saving mode ?N???}
{
/*	0: Normal operation state
  1: Power saving state		*/	
  do
  {

  }while( (LCD_StatusRead()&0x02) == 0x00 );
}
void Check_NO_Interrupt(void)//?S?? Interrupt ?N???}
{
/*	0: without interrupt event
  1: interrupt event occur
    */	
  do
  {

  }while( LCD_StatusRead()&0x01 );
}
void Check_Interrupt_Occur(void)//?? Interrupt ?N???}
{
/*	0: without interrupt event
  1: interrupt event occur
    */	
  do
  {

  }while( (LCD_StatusRead()&0x01) == 0x00 );
}

void Check_Busy_Draw(void)
{
unsigned char temp;

//(0)
//	Delay_ms(10);
  
  
//(A)
  do
  {
    temp=LCD_StatusRead();
  }	while(temp&0x08);
/*
//(B)
  do
  {
    LCD_CmdWrite(0x67);
    temp=LCD_DataRead();
  }	while(temp&0x80);

//(C)
  do
  {
    //(1)
    temp=LCD_RegisterRead(0x76);
    //(2)
    //LCD_CmdWrite(0x76);
    //temp=LCD_DataRead();
  }	while(temp&0x80);
*/

//Delay_ms(10);
}

//[00h]=========================================================================

void RA8876_SW_Reset(void)//IC Reset?A?????|?MRegister/Memory?C
{
  unsigned char temp;

  LCD_CmdWrite(0x00);
  temp = LCD_DataRead();
  temp |= 0x01;
  LCD_DataWrite(temp);

    do
    {
      temp = LCD_DataRead();
    }
    while( temp&0x01 );
}

//[01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h]
void Enable_PLL(void)
{
/*  0: PLL disable; allow change PLL parameter.
    1: PLL enable; cannot change PLL parameter.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb7;
  LCD_DataWrite(temp);

    delay_us(1);//PLL ??????A??? 1 us?C
}
void RA8876_Sleep(void)
{
/*  0: Normal mode.
    1: Sleep mode.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void RA8876_WakeUp(void)
{
/*  0: Normal mode.
    1: Sleep mode.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Key_Scan_Enable(void)
{
/*  0: Disable.
    1: Enable.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb5;
  LCD_DataWrite(temp);    
}
void Key_Scan_Disable(void)
{
/*  0: Disable.
    1: Enable.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb5;
  LCD_DataWrite(temp);    
}
void TFT_24bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb4;
    temp &= cClrb3;
  LCD_DataWrite(temp);    
}
void TFT_18bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb4;
    temp |= cSetb3;
  LCD_DataWrite(temp);  
}
void TFT_16bit(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb4;
    temp &= cClrb3;
  LCD_DataWrite(temp);  
}
void TFT_LVDS(void)
{
/*  00b: 24-bits output.
    01b: 18-bits output, unused pins are set as GPIO.
    10b: 16-bits output, unused pins are set as GPIO.
    11b: LVDS, all 24-bits unused output pins are set as GPIO.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb4;
    temp |= cSetb3;
  LCD_DataWrite(temp);  
}

void RA8876_I2CM_Enable(void)
{
/*  I2C master Interface Enable/Disable
    0: Disable
    1: Enable*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb2;
  LCD_DataWrite(temp);    
}

void RA8876_I2CM_Disable(void)
{
/*  I2C master Interface Enable/Disable
    0: Disable
    1: Enable*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb2;
  LCD_DataWrite(temp);     
}

void Enable_SFlash_SPI(void)
{
/*  Serial Flash SPI Interface Enable/Disable
    0: Disable
    1: Enable*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb1;
  LCD_DataWrite(temp);     
}

void Disable_SFlash_SPI(void)
{
/*  Serial Flash SPI Interface Enable/Disable
    0: Disable
    1: Enable*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb1;
  LCD_DataWrite(temp);     
}
void Host_Bus_8bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Host_Bus_16bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
  unsigned char temp;
  LCD_CmdWrite(0x01);
  temp = LCD_DataRead();
  temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h]

void RGB_8b_8bpp(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void RGB_8b_16bpp(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void RGB_8b_24bpp(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}

void RGB_16b_8bpp(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb7;
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void RGB_16b_16bpp(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void RGB_16b_24bpp_mode1(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void RGB_16b_24bpp_mode2(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb7;
  temp |= cSetb6;
  LCD_DataWrite(temp);
}

void MemRead_Left_Right_Top_Down(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb5;
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
void MemRead_Right_Left_Top_Down(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb5;
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void MemRead_Top_Down_Left_Right(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb5;
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void MemRead_Down_Top_Left_Right(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb5;
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void MemWrite_Left_Right_Top_Down(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb2;
  temp &= cClrb1;
  LCD_DataWrite(temp);
}
void MemWrite_Right_Left_Top_Down(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp &= cClrb2;
  temp |= cSetb1;
  LCD_DataWrite(temp);
}
void MemWrite_Top_Down_Left_Right(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb2;
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void MemWrite_Down_Top_Left_Right(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x02);
  temp = LCD_DataRead();
  temp |= cSetb2;
  temp |= cSetb1;
  LCD_DataWrite(temp);
}
//[03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h]
void Interrupt_Active_Low(void)
{
/*  MPU Interrupt active level
    0 : active low.
    1 : active high.*/
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Interrupt_Active_High(void)
{
/*  MPU Interrupt active level
    0 : active low.
    1 : active high.*/
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp |= cSetb7;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Debounce(void)
{
/*  External interrupt de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)*/
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Nodebounce(void)
{
/*  External interrupt de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)*/
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Low_Level_Trigger(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp &= cClrb5;
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Input_High_Level_Trigger(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp |= cSetb5;
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Falling_Edge_Trigger(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Rising_Edge_Trigger(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
  temp |= cSetb5;
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void LVDS_Format1(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void LVDS_Format2(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Graphic_Mode(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Text_Mode(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Memory_Select_SDRAM(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;	// B
  LCD_DataWrite(temp);
}
void Memory_Select_Graphic_Cursor_RAM(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Memory_Select_Color_Palette_RAM(void)
{
  unsigned char temp;
  LCD_CmdWrite(0x03);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[05h]=========================================================================
//[06h]=========================================================================
//[07h]=========================================================================
//[08h]=========================================================================
//[09h]=========================================================================
//[0Ah]=========================================================================
//[0Bh]=========================================================================

void Enable_Resume_Interrupt(void)
{
/*
Resume Interrupt Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Disable_Resume_Interrupt(void)
{
/*
Resume Interrupt Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Enable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Disable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Enable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Disable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Enable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Disable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Enable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Disable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Enable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Disable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Enable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Disable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Enable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Disable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0B);
  temp = LCD_DataRead();
    temp &= cClrb0;
  LCD_DataWrite(temp);
}

//[0Ch]=========================================================================
unsigned char Read_Interrupt_status(void)
{
/*
[Bit7]Read Function ..Resume Interrupt Status
0: No Resume interrupt happens.
1: Resume interrupt happens.
[Bit6]Read Function .. PS[0] pin Interrupt Status
0: No PS[0] pin interrupt happens.
1: PS[0] pin interrupt happens.
[Bit5]Read Function .. I2C master Interrupt Status
0: No I2C master interrupt happens.
1: I2C master interrupt happens.
[Bit4]Read Function .. Vsync Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit3]Read Function ..Key Scan Interrupt Status
0: No Key Scan interrupt happens.
1: Key Scan interrupt happens.
[Bit2]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit1]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit0]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    return temp;
}
void Clear_Resume_Interrupt_Flag(void)
{
/*
Resume Interrupt flag
Write Function .. Resume Interrupt Clear Bit
0: No operation.
1: Clear Resume interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Clear_ExtInterrupt_Input_Flag(void)
{
/*
External Interrupt (PS[0] pin) flag
Write Function .. PS[0] pin edge Interrupt Clear Bit
0: No operation.
1: Clear the PS[0] pin edge interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Clear_I2CM_Interrupt_Flag(void)
{
/*
I2C master Interrupt flag
Write Function.. I2C master Interrupt Clear Bit
0: No operation.
1: Clear the I2C master interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Clear_Vsync_Interrupt_Flag(void)
{
/*
Vsync Time base interrupt flag
Write Function ..Vsync Interrupt Clear Bit
0: No operation.
1: Clear the interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Clear_KeyScan_Interrupt_Flag(void)
{
/*
Key Scan Interrupt flag
Write Function..Key Scan Interrupt Clear Bit
0: No operation.
1: Clear the Key Scan interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Clear_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt flag
Write Function.. Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Clear_PWM1_Interrupt_Flag(void)
{
/*
PWM 1 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Clear_PWM0_Interrupt_Flag(void)
{
/*
PWM 0 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0C);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
//[0Dh]=========================================================================
void Mask_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Mask_ExtInterrupt_Input_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Mask_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Mask_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Mask_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Mask_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Mask_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Mask_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

void Enable_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Enable_ExtInterrupt_Inpur_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Enable_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Enable_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Enable_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Enable_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Enable_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Enable_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
  unsigned char temp;
  LCD_CmdWrite(0x0D);
  temp = LCD_DataRead();
    temp &= cClrb0;
  LCD_DataWrite(temp);
}

//[0Eh]=========================================================================
void Enable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Enable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Enable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Enable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Enable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Enable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Disable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Disable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Disable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Disable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Disable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Disable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
  unsigned char temp;
  LCD_CmdWrite(0x0E);
  temp = LCD_DataRead();
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
//[0Fh]=========================================================================
void XPDAT18_Set_GPIO_D7(void)
{
/*
XPDAT[18] ?V not scan function select
0: GPIO-D7
1: KOUT[4]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void XPDAT18_Set_KOUT4(void)
{
/*
XPDAT[18] ?V not scan function select
0: GPIO-D7
1: KOUT[4]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void XPDAT17_Set_GPIO_D5(void)
{
/*
XPDAT[17] ?V not scan function select
0: GPIO-D5
1: KOUT[2]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void XPDAT17_Set_KOUT2(void)
{
/*
XPDAT[17] ?V not scan function select
0: GPIO-D5
1: KOUT[2]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void XPDAT16_Set_GPIO_D4(void)
{
/*
XPDAT[16] ?V not scan function select
0: GPIO-D4
1: KOUT[1]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void XPDAT16_Set_KOUT1(void)
{
/*
XPDAT[16] ?V not scan function select
0: GPIO-D4
1: KOUT[1]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void XPDAT9_Set_GPIO_D3(void)
{
/*
XPDAT[9] ?V not scan function select
0: GPIO-D3
1: KOUT[3]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void XPDAT9_Set_KOUT3(void)
{
/*
XPDAT[9] ?V not scan function select
0: GPIO-D3
1: KOUT[3]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void XPDAT8_Set_GPIO_D2(void)
{
/*
XPDAT[8] ?V not scan function select
0: GPIO-D2
1: KIN[3]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void XPDAT8_Set_KIN3(void)
{
/*
XPDAT[8] ?V not scan function select
0: GPIO-D2
1: KIN[3]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void XPDAT2_Set_GPIO_D6(void)
{
/*
XPDAT[2] ?V not scan function select
0: GPIO-D6
1: KIN[4]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void XPDAT2_Set_KIN4(void)
{
/*
XPDAT[2] ?V not scan function select
0: GPIO-D6
1: KIN[4]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void XPDAT1_Set_GPIO_D1(void)
{
/*
XPDAT[1] ?V not scan function select
0: GPIO-D1
1: KIN[2]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void XPDAT1_Set_KIN2(void)
{
/*
XPDAT[1] ?V not scan function select
0: GPIO-D1
1: KIN[2]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void XPDAT0_Set_GPIO_D0(void)
{
/*
XPDAT[0] ?V not scan function select
0: GPIO-D0
1: KIN[1]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void XPDAT0_Set_KIN1(void)
{
/*
XPDAT[0] ?V not scan function select
0: GPIO-D0
1: KIN[1]
*/
  unsigned char temp;
  LCD_CmdWrite(0x0F);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[10h]=========================================================================
void Enable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Disable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Enable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Disable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Select_PIP1_Parameter(void)
{
/*
0: To configure PIP 1??s parameters.
1: To configure PIP 2??s parameters..
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Select_PIP2_Parameter(void)
{
/*
0: To configure PIP 1??s parameters.
1: To configure PIP 2??s parameters..
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Select_Main_Window_8bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Select_Main_Window_16bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Select_Main_Window_24bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x10);
  temp = LCD_DataRead();
    temp |= cSetb3;
    //temp |= cSetb2;
  LCD_DataWrite(temp);
}
//[11h]=========================================================================
void Select_PIP2_Window_8bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Select_PIP2_Window_16bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Select_PIP2_Window_24bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Select_PIP1_Window_8bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Select_PIP1_Window_16bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Select_PIP1_Window_24bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
  unsigned char temp;
  LCD_CmdWrite(0x11);
  temp = LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
  LCD_DataWrite(temp);
}

//[12h]=========================================================================
void PCLK_Rising(void)	 
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
  unsigned char temp;
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void PCLK_Falling(void)
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
  unsigned char temp;
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Display_ON(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}

void Display_OFF(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Color_Bar_ON(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Color_Bar_OFF(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &= cClrb5;
  LCD_DataWrite(temp);
}
void HSCAN_L_to_R(void)
{
/*	
Horizontal Scan Direction
0 : From Left to Right
1 : From Right to Left
PIP window will be disabled when HDIR set as 1.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
void HSCAN_R_to_L(void)
{
/*	
Horizontal Scan Direction
0 : From Left to Right
1 : From Right to Left
PIP window will be disabled when HDIR set as 1.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void VSCAN_T_to_B(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &= cClrb3;
  LCD_DataWrite(temp);
}
void VSCAN_B_to_T(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp |= cSetb3;
  LCD_DataWrite(temp);
}
void PDATA_Set_RGB(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
    temp &=0xf8;
  LCD_DataWrite(temp);
}
void PDATA_Set_RBG(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &=0xf8;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void PDATA_Set_GRB(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &=0xf8;
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void PDATA_Set_GBR(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &=0xf8;
    temp |= cSetb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void PDATA_Set_BRG(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &=0xf8;
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
void PDATA_Set_BGR(void)
{
/*	
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp &=0xf8;
  temp |= cSetb2;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

void PDATA_IDLE_STATE(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x12);
  temp = LCD_DataRead();
  temp |=0x07;
  LCD_DataWrite(temp);

}


//[13h]=========================================================================
void HSYNC_Low_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp &= cClrb7;
  LCD_DataWrite(temp);
}
void HSYNC_High_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();	 
  temp |= cSetb7;
  LCD_DataWrite(temp);
}
void VSYNC_Low_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp &= cClrb6;	
  LCD_DataWrite(temp);
}
void VSYNC_High_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void DE_Low_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void DE_High_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Idle_DE_Low(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??DE?? output is low.
1 : Pin ??DE?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Idle_DE_High(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??DE?? output is low.
1 : Pin ??DE?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Idle_PCLK_Low(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??PCLK?? output is low.
1 : Pin ??PCLK?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Idle_PCLK_High(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??PCLK?? output is low.
1 : Pin ??PCLK?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Idle_PDAT_Low(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ??PDAT[23:0]?? output is low.
1 : Pins ??PCLK[23:0]?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Idle_PDAT_High(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ??PDAT[23:0]?? output is low.
1 : Pins ??PCLK[23:0]?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Idle_HSYNC_Low(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??HSYNC?? output is low.
1 : Pin ??HSYNC?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Idle_HSYNC_High(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??HSYNC?? output is low.
1 : Pin ??HSYNC?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Idle_VSYNC_Low(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??VSYNC?? output is low.
1 : Pin ??VSYNC?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Idle_VSYNC_High(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ??VSYNC?? output is low.
1 : Pin ??VSYNC?? output is high.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x13);
  temp = LCD_DataRead();
  temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[14h][15h][1Ah][1Bh]=========================================================================
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY)
{
/*
[14h] Horizontal Display Width Setting Bit[7:0]
[15h] Horizontal Display Width Fine Tuning (HDWFT) [3:0]
The register specifies the LCD panel horizontal display width in
the unit of 8 pixels resolution.
Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR

[1Ah] Vertical Display Height Bit[7:0]
Vertical Display Height(Line) = VDHR + 1
[1Bh] Vertical Display Height Bit[10:8]
Vertical Display Height(Line) = VDHR + 1
*/
  unsigned char temp;
  if(WX<8)
    {
  LCD_CmdWrite(0x14);
  LCD_DataWrite(0x00);
    
  LCD_CmdWrite(0x15);
  LCD_DataWrite(WX);
    
    temp=HY-1;
  LCD_CmdWrite(0x1A);
  LCD_DataWrite(temp);
      
  temp=(HY-1)>>8;
  LCD_CmdWrite(0x1B);
  LCD_DataWrite(temp);
  }
  else
  {
    temp=(WX/8)-1;
  LCD_CmdWrite(0x14);
  LCD_DataWrite(temp);
    
    temp=WX%8;
  LCD_CmdWrite(0x15);
  LCD_DataWrite(temp);
    
    temp=HY-1;
  LCD_CmdWrite(0x1A);
  LCD_DataWrite(temp);
      
  temp=(HY-1)>>8;
  LCD_CmdWrite(0x1B);
  LCD_DataWrite(temp);
  }
}
//[16h][17h]=========================================================================
void LCD_Horizontal_Non_Display(unsigned short WX)
{
/*
[16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
This register specifies the horizontal non-display period. Also
called back porch.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR

[17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
This register specifies the fine tuning for horizontal non-display
period; it is used to support the SYNC mode panel. Each level of
this modulation is 1-pixel.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
*/
  unsigned char temp;
  if(WX<8)
  {
  LCD_CmdWrite(0x16);
  LCD_DataWrite(0x00);
    
  LCD_CmdWrite(0x17);
  LCD_DataWrite(WX);
  }
  else
  {
    temp=(WX/8)-1;
  LCD_CmdWrite(0x16);
  LCD_DataWrite(temp);
    
    temp=WX%8;
  LCD_CmdWrite(0x17);
  LCD_DataWrite(temp);
  }	
}
//[18h]=========================================================================
void LCD_HSYNC_Start_Position(unsigned short WX)
{
/*
[18h] HSYNC Start Position[4:0]
The starting position from the end of display area to the
beginning of HSYNC. Each level of this modulation is 8-pixel.
Also called front porch.
HSYNC Start Position(pixels) = (HSTR + 1)x8
*/
  unsigned char temp;
  if(WX<8)
  {
  LCD_CmdWrite(0x18);
  LCD_DataWrite(0x00);
  }
  else
  {
    temp=(WX/8)-1;
  LCD_CmdWrite(0x18);
  LCD_DataWrite(temp);	
  }
}
//[19h]=========================================================================
void LCD_HSYNC_Pulse_Width(unsigned short WX)
{
/*
[19h] HSYNC Pulse Width(HPW) [4:0]
The period width of HSYNC.
HSYNC Pulse Width(pixels) = (HPW + 1)x8
*/
  unsigned char temp;
  if(WX<8)
  {
  LCD_CmdWrite(0x19);
  LCD_DataWrite(0x00);
  }
  else
  {
    temp=(WX/8)-1;
  LCD_CmdWrite(0x19);
  LCD_DataWrite(temp);	
  }
}
//[1Ch][1Dh]=========================================================================
void LCD_Vertical_Non_Display(unsigned short HY)
{
/*
[1Ch] Vertical Non-Display Period Bit[7:0]
Vertical Non-Display Period(Line) = (VNDR + 1)

[1Dh] Vertical Non-Display Period Bit[9:8]
Vertical Non-Display Period(Line) = (VNDR + 1)
*/
  unsigned char temp;
    temp=HY-1;
  LCD_CmdWrite(0x1C);
  LCD_DataWrite(temp);

  LCD_CmdWrite(0x1D);
  LCD_DataWrite(temp>>8);
}
//[1Eh]=========================================================================
void LCD_VSYNC_Start_Position(unsigned short HY)
{
/*
[1Eh] VSYNC Start Position[7:0]
The starting position from the end of display area to the beginning of VSYNC.
VSYNC Start Position(Line) = (VSTR + 1)
*/
  unsigned char temp;
    temp=HY-1;
  LCD_CmdWrite(0x1E);
  LCD_DataWrite(temp);
}
//[1Fh]=========================================================================
void LCD_VSYNC_Pulse_Width(unsigned short HY)
{
/*
[1Fh] VSYNC Pulse Width[5:0]
The pulse width of VSYNC in lines.
VSYNC Pulse Width(Line) = (VPWR + 1)
*/
  unsigned char temp;
    temp=HY-1;
  LCD_CmdWrite(0x1F);
  LCD_DataWrite(temp);
}
//[20h][21h][22h][23h]=========================================================================
void Main_Image_Start_Address(unsigned long Addr)	
{
/*
[20h] Main Image Start Address[7:2]
[21h] Main Image Start Address[15:8]
[22h] Main Image Start Address [23:16]
[23h] Main Image Start Address [31:24]
*/
  LCD_RegisterWrite(0x20,Addr);
  LCD_RegisterWrite(0x21,Addr>>8);
  LCD_RegisterWrite(0x22,Addr>>16);
  LCD_RegisterWrite(0x23,Addr>>24);
}
//[24h][25h]=========================================================================
void Main_Image_Width(unsigned short WX)	
{
/*
[24h] Main Image Width [7:0]
[25h] Main Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. MIW Bit [1:0] tie to ??0?? internally.
The value is physical pixel number. Maximum value is 8188 pixels
*/
  LCD_RegisterWrite(0x24,WX);
  LCD_RegisterWrite(0x25,WX>>8);
}
//[26h][27h][28h][29h]=========================================================================
void Main_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[26h] Main Window Upper-Left corner X-coordination [7:0]
[27h] Main Window Upper-Left corner X-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
It must be divisible by 4. MWULX Bit [1:0] tie to ??0?? internally.
X-axis coordination plus Horizontal display width cannot large than 8188.

[28h] Main Window Upper-Left corner Y-coordination [7:0]
[29h] Main Window Upper-Left corner Y-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
Range is between 0 and 8191.
*/
  LCD_RegisterWrite(0x26,WX);
  LCD_RegisterWrite(0x27,WX>>8);

  LCD_RegisterWrite(0x28,HY);
  LCD_RegisterWrite(0x29,HY>>8);
}
//[2Ah][2Bh][2Ch][2Dh]=========================================================================
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[2Ah] PIP Window Display Upper-Left corner X-coordination [7:0]
[2Bh] PIP Window Display Upper-Left corner X-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
It must be divisible by 4. PWDULX Bit [1:0] tie to ??0?? internally.
X-axis coordination should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Window??s parameters. 
Function bit will be configured for relative PIP window.

[2Ch] PIP Window Display Upper-Left corner Y-coordination [7:0]
[2Dh] PIP Window Display Upper-Left corner Y-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
Y-axis coordination should less than vertical display height.
According to bit of Select Configure PIP 1 or 2 Window??s parameters.
Function bit will be configured for relative PIP window.
*/
  LCD_RegisterWrite(0x2A,WX);
  LCD_RegisterWrite(0x2B,WX>>8);
  
  LCD_RegisterWrite(0x2C,HY);
  LCD_RegisterWrite(0x2D,HY>>8);
}
//[2Eh][2Fh][30h][31h]=========================================================================
void PIP_Image_Start_Address(unsigned long Addr)	
{
/*
[2Eh] PIP Image Start Address[7:2]
[2Fh] PIP Image Start Address[15:8]
[30h] PIP Image Start Address [23:16]
[31h] PIP Image Start Address [31:24]
*/
  LCD_RegisterWrite(0x2E,Addr);
  LCD_RegisterWrite(0x2F,Addr>>8);
  LCD_RegisterWrite(0x30,Addr>>16);
  LCD_RegisterWrite(0x31,Addr>>24);
}
//[32h][33h]=========================================================================
void PIP_Image_Width(unsigned short WX)	
{
/*
[32h] PIP Image Width [7:0]
[33h] PIP Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. PIW Bit [1:0] tie to ??0?? internally.
The value is physical pixel number.
This width should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Window??s parameters.
Function bit will be configured for relative PIP window.
*/
  LCD_RegisterWrite(0x32,WX);
  LCD_RegisterWrite(0x33,WX>>8);
}
//[34h][35h][36h][37h]=========================================================================
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[34h] PIP 1 or 2 Window Image Upper-Left corner X-coordination [7:0]
[35h] PIP Window Image Upper-Left corner X-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
It must be divisible by 4. PWIULX Bit [1:0] tie to ??0?? internally.
X-axis coordination plus PIP image width cannot large than 8188.
According to bit of Select Configure PIP 1 or 2 Window??s parameters. 
Function bit will be configured for relative PIP window.

[36h] PIP Windows Display Upper-Left corner Y-coordination [7:0]
[37h] PIP Windows Image Upper-Left corner Y-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
Y-axis coordination plus PIP window height should less than 8191.
According to bit of Select Configure PIP 1 or 2 Window??s parameters. 
Function bit will be configured for relative PIP window.
*/
  LCD_RegisterWrite(0x34,WX);
  LCD_RegisterWrite(0x35,WX>>8);
  
  LCD_RegisterWrite(0x36,HY);
  LCD_RegisterWrite(0x37,HY>>8);
}
//[38h][39h][3Ah][3Bh]=========================================================================
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY)	
{
/*
[38h] PIP Window Width [7:0]
[39h] PIP Window Width [10:8]
Unit: Pixel.
It must be divisible by 4. The value is physical pixel number.
Maximum value is 8188 pixels.
According to bit of Select Configure PIP 1 or 2 Window??s parameters. 
Function bit will be configured for relative PIP window.

[3Ah] PIP Window Height [7:0]
[3Bh] PIP Window Height [10:8]
Unit: Pixel
The value is physical pixel number. Maximum value is 8191 pixels.
According to bit of Select Configure PIP 1 or 2 Window??s parameters. 
Function bit will be configured for relative PIP window.
*/
  LCD_RegisterWrite(0x38,WX);
  LCD_RegisterWrite(0x39,WX>>8);
  
  LCD_RegisterWrite(0x3A,HY);
  LCD_RegisterWrite(0x3B,HY>>8);
}

//[3Ch]=========================================================================
void Enable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Disable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
//
void Select_Graphic_Cursor_1(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp &= cClrb3;
  temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_2(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp &= cClrb3;
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_3(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp |= cSetb3;
  temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_4(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp |= cSetb3;
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Disable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp &= cClrb1;
  LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Disable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x3C);
  temp = LCD_DataRead();
  temp &= cClrb0;
  LCD_DataWrite(temp);
}
//[3Dh]=========================================================================
void Blinking_Time_Frames(unsigned char temp)	
{
/*
Text Cursor Blink Time Setting (Unit: Frame)
00h : 1 frame time.
01h : 2 frames time.
02h : 3 frames time.
:
FFh : 256 frames time.
*/
  LCD_CmdWrite(0x3D);
  LCD_DataWrite(temp);
}
//[3Eh]=========================================================================
void Text_Cursor_H_V(unsigned short WX,unsigned short HY)
{
/*
[3Eh]
Text Cursor Horizontal Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ??0?? means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
[3Fh]
Text Cursor Vertical Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ??0?? means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
*/
  LCD_CmdWrite(0x3E);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0x3F);
  LCD_DataWrite(HY);
}
//[40h][41h][42h][43h]=========================================================================
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY)
{
/*
[40h] Graphic Cursor Horizontal Location[7:0]
[41h] Graphic Cursor Horizontal Location[12:8]
[42h] Graphic Cursor Vertical Location[7:0]
[43h] Graphic Cursor Vertical Location[12:8]
Reference main Window coordination.
*/	
  LCD_RegisterWrite(0x40,WX);
  LCD_RegisterWrite(0x41,WX>>8);

  LCD_RegisterWrite(0x42,HY);
  LCD_RegisterWrite(0x43,HY>>8);
}
//[44h]=========================================================================
void Set_Graphic_Cursor_Color_1(unsigned char temp)
{
/*
[44h] Graphic Cursor Color 0 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
  LCD_RegisterWrite(0x44,temp);
}
//[45h]=========================================================================
void Set_Graphic_Cursor_Color_2(unsigned char temp)
{
/*
[45h] Graphic Cursor Color 1 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
  LCD_RegisterWrite(0x45,temp);
}
//[50h][51h][52h][53h]=========================================================================
void Canvas_Image_Start_address(unsigned long Addr)	
{
/*
[50h] Start address of Canvas [7:0]
[51h] Start address of Canvas [15:8]
[52h] Start address of Canvas [23:16]
[53h] Start address of Canvas [31:24]
*/
  LCD_RegisterWrite(0x50,Addr);
  LCD_RegisterWrite(0x51,Addr>>8);
  LCD_RegisterWrite(0x52,Addr>>16);
  LCD_RegisterWrite(0x53,Addr>>24);
}
//[54h][55h]=========================================================================
void Canvas_image_width(unsigned short WX)	
{
/*
[54h] Canvas image width [7:2]
[55h] Canvas image width [12:8]
*/
  LCD_RegisterWrite(0x54,WX);
  LCD_RegisterWrite(0x55,WX>>8);
}
//[56h][57h][58h][59h]=========================================================================
void Active_Window_XY(unsigned short WX,unsigned short HY)	
{
/*
[56h] Active Window Upper-Left corner X-coordination [7:0]
[57h] Active Window Upper-Left corner X-coordination [12:8]
[58h] Active Window Upper-Left corner Y-coordination [7:0]
[59h] Active Window Upper-Left corner Y-coordination [12:8]
*/
  LCD_RegisterWrite(0x56,WX);
  LCD_RegisterWrite(0x57,WX>>8);
  
  LCD_RegisterWrite(0x58,HY);
  LCD_RegisterWrite(0x59,HY>>8);
}
//[5Ah][5Bh][5Ch][5Dh]=========================================================================
void Active_Window_WH(unsigned short WX,unsigned short HY)	
{
/*
[5Ah] Width of Active Window [7:0]
[5Bh] Width of Active Window [12:8]
[5Ch] Height of Active Window [7:0]
[5Dh] Height of Active Window [12:8]
*/
  LCD_RegisterWrite(0x5A,WX);
  LCD_RegisterWrite(0x5B,WX>>8);
 
   LCD_RegisterWrite(0x5C,HY);
  LCD_RegisterWrite(0x5D,HY>>8);
}
//[5Eh]=========================================================================
void Select_Write_Data_Position(void)	
{
/*
Select to read back Graphic Read/Write position
0: read back Graphic Write position
1: read back Graphic Read position
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Select_Read_Data_Position(void)	
{
/*
Select to read back Graphic Read/Write position
0: read back Graphic Write position
1: read back Graphic Read position
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Memory_XY_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Memory_Linear_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Memory_8bpp_Mode(void)	
{
/*
Canvas image??s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp &= cClrb1;
  temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Memory_16bpp_Mode(void)	
{
/*
Canvas image??s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp &= cClrb1;
  temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Memory_24bpp_Mode(void)	
{
/*
Canvas image??s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
X0: 8-bits memory data read/write.
X1: 16-bits memory data read/write
*/
  unsigned char temp;

  LCD_CmdWrite(0x5E);
  temp = LCD_DataRead();
  temp |= cSetb1;
  temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[5Fh][60h][61h][62h]=========================================================================
void Goto_Pixel_XY(unsigned short WX,unsigned short HY)	
{
/*
[Write]: Set Graphic Read/Write position
[Read]: Current Graphic Read/Write position
Read back is Read position or Write position depends on
REG[5Eh] bit3, Select to read back Graphic Read/Write position.
When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
When DPRAM Active window mode:Graphic Read/Write 
Horizontal Position [12:8][7:0], 
Vertical Position [12:8][7:0].
Reference Canvas image coordination. Unit: Pixel
*/
  LCD_RegisterWrite(0x5F,WX);
  LCD_RegisterWrite(0x60,WX>>8);
  
  LCD_RegisterWrite(0x61,HY);
  LCD_RegisterWrite(0x62,HY>>8);
}
void Goto_Linear_Addr(unsigned long Addr)
{
 LCD_RegisterWrite(0x5F,Addr);
 LCD_RegisterWrite(0x60,Addr>>8); 
 LCD_RegisterWrite(0x61,Addr>>16);
 LCD_RegisterWrite(0x62,Addr>>24);
}


//[63h][64h][65h][66h]=========================================================================
void Goto_Text_XY(unsigned short WX,unsigned short HY)	
{
/*
Write: Set Text Write position
Read: Current Text Write position
Text Write X-coordination [12:8][7:0]
Text Write Y-coordination [12:8][7:0]
Reference Canvas image coordination.
Unit: Pixel
*/
  LCD_RegisterWrite(0x63,WX);
  LCD_RegisterWrite(0x64,WX>>8);
  
  LCD_RegisterWrite(0x65,HY);
  LCD_RegisterWrite(0x66,HY>>8);
}
//[67h]=========================================================================
/*
[bit7]Draw Line / Triangle Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit5]Fill function for Triangle Signal
0 : Non fill.
1 : Fill.
[bit1]Draw Triangle or Line Select Signal
0 : Draw Line
1 : Draw Triangle
*/
void Start_Line(void)
{
  LCD_CmdWrite(0x67);
  LCD_DataWrite(0x80);
  Check_Busy_Draw();
}
void Start_Triangle(void)
{
  LCD_CmdWrite(0x67);
  LCD_DataWrite(0x82);//B1000_0010
  Check_Busy_Draw();
}
void Start_Triangle_Fill(void)
{

  LCD_CmdWrite(0x67);
  LCD_DataWrite(0xA2);//B1010_0010
  Check_Busy_Draw();
}
//[68h][69h][6Ah][6Bh]=========================================================================
//?u?_?I
void Line_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  LCD_CmdWrite(0x68);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x69);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6A);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6B);
  LCD_DataWrite(HY>>8);
}
//[6Ch][6Dh][6Eh][6Fh]=========================================================================
//?u???I
void Line_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  LCD_CmdWrite(0x6C);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x6D);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6E);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6F);
  LCD_DataWrite(HY>>8);
}
//[68h]~[73h]=========================================================================
//?T??-?I1
void Triangle_Point1_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  LCD_CmdWrite(0x68);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x69);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6A);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6B);
  LCD_DataWrite(HY>>8);
}
//?T??-?I2
void Triangle_Point2_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  LCD_CmdWrite(0x6C);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x6D);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6E);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6F);
  LCD_DataWrite(HY>>8);
}
//?T??-?I3
void Triangle_Point3_XY (unsigned short WX,unsigned short HY)
{
/*
[70h] Draw Triangle Point 3 X-coordination [7:0]
[71h] Draw Triangle Point 3 X-coordination [12:8]
[72h] Draw Triangle Point 3 Y-coordination [7:0]
[73h] Draw Triangle Point 3 Y-coordination [12:8]
*/
  LCD_CmdWrite(0x70);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x71);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x72);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x73);
  LCD_DataWrite(HY>>8);
}
//??_?I
void Square_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  LCD_CmdWrite(0x68);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x69);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6A);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6B);
  LCD_DataWrite(HY>>8);
}
//????I
void Square_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  LCD_CmdWrite(0x6C);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x6D);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x6E);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x6F);
  LCD_DataWrite(HY>>8);
}
//[76h]=========================================================================
/*
[bit7]
Draw Circle / Ellipse / Square /Circle Square Start Signal 
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit6]
Fill the Circle / Ellipse / Square / Circle Square Signal
0 : Non fill.
1 : fill.
[bit5 bit4]
Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
00 : Draw Circle / Ellipse
01 : Draw Circle / Ellipse Curve
10 : Draw Square.
11 : Draw Circle Square.
[bit1 bit0]
Draw Circle / Ellipse Curve Part Select
00 : 
01 : 
10 : 
11 : 
*/
void Start_Circle_or_Ellipse(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0x80);//B1000_XXXX
  Check_Busy_Draw();	
}
void Start_Circle_or_Ellipse_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xC0);//B1100_XXXX
  Check_Busy_Draw();	
}
//
void Start_Left_Down_Curve(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0x90);//B1001_XX00
  Check_Busy_Draw();	
}
void Start_Left_Up_Curve(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0x91);//B1001_XX01
  Check_Busy_Draw();	
}
void Start_Right_Up_Curve(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0x92);//B1001_XX10
  Check_Busy_Draw();	
}
void Start_Right_Down_Curve(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0x93);//B1001_XX11
  Check_Busy_Draw();	
}
//
void Start_Left_Down_Curve_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xD0);//B1101_XX00
  Check_Busy_Draw();
}
void Start_Left_Up_Curve_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xD1);//B1101_XX01
  Check_Busy_Draw();
}
void Start_Right_Up_Curve_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xD2);//B1101_XX10
  Check_Busy_Draw();
}
void Start_Right_Down_Curve_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xD3);//B1101_XX11
  Check_Busy_Draw();
}
//
void Start_Square(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xA0);//B1010_XXXX
  Check_Busy_Draw();
}
void Start_Square_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xE0);//B1110_XXXX
  Check_Busy_Draw();
}
void Start_Circle_Square(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xB0);//B1011_XXXX
  Check_Busy_Draw();	
}
void Start_Circle_Square_Fill(void)
{
  LCD_CmdWrite(0x76);
  LCD_DataWrite(0xF0);//B1111_XXXX
  Check_Busy_Draw();	
}
//[77h]~[7Eh]=========================================================================
//????
void Circle_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
  LCD_CmdWrite(0x7B);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x7C);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x7D);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x7E);
  LCD_DataWrite(HY>>8);
}
//?????
void Ellipse_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
  LCD_CmdWrite(0x7B);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x7C);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x7D);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x7E);
  LCD_DataWrite(HY>>8);
}
//??b?|
void Circle_Radius_R(unsigned short WX)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  LCD_CmdWrite(0x77);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x78);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x79);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x7A);
  LCD_DataWrite(WX>>8);
}

//???b?|
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  LCD_CmdWrite(0x77);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x78);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x79);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x7A);
  LCD_DataWrite(HY>>8);
}

//??????b?|
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  LCD_CmdWrite(0x77);
  LCD_DataWrite(WX);

  LCD_CmdWrite(0x78);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0x79);
  LCD_DataWrite(HY);

  LCD_CmdWrite(0x7A);
  LCD_DataWrite(HY>>8);
}

//[84h]=========================================================================
void Set_PWM_Prescaler_1_to_256(unsigned short WX)
{
/*
PWM Prescaler Register
These 8 bits determine prescaler value for Timer 0 and 1.
Time base is ??Core_Freq / (Prescaler + 1)??
*/
  WX=WX-1;
  LCD_CmdWrite(0x84);
  LCD_DataWrite(WX);
}
//[85h]=========================================================================
void Select_PWM1_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb7;
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb7;
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb5;
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb5;
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb5;
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb5;
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
//[85h].[bit3][bit2]=========================================================================
/*
XPWM[1] pin function control
0X:	XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth insufficient + Memory access out of range)
10:	XPWM[1] enabled and controlled by PWM timer 1
11:	XPWM[1] output oscillator clock
//If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
*/
void Select_PWM1_is_ErrorFlag(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Select_PWM1(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb3;
  temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Select_PWM1_is_Osc_Clock(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb3;
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
//[85h].[bit1][bit0]=========================================================================
/*
XPWM[0] pin function control
0X: XPWM[0] becomes GPIO-C[7]
10: XPWM[0] enabled and controlled by PWM timer 0
11: XPWM[0] output core clock
*/
void Select_PWM0_is_GPIO_C7(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Select_PWM0(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb1;
  temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Select_PWM0_is_Core_Clock(void)
{
  unsigned char temp;
  
  LCD_CmdWrite(0x85);
  temp = LCD_DataRead();
  temp |= cSetb1;
  temp |= cSetb0;
  LCD_DataWrite(temp);
}
//[86h]=========================================================================
//[86h]PWM1
void Enable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Disable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Auto_Reload_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb5;
  LCD_DataWrite(temp);
}
void One_Shot_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Start_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Stop_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Disable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb3;
  LCD_DataWrite(temp);
}
void Enable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Disable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Auto_Reload_PWM0(void)
{
/*
PWM Timer 0 auto reload on/off
Determine auto reload on/off for Timer 0. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb1;
  LCD_DataWrite(temp);
}
void One_Shot_PWM0(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb1;
  LCD_DataWrite(temp);
}
void Start_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Stop_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
  unsigned char temp;
  LCD_CmdWrite(0x86);
  temp = LCD_DataRead();
  temp &= cClrb0;
  LCD_DataWrite(temp);
}
//[87h]=========================================================================
void Set_Timer0_Dead_Zone_Length(unsigned char temp)
{
/*
Timer 0 Dead zone length register
These 8 bits determine the dead zone length. The 1 unit time of
the dead zone length is equal to that of timer 0.
*/
  LCD_CmdWrite(0x87);
  LCD_DataWrite(temp);
}
//[88h][89h]=========================================================================
void Set_Timer0_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
  LCD_CmdWrite(0x88);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0x89);
  LCD_DataWrite(WX>>8);
}
//[8Ah][8Bh]=========================================================================
void Set_Timer0_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter??s real time value when PWM timer start.
*/
  LCD_CmdWrite(0x8A);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0x8B);
  LCD_DataWrite(WX>>8);
}
//[8Ch][8Dh]=========================================================================
void Set_Timer1_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
  LCD_CmdWrite(0x8C);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0x8D);
  LCD_DataWrite(WX>>8);
}
//[8Eh][8Fh]=========================================================================
void Set_Timer1_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter??s real time value when PWM timer start.
*/
  LCD_CmdWrite(0x8E);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0x8F);
  LCD_DataWrite(WX>>8);
}


//[90h]~[B5h]=========================================================================

//[90h]=========================================================================
void BTE_Enable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetb4 ;
  LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void BTE_Disable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrb4 ;
  LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void Check_BTE_Busy(void)
{	
/*
BTE Function Status
0 : BTE Function is idle.
1 : BTE Function is busy.
*/
  unsigned char temp; 	
  do
  {
    temp=LCD_StatusRead();
  }while(temp&0x08);

}
//[90h]=========================================================================
void Pattern_Format_8X8(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrb0 ;
  LCD_DataWrite(temp);
}	
//[90h]=========================================================================
void Pattern_Format_16X16(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetb0 ;
    LCD_DataWrite(temp);
}	

//[91h]=========================================================================
void BTE_ROP_Code(unsigned char setx)
{	
/*
BTE ROP Code[Bit7:4]
  
0000 : 0(Blackness)
0001 : ~S0.~S1 or ~ ( S0+S1 )
0010 : ~S0.S1
0011 : ~S0
0100 : S0.~S1
0101 : ~S1
0110 : S0^S1
0111 : ~S0+~S1 or ~ ( S0.S1 )
1000 : S0.S1
1001 : ~ ( S0^S1 )
1010 : S1
1011 : ~S0+S1
1100 : S0
1101 : S0+~S1
1110 : S0+S1
1111 : 1 ( Whiteness )
*/
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0x0f ;
    temp |= (setx<<4);
    LCD_DataWrite(temp);
}
  
//[91h]=========================================================================
void BTE_Operation_Code(unsigned char setx)
{	
/*
BTE Operation Code[Bit3:0]
  
0000 : MPU Write BTE with ROP.
0001 : MPU Read BTE w/o ROP.
0010 : Memory copy (move) BTE in positive direction with ROP.
0011 : Memory copy (move) BTE in negative direction with ROP.
0100 : MPU Transparent Write BTE. (w/o ROP.)
0101 : Transparent Memory copy (move) BTE in positive direction (w/o ROP.)
0110 : Pattern Fill with ROP.
0111 : Pattern Fill with key-chroma
1000 : Color Expansion
1001 : Color Expansion with transparency
1010 : Move BTE in positive direction with Alpha blending
1011 : MPU Write BTE with Alpha blending
1100 : Solid Fill
1101 : Reserved
1110 : Reserved
1111 : Reserved
*/
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0xf0 ;
    temp |= setx ;
    LCD_DataWrite(temp);

}
//[92h]=========================================================================
void BTE_S0_Color_8bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb6 ;
    temp &= cClrb5 ;
    LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_S0_Color_16bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb6 ;
    temp |= cSetb5 ;
    LCD_DataWrite(temp);

}	
//[92h]=========================================================================
void BTE_S0_Color_24bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb6 ;
    //temp |= cSetb5 ;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_8bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4 ;
    temp &= cClrb3 ;
    temp &= cClrb2 ;
    LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_S1_Color_16bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4 ;
    temp &= cClrb3 ;
    temp |= cSetb2 ;
    LCD_DataWrite(temp);

}
//[92h]=========================================================================
void BTE_S1_Color_24bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4 ;
    temp |= cSetb3 ;
    temp &= cClrb2 ;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_Constant(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4 ;
    temp |= cSetb3 ;
    temp |= cSetb2 ;
    LCD_DataWrite(temp);
}



//[92h]=========================================================================
void BTE_S1_Color_8bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb4 ;
    temp &= cClrb3 ;
    temp &= cClrb2 ;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_16bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb4 ;
    temp &= cClrb3 ;
    temp |= cSetb2 ;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_Destination_Color_8bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb1 ;
    temp &= cClrb0 ;
    LCD_DataWrite(temp);
}	
//[92h]=========================================================================
void BTE_Destination_Color_16bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb1 ;
    temp |= cSetb0 ;
    LCD_DataWrite(temp);

}	
//[92h]=========================================================================
void BTE_Destination_Color_24bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
10 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb1 ;
    //temp |= cSetb0 ;
    LCD_DataWrite(temp);
}


//[93h][94h][95h][96h]=========================================================================
void BTE_S0_Memory_Start_Address(unsigned long Addr)	
{
/*
[93h] BTE S0 Memory Start Address [7:0]
[94h] BTE S0 Memory Start Address [15:8]
[95h] BTE S0 Memory Start Address [23:16]
[96h] BTE S0 Memory Start Address [31:24]
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0x93,Addr);
  LCD_RegisterWrite(0x94,Addr>>8);
  LCD_RegisterWrite(0x95,Addr>>16);
  LCD_RegisterWrite(0x96,Addr>>24);
}


//[97h][98h]=========================================================================
void BTE_S0_Image_Width(unsigned short WX)	
{
/*
[97h] BTE S0 Image Width [7:0]
[98h] BTE S0 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0x97,WX);
  LCD_RegisterWrite(0x98,WX>>8);
}


//[99h][9Ah][9Bh][9Ch]=========================================================================
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[99h] BTE S0 Window Upper-Left corner X-coordination [7:0]
[9Ah] BTE S0 Window Upper-Left corner X-coordination [12:8]
[9Bh] BTE S0 Window Upper-Left corner Y-coordination [7:0]
[9Ch] BTE S0 Window Upper-Left corner Y-coordination [12:8]
*/
  LCD_RegisterWrite(0x99,WX);
  LCD_RegisterWrite(0x9A,WX>>8);

  LCD_RegisterWrite(0x9B,HY);
  LCD_RegisterWrite(0x9C,HY>>8);
}




//[9Dh][9Eh][9Fh][A0h]=========================================================================
void BTE_S1_Memory_Start_Address(unsigned long Addr)	
{
/*
[9Dh] BTE S1 Memory Start Address [7:0]
[9Eh] BTE S1 Memory Start Address [15:8]
[9Fh] BTE S1 Memory Start Address [23:16]
[A0h] BTE S1 Memory Start Address [31:24]
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0x9D,Addr);
  LCD_RegisterWrite(0x9E,Addr>>8);
  LCD_RegisterWrite(0x9F,Addr>>16);
  LCD_RegisterWrite(0xA0,Addr>>24);
}


//Input data format:R3G3B2
void S1_Constant_color_256(unsigned char temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp<<3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp<<6);
}

//Input data format:R5G6B6
void S1_Constant_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp>>8);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp>>3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp<<3);
}

//Input data format:R8G8B8
void S1_Constant_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp>>16);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp>>8);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp);
}




//[A1h][A2h]=========================================================================
void BTE_S1_Image_Width(unsigned short WX)	
{
/*
[A1h] BTE S1 Image Width [7:0]
[A2h] BTE S1 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0xA1,WX);
  LCD_RegisterWrite(0xA2,WX>>8);
}


//[A3h][A4h][A5h][A6h]=========================================================================
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[A3h] BTE S1 Window Upper-Left corner X-coordination [7:0]
[A4h] BTE S1 Window Upper-Left corner X-coordination [12:8]
[A5h] BTE S1 Window Upper-Left corner Y-coordination [7:0]
[A6h] BTE S1 Window Upper-Left corner Y-coordination [12:8]
*/
  LCD_RegisterWrite(0xA3,WX);
  LCD_RegisterWrite(0xA4,WX>>8);

  LCD_RegisterWrite(0xA5,HY);
  LCD_RegisterWrite(0xA6,HY>>8);
}




//[A7h][A8h][A9h][AAh]=========================================================================
void BTE_Destination_Memory_Start_Address(unsigned long Addr)	
{
/*
[A7h] BTE Destination Memory Start Address [7:0]
[A8h] BTE Destination Memory Start Address [15:8]
[A9h] BTE Destination Memory Start Address [23:16]
[AAh] BTE Destination Memory Start Address [31:24]
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0xA7,Addr);
  LCD_RegisterWrite(0xA8,Addr>>8);
  LCD_RegisterWrite(0xA9,Addr>>16);
  LCD_RegisterWrite(0xAA,Addr>>24);
}


//[ABh][ACh]=========================================================================
void BTE_Destination_Image_Width(unsigned short WX)	
{
/*
[ABh] BTE Destination Image Width [7:0]
[ACh] BTE Destination Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ??0?? internally.
*/
  LCD_RegisterWrite(0xAB,WX);
  LCD_RegisterWrite(0xAC,WX>>8);
}


//[ADh][AEh][AFh][B0h]=========================================================================
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[ADh] BTE Destination Window Upper-Left corner X-coordination [7:0]
[AEh] BTE Destination Window Upper-Left corner X-coordination [12:8]
[AFh] BTE Destination Window Upper-Left corner Y-coordination [7:0]
[B0h] BTE Destination Window Upper-Left corner Y-coordination [12:8]
*/
  LCD_RegisterWrite(0xAD,WX);
  LCD_RegisterWrite(0xAE,WX>>8);

  LCD_RegisterWrite(0xAF,HY);
  LCD_RegisterWrite(0xB0,HY>>8);
}


//[B1h][B2h][B3h][B4h]===============================================================

void BTE_Window_Size(unsigned short WX, unsigned short WY)

{
/*
[B1h] BTE Window Width [7:0]
[B2h] BTE Window Width [12:8]

[B3h] BTE Window Height [7:0]
[B4h] BTE Window Height [12:8]
*/
        LCD_RegisterWrite(0xB1,WX);
        LCD_RegisterWrite(0xB2,WX>>8);
  
      LCD_RegisterWrite(0xB3,WY);
        LCD_RegisterWrite(0xB4,WY>>8);
}

//[B5h]=========================================================================
void BTE_Alpha_Blending_Effect(unsigned char temp)
{	
/*
Window Alpha Blending effect for S0 & S1
The value of alpha in the color code ranges from 0.0 to 1.0,
where 0.0 represents a fully transparent color, and 1.0
represents a fully opaque color.
00h: 0
01h: 1/32
02h: 2/32
:
1Eh: 30/32
1Fh: 31/32
2Xh: 1
Output Effect = (S0 image x (1 - alpha setting value)) + (S1 image x alpha setting value)
*/
    LCD_CmdWrite(0xB5);
  LCD_DataWrite(temp);  
}


//[B6h]=========================================================================
void Start_SFI_DMA(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB6);
  temp = LCD_DataRead();
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

void Check_Busy_SFI_DMA(void)
{
  LCD_CmdWrite(0xB6);
  do
  {		
  }while((LCD_DataRead()&0x01)==0x01);
}


//[B7h]=========================================================================
void Select_SFI_0(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Select_SFI_1(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Select_SFI_Font_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode ?V for external cgrom
1: DMA mode ?V for cgram , pattern , bootstart image or osd
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Select_SFI_DMA_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode ?V for external cgrom
1: DMA mode ?V for cgram , pattern , bootstart image or osd
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Select_SFI_24bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void Select_SFI_32bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void Select_SFI_Waveform_Mode_0(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Select_SFI_Waveform_Mode_3(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Select_SFI_0_DummyRead(void)
{
/*[bit3][bit2]
Serial Flash /ROM Read Cycle 0 RW
00b: no dummy cycle mode
01b: 1 dummy cycle mode
10b: 2 dummy cycle mode
11b: 4 dummy cycle mode
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
  temp &= 0xF3;
  LCD_DataWrite(temp);
}
void Select_SFI_8_DummyRead(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
  temp &= 0xF3;
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Select_SFI_16_DummyRead(void)
{

  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
  temp &= 0xF3;
    temp |= cSetb3;
  LCD_DataWrite(temp);
}
void Select_SFI_24_DummyRead(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= 0x0c;
  LCD_DataWrite(temp);
}
void Select_SFI_Single_Mode(void)
{
/*[bit1][bit0]
Serial Flash /ROM I/F Data Latch Mode Select
0Xb: Single Mode
10b: Dual Mode 0.
11b: Dual Mode 1.
*/
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
  temp &= 0xFC;
  LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode0(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
  temp &= 0xFC;
    temp |= cSetb1;
  LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode1(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB7);
  temp = LCD_DataRead();
    temp |= 0x03;
  LCD_DataWrite(temp);
}

//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data)
{
    unsigned char temp;
  LCD_CmdWrite(0xB8);
  LCD_DataWrite(Data);
  while(Tx_FIFO_Empty_Flag()==0);	//?TF?A????U?@??
  temp = SPI_Master_FIFO_Data_Get();
  return temp;
}

unsigned char SPI_Master_FIFO_Data_Get(void)
{
   unsigned char temp;

  while(Rx_FIFO_Empty_Flag()==1);//???O?U????U????
  LCD_CmdWrite(0xB8);
  temp=LCD_DataRead();
  //while(Rx_FIFO_full_flag()); //?s??g?J16?????~??n
   return temp;
}

//REG[B9h] SPI master Control Register (SPIMCR2) 
void Mask_SPI_Master_Interrupt_Flag(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);

} 

void Select_nSS_drive_on_xnsfcs0(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrb5;
  LCD_DataWrite(temp);

}

void Select_nSS_drive_on_xnsfcs1(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb5;
  LCD_DataWrite(temp);

}

//0: inactive (nSS port will goes high) 
void nSS_Inactive(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrb4;
  LCD_DataWrite(temp);
}
//1: active (nSS port will goes low) 
void nSS_Active(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb4;
  LCD_DataWrite(temp);
}

//Interrupt enable for FIFO overflow error [OVFIRQEN] 
void OVFIRQEN_Enable(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb3;
  LCD_DataWrite(temp);

}
//Interrupt enable for while Tx FIFO empty & SPI engine/FSM idle
void EMTIRQEN_Enable(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb2;
  LCD_DataWrite(temp);
}


//At CPOL=0 the base value of the clock is zero   
//o  For CPHA=0, data are read on the clock's rising edge (low->high transition) and 
//data are changed on a falling edge (high->low clock transition). 
//o  For CPHA=1, data are read on the clock's falling edge and data are changed on a 
//rising edge. 

//At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
//o  For CPHA=0, data are read on clock's falling edge and data are changed on a 
//rising edge. 
//o  For CPHA=1, data are read on clock's rising edge and data are changed on a 
//falling edge.

void Reset_CPOL(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrb1;
  LCD_DataWrite(temp);
}

void Set_CPOL(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb1;
  LCD_DataWrite(temp);
}


void Reset_CPHA(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp &= cClrb0;
  LCD_DataWrite(temp);
}

void Set_CPHA(void)
{
  unsigned char temp;
  LCD_CmdWrite(0xB9);
  temp = LCD_DataRead();
  temp |= cSetb0;
  LCD_DataWrite(temp);
}


//REG[BAh] SPI master Status Register (SPIMSR) 
unsigned char Tx_FIFO_Empty_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x80)==0x80)
  return 1;
  else
  return 0;
}

unsigned char Tx_FIFO_Full_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x40)==0x40)
  return 1;
  else
  return 0;
} 

unsigned char Rx_FIFO_Empty_Flag(void)
{
  LCD_CmdWrite(0xBA);
  if((LCD_DataRead()&0x20)==0x20)
  return 1;
  else
  return 0;
} 

unsigned char Rx_FIFO_full_flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x10)==0x10)
   return 1;
   else
   return 0;
} 

unsigned char OVFI_Flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x08)==0x08)
   return 1;
   else
   return 0;
}

void Clear_OVFI_Flag(void)
{
   unsigned char temp;
   LCD_CmdWrite(0xBA);
   temp = LCD_DataRead();
   temp |= cSetb3;
   LCD_DataWrite(temp);
}

unsigned char EMTI_Flag(void)
{
   LCD_CmdWrite(0xBA);
   if((LCD_DataRead()&0x04)==0x04)
   return 1;
   else
   return 0;
}

void Clear_EMTI_Flag(void)
{
   unsigned char temp;
   LCD_CmdWrite(0xBA);
   temp = LCD_DataRead();
   temp |= cSetb2;
   LCD_DataWrite(temp);
}


//REG[BB] SPI Clock period (SPIDIV) 
void SPI_Clock_Period(unsigned char temp)
{
   LCD_CmdWrite(0xBB);
   LCD_DataWrite(temp);
} 

//[BCh][BDh][BEh][BFh]=========================================================================
void SFI_DMA_Source_Start_Address(unsigned long Addr)
{
/*
DMA Source START ADDRESS
This bits index serial flash address [7:0][15:8][23:16][31:24]
*/
  LCD_CmdWrite(0xBC);
  LCD_DataWrite(Addr);
  LCD_CmdWrite(0xBD);
  LCD_DataWrite(Addr>>8);
  LCD_CmdWrite(0xBE);
  LCD_DataWrite(Addr>>16);
  LCD_CmdWrite(0xBF);
  LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Start_Address(unsigned long Addr)
{
/*
DMA Destination START ADDRESS 
[1:0]Fix at 0
This bits index SDRAM address [7:0][15:8][23:16][31:24]
*/
  LCD_CmdWrite(0xC0);
  LCD_DataWrite(Addr);
  LCD_CmdWrite(0xC1);
  LCD_DataWrite(Addr>>8);
  LCD_CmdWrite(0xC2);
  LCD_DataWrite(Addr>>16);
  LCD_CmdWrite(0xC3);
  LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY)
{
/*
C0h
This register defines DMA Destination Window Upper-Left corner 
X-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [7:2] in SDRAM. 
C1h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
X-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [15:8] in SDRAM.
C2h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner
Y-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [23:16] in SDRAM. 
C3h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
Y-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [31:24] in SDRAM. 
*/
 
 LCD_CmdWrite(0xC0);
 LCD_DataWrite(WX);
 LCD_CmdWrite(0xC1);
 LCD_DataWrite(WX>>8);
 
 LCD_CmdWrite(0xC2);
 LCD_DataWrite(HY);
 LCD_CmdWrite(0xC3);
 LCD_DataWrite(HY>>8);
}



//[C6h][C7h][C8h][C9h]=========================================================================
void SFI_DMA_Transfer_Number(unsigned long Addr)
{
/*
Unit : Pixel
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
  LCD_CmdWrite(0xC6);
  LCD_DataWrite(Addr);
  LCD_CmdWrite(0xC7);
  LCD_DataWrite(Addr>>8);
  LCD_CmdWrite(0xC8);
  LCD_DataWrite(Addr>>16);
  LCD_CmdWrite(0xC9);
  LCD_DataWrite(Addr>>24);
}
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY)
{
/*
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
  LCD_CmdWrite(0xC6);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0xC7);
  LCD_DataWrite(WX>>8);

  LCD_CmdWrite(0xC8);
  LCD_DataWrite(HY);
  LCD_CmdWrite(0xC9);
  LCD_DataWrite(HY>>8);
}
//[CAh][CBh]=========================================================================
void SFI_DMA_Source_Width(unsigned short WX)
{
/*
DMA Source Picture Width [7:0][12:8]
Unit: pixel
*/
  LCD_CmdWrite(0xCA);
  LCD_DataWrite(WX);
  LCD_CmdWrite(0xCB);
  LCD_DataWrite(WX>>8);
}

//[CCh]=========================================================================

void Font_Select_UserDefine_Mode(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp |= cSetb7;
  temp &= cClrb6;
  LCD_DataWrite(temp);
}
void CGROM_Select_Internal_CGROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
  temp &= cClrb7;
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void CGROM_Select_Genitop_FontROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
  temp &= cClrb7;
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Font_Select_8x16_16x16(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash??s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Font_Select_12x24_24x24(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash??s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Font_Select_16x32_32x32(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash??s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_1(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_2(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_3(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_4(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCC);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
//[CDh]=========================================================================
void Enable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb7;
  LCD_DataWrite(temp);
}
void Disable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb7;
  LCD_DataWrite(temp);
}
void Font_Background_select_Transparency(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb6;
  LCD_DataWrite(temp);
}
void Font_Background_select_Color(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb6;
  LCD_DataWrite(temp);
}
void Font_0_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb4;
  LCD_DataWrite(temp);
}
void Font_90_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb4;
  LCD_DataWrite(temp);
}
void Font_Width_X1(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Font_Width_X2(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Font_Width_X3(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
  LCD_DataWrite(temp);
}
void Font_Width_X4(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb3;
    temp |= cSetb2;
  LCD_DataWrite(temp);
}
void Font_Height_X1(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Font_Height_X2(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}
void Font_Height_X3(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  LCD_DataWrite(temp);
}
void Font_Height_X4(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  LCD_CmdWrite(0xCD);
  temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
  LCD_DataWrite(temp);
}

//[CEh]=========================================================================
void GTFont_Select_GT21L16TW_GT21H16T1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT23L16U2W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT23L24T3Y_GT23H24T3Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT23L24M1Z(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT23L32S4W_GT23H32S4W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT20L24F6Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    temp |= cSetb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT21L24S1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb6;
    temp &= cClrb5;
  LCD_DataWrite(temp);
}
void GTFont_Select_GT22L16A1Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16TW / GT21H16T1W
001b: GT23L16U2W
010b: GT23L24T3Y / GT23H24T3Y
011b: GT23L24M1Z
100b: GT23L32S4W / GT23H32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
111b: GT22L16A1Y
*/
  unsigned char temp;
  LCD_CmdWrite(0xCE);
  temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb6;
    temp |= cSetb5;
  LCD_DataWrite(temp);
}

//[CFh]=========================================================================
void Set_GTFont_Decoder(unsigned char temp)
{
/*
[bit7-3]
FONT ROM Coding Setting
For specific GT serial Font ROM, the coding method must be set for decoding.
00000b: GB2312
00001b: GB12345/GB18030
00010b: BIG5
00011b: UNICODE
00100b: ASCII
00101b: UNI-Japanese
00110b: JIS0208
00111b: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
01000b: Korea
10001b: ISO-8859-1
10010b: ISO-8859-2
10011b: ISO-8859-3
10100b: ISO-8859-4
10101b: ISO-8859-5
10110b: ISO-8859-6
10111b: ISO-8859-7
11000b: ISO-8859-8
11001b: ISO-8859-9
11010b: ISO-8859-10
11011b: ISO-8859-11
11100b: ISO-8859-12
11101b: ISO-8859-13
11110b: ISO-8859-14
11111b: ISO-8859-15

[bit1-0]
ASCII / Latin/Greek/ Cyrillic / Arabic
    (ASCII)		(Latin/Greek/Cyrillic)	(Arabic)
00b		Normal			Normal 					NA
01b		Arial 		Variable Width 			Presentation Forms-A
10b		Roman 			NA 					Presentation Forms-B
11b		Bold			NA 						NA
*/
  LCD_CmdWrite(0xCF);
  LCD_DataWrite(temp);
}
//[D0h]=========================================================================
void Font_Line_Distance(unsigned char temp)
{
/*[bit4-0]
Font Line Distance Setting
Setting the font character line distance when setting memory font
write cursor auto move. (Unit: pixel)
*/
  LCD_CmdWrite(0xD0);
  LCD_DataWrite(temp);
}
//[D1h]=========================================================================
void Set_Font_to_Font_Width(unsigned char temp)
{
/*[bit5-0]
Font to Font Width Setting (Unit: pixel)
*/
  LCD_CmdWrite(0xD1);
  LCD_DataWrite(temp);
}
//[D2h]~[D4h]=========================================================================
void Foreground_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE)
{
/*
[D2h] Foreground Color - Red, for draw, text or color expansion
[D3h] Foreground Color - Green, for draw, text or color expansion
[D4h] Foreground Color - Blue, for draw, text or color expansion
*/  
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(RED);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(GREEN);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(BLUE);
}

//Input data format:R3G3B2  
void Foreground_color_256(unsigned char temp) 
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp<<3);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp<<6);
}
 
//Input data format:R5G6B5 
void Foreground_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp>>8);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp>>3);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp<<3);
}
 
//Input data format:R8G8B8 
void Foreground_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0xD2);
 LCD_DataWrite(temp>>16);
 
    LCD_CmdWrite(0xD3);
 LCD_DataWrite(temp>>8);
  
    LCD_CmdWrite(0xD4);
 LCD_DataWrite(temp);
}
 
 
 
//[D5h]~[D7h]=========================================================================
/*
[D5h] Background Color - Red, for Text or color expansion
[D6h] Background Color - Green, for Text or color expansion
[D7h] Background Color - Blue, for Text or color expansion
*/ 
void Background_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE)
{
   
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(RED);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(GREEN);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(BLUE);
}
 
//Input data format:R3G3B2
void Background_color_256(unsigned char temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp<<3);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp<<6);
}
 
//Input data format:R5G6B6
void Background_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp>>8);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp>>3);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp<<3);
}
 
//Input data format:R8G8B8
void Background_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0xD5);
 LCD_DataWrite(temp>>16);
  
    LCD_CmdWrite(0xD6);
 LCD_DataWrite(temp>>8);
   
    LCD_CmdWrite(0xD7);
 LCD_DataWrite(temp);
}

//[DBh]~[DEh]=========================================================================
void CGRAM_Start_address(unsigned long Addr)
{
/*
CGRAM START ADDRESS [31:0]
*/	 
    LCD_CmdWrite(0xDB);
  LCD_DataWrite(Addr);
    LCD_CmdWrite(0xDC);
  LCD_DataWrite(Addr>>8);
    LCD_CmdWrite(0xDD);
  LCD_DataWrite(Addr>>16);
    LCD_CmdWrite(0xDE);
  LCD_DataWrite(Addr>>24);
}

//[DFh]=========================================================================
/*
[bit7] Enter Power saving state
0: Normal state.
1: Enter power saving state.
[bit1][bit0] Power saving Mode definition
00: NA
01: Standby Mode
10: Suspend Mode
11: Sleep Mode
*/
void Power_Normal_Mode(void)
{
  LCD_CmdWrite(0xDF);
  LCD_DataWrite(0x00);
}
void Power_Saving_Standby_Mode(void)
{
  LCD_CmdWrite(0xDF);
  LCD_DataWrite(0x01);
  LCD_CmdWrite(0xDF);
  LCD_DataWrite(0x81);
}
void Power_Saving_Suspend_Mode(void)
{
  LCD_CmdWrite(0xDF);
//	LCD_DataWrite(0x02);
//	LCD_CmdWrite(0xDF);
  LCD_DataWrite(0x82);
}
void Power_Saving_Sleep_Mode(void)
{
  LCD_CmdWrite(0xDF);
//	LCD_DataWrite(0x03);
//	LCD_CmdWrite(0xDF);
  LCD_DataWrite(0x83);
}


//[E5h]~[E6h]=========================================================================
void RA8876_I2CM_Clock_Prescale(unsigned short WX)
{
/*
I2C Master Clock Pre-scale [7:0]
I2C Master Clock Pre-scale [15:8]
*/  
    LCD_CmdWrite(0xE5);
 LCD_DataWrite(WX);
    LCD_CmdWrite(0xE6);
 LCD_DataWrite(WX>>8);
}
//[E7h]=========================================================================
void RA8876_I2CM_Transmit_Data(unsigned char temp)
{
/*
I2C Master Transmit[7:0]
*/  
    LCD_CmdWrite(0xE7);
 LCD_DataWrite(temp);
}
//[E8h]=========================================================================
unsigned char RA8876_I2CM_Receiver_Data(void)
{
/*
I2C Master Receiver [7:0]
*/  
 unsigned char temp;
    LCD_CmdWrite(0xE8);
 temp=LCD_DataRead();
 return temp;
}
//[E9h]=========================================================================
/*
[bit7] START
Generate (repeated) start condition and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit6] STOP
Generate stop condition and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit5]
READ(READ and WRITE can??t be used simultaneously)
Read form slave and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit4]
WRITE(READ and WRITE can??t be used simultaneously)
Write to slave and be cleared by hardware automatically
Note : This bit is always read as 0.
[bit3] ACKNOWLEDGE
When as a I2C master receiver
0 : Sent ACK.
1 : Sent NACK.
Note : This bit is always read as 0.
[bit0] Noise Filter
0 : Enable.
1 : Disable.
*/
 
void RA8876_I2CM_Stop(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x40);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x41);
#endif
}
void RA8876_I2CM_Read_With_Ack(void)
{
 
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x20);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x21);
#endif
}
 
void RA8876_I2CM_Read_With_Nack(void)
{
 
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x68);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x69);
#endif
}
 
void RA8876_I2CM_Write_With_Start(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x90);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x91);
#endif
 
}
 
void RA8876_I2CM_Write(void)
{
 LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 LCD_DataWrite(0x10);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 LCD_DataWrite(0x11);
#endif
}
 
 
 
//[EAh]=========================================================================
/*
 0=Ack
 1=Nack
*/
unsigned char RA8876_I2CM_Check_Slave_ACK(void)
{ 
unsigned char temp;
/*[bit7]
Received acknowledge from slave
0 : Acknowledge received.
1 : No Acknowledge received. 
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x80)==0x80)
  return 1;
 else
  return 0; 
}
 

/*
 0=Idle
 1=Busy
*/
unsigned char RA8876_I2CM_Bus_Busy(void)
{
 unsigned char temp; 
 
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x40)==0x40)
  return 1;
 else
  return 0;    
}
 
/*
 0=Complete
 1=Transferring
*/
unsigned char RA8876_I2CM_transmit_Progress(void)
{
 unsigned char temp; 
/*[bit6]
 0=Complete
 1=Transferring
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 if((temp&0x02)==0x02)
 return 1;
 else
 return 0;
}
 
 /*
 0= Arbitration win
 1= Arbitration lost
*/
unsigned char RA8876_I2CM_Arbitration(void)
{
 unsigned char temp; 
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 LCD_CmdWrite(0xEA);
 temp=LCD_DataRead();
 temp&=0x01;
 return temp;
}


//[F0h]=========================================================================
void Set_GPIO_A_In_Out(unsigned char temp)
{
/*
GPO-A_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
  LCD_CmdWrite(0xF0);
  LCD_DataWrite(temp);
}
//[F1h]=========================================================================
void Write_GPIO_A_7_0(unsigned char temp)
{
/*
GPI-A[7:0] : General Purpose Input, share with DB[15:8]
GPO-A[7:0] : General Purpose Output, share with DB[15:8]
*/
  LCD_CmdWrite(0xF1);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_A_7_0(void)
{
/*
GPI-A[7:0] : General Purpose Input, share with DB[15:8]
GPO-A[7:0] : General Purpose Output, share with DB[15:8]
*/
  unsigned char temp;
  LCD_CmdWrite(0xF1);
  temp=LCD_DataRead();
  return temp;
}
//[F2h]=========================================================================
void Write_GPIO_B_7_4(unsigned char temp)
{
/*
GPI-B[7:0] : General Purpose Input ; share with {XKIN[3:0], XA0, XnWR, XnRD, XnCS}
GPO-B[7:4] : General Purpose Output ; share with XKOUT[3:0] ;
*/
  LCD_CmdWrite(0xF2);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_B_7_0(void)
{
/*
GPI-B[7:0] : General Purpose Input ; share with {XKIN[3:0], XA0, XnWR, XnRD, XnCS}
GPO-B[7:4] : General Purpose Output ; share with XKOUT[3:0] ;
*/
  unsigned char temp;
  LCD_CmdWrite(0xF2);
  temp=LCD_DataRead();
  return temp;
}

//[F3h]=========================================================================
void Set_GPIO_C_In_Out(unsigned char temp)
{
/*
GPIO-C_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
  LCD_CmdWrite(0xF3);
  LCD_DataWrite(temp);
}
//[F4h]=========================================================================
void Write_GPIO_C_7_0(unsigned char temp)
{
/*
GPIO-C[7:0] : General Purpose Input / Output
share with {XPWM0, XI2CSDA, XI2CSCL, XnSFCS1, XnSFCS0,XMISO, XMOSI, XSCLK}
*/
  LCD_CmdWrite(0xF4);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_C_7_0(void)
{
/*
GPIO-C[7:0] : General Purpose Input / Output
share with {XPWM0, XI2CSDA, XI2CSCL, XnSFCS1, XnSFCS0,XMISO, XMOSI, XSCLK}
*/
  unsigned char temp;
  LCD_CmdWrite(0xF4);
  temp=LCD_DataRead();
  return temp;
}
//[F5h]=========================================================================
void Set_GPIO_D_In_Out(unsigned char temp)
{
/*
GPIO-D_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
  LCD_CmdWrite(0xF5);
  LCD_DataWrite(temp);
}
//[F6h]=========================================================================
void Write_GPIO_D_7_0(unsigned char temp)
{
/*
GPIO-D[7:0] : General Purpose Input/Output
*/
  LCD_CmdWrite(0xF6);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_D_7_0(void)
{
/*
GPIO-D[7:0] : General Purpose Input/Output
*/
  unsigned char temp;
  LCD_CmdWrite(0xF6);
  temp=LCD_DataRead();
  return temp;
}
//[F7h]=========================================================================
void Set_GPIO_E_In_Out(unsigned char temp)
{
/*
GPIO-E_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
  LCD_CmdWrite(0xF7);
  LCD_DataWrite(temp);
}
//[F8h]=========================================================================
void Write_GPIO_E_7_0(unsigned char temp)
{
/*
GPIO-E[7:0] : General Purpose Input/Output.
share with {PDAT[23:19], PDAT[15:13]}
*/
  LCD_CmdWrite(0xF8);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_E_7_0(void)
{
/*
GPIO-E[7:0] : General Purpose Input/Output.
share with {PDAT[23:19], PDAT[15:13]}
*/
  unsigned char temp;
  LCD_CmdWrite(0xF8);
  temp=LCD_DataRead();
  return temp;
}
//[F9h]=========================================================================
void Set_GPIO_F_In_Out(unsigned char temp)
{
/*
GPIO-F_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
  LCD_CmdWrite(0xF9);
  LCD_DataWrite(temp);
}
//[FAh]=========================================================================
void Write_GPIO_F_7_0(unsigned char temp)
{
/*
GPIO-F[7:0] : General Purpose Input/Output.
share with {XPDAT[12:10], XPDAT[7:3]}
*/
  LCD_CmdWrite(0xFA);
  LCD_DataWrite(temp);
}
unsigned char Read_GPIO_F_7_0(void)
{
/*
GPIO-F[7:0] : General Purpose Input/Output.
share with {XPDAT[12:10], XPDAT[7:3]}
*/
  unsigned char temp;
  LCD_CmdWrite(0xFA);
  temp=LCD_DataRead();
  return temp;
}

//[FBh]=========================================================================


void Long_Key_enable(void)
{
 /*
Key-Scan Control Register 1
[bit6]		LongKey Enable Bit
1 : Enable. Long key period is set by KSCR2 bit4-2.
0 : Disable.
*/
  unsigned char temp;
  LCD_CmdWrite(0xFB);
  temp=LCD_DataRead();
  temp |= cSetb6;
  LCD_DataWrite(temp);
}


void Key_Scan_Freg(unsigned char setx)
{
/*KF2-0: Key-Scan Frequency */ 
  unsigned char temp;
  LCD_CmdWrite(0xFB);
  temp = LCD_DataRead();
  temp &= 0xf0;
  temp|= (setx&0x07);
  LCD_DataWrite(temp);  
}


//[FCh]=========================================================================

void Key_Scan_Wakeup_Function_Enable(void)
{
/*
Key-Scan Controller Register 2
[bit7]	
Key-Scan Wakeup Function Enable Bit
0: Key-Scan Wakeup function is disabled.
1: Key-Scan Wakeup function is enabled.
*/
  unsigned char temp;
  LCD_CmdWrite(0xFC);
  temp=LCD_DataRead();
  temp |= cSetb7;
  LCD_DataWrite(temp);
}


void Long_Key_Timing_Adjustment(unsigned char setx)
{
 /*Long Key Timing Adjustment*/ 
  unsigned char temp,temp1;
  temp = setx & 0x1c;
  LCD_CmdWrite(0xFC);
  temp1 = LCD_DataRead();
  temp1|=temp;
  LCD_DataWrite(temp1);   
 
}

unsigned char Numbers_of_Key_Hit(void)
{  
   unsigned char temp;
   LCD_CmdWrite(0xFC);
   temp = LCD_DataRead();   //read key touch number
   temp = temp & 0x03;	 //?T?{???X?????Q???U
   return temp;
}

//[FDh][FEh][FFh]=========================================================================
unsigned char Read_Key_Strobe_Data_0(void)
{
/*
Key Strobe Data 0
The corresponding key code 0 that is pressed.
*/
  unsigned char temp;
  LCD_CmdWrite(0xFD);
  temp=LCD_DataRead();
  return temp;
}
unsigned char Read_Key_Strobe_Data_1(void)
{
/*
Key Strobe Data 1
The corresponding key code 1 that is pressed.
*/
  unsigned char temp;
  LCD_CmdWrite(0xFE);
  temp=LCD_DataRead();
  return temp;
}
unsigned char Read_Key_Strobe_Data_2(void)
{
/*
Key Strobe Data 2
The corresponding key code 2 that is pressed.
*/
  unsigned char temp;
  LCD_CmdWrite(0xFF);
  temp=LCD_DataRead();
  return temp;
}



void Show_String(char *str)
{   
  Text_Mode();
  LCD_CmdWrite(0x04);
  while(*str != '\0')
  {
   LCD_DataWrite(*str);
   Check_Mem_WR_FIFO_not_Full();
  ++str;   
  }
   Check_2D_Busy();

   Graphic_Mode(); //back to graphic mode
}


void Show_picture(unsigned long numbers,const unsigned short *datap)
{   
  unsigned long i;

  LCD_CmdWrite(0x04);  
  for(i=0;i<numbers;i++)
   {
   LCD_DataWrite(datap[i]);
   LCD_DataWrite(datap[i]>>8);
   Check_Mem_WR_FIFO_not_Full();
   }

}  

void Show_picture1(unsigned long numbers,const unsigned short *datap)
{   
  unsigned long i;

  LCD_CmdWrite(0x04);  
  for(i=0;i<numbers;i++)
   {
   LCD_DataWrite(datap[i]>>8);
   LCD_DataWrite(datap[i]);
   Check_Mem_WR_FIFO_not_Full();
   }

} 

void Graphic_cursor_initial(void)
{
  unsigned int i ;

//???g?Jgraphic cursor
  Memory_Select_Graphic_Cursor_RAM(); 
  Graphic_Mode();

    Select_Graphic_Cursor_1();
    LCD_CmdWrite(0x04);
    for(i=0;i<256;i++)
    {					 
     LCD_DataWrite(gImage_pen_il[i]);
    }

    Select_Graphic_Cursor_2();
    LCD_CmdWrite(0x04);
    for(i=0;i<256;i++)
    {					 
     LCD_DataWrite(gImage_arrow_il[i]);
    }

     Select_Graphic_Cursor_3();
     LCD_CmdWrite(0x04);
     for(i=0;i<256;i++)
     {					 
      LCD_DataWrite(gImage_busy_im[i]);

     }

     Select_Graphic_Cursor_4();
     LCD_CmdWrite(0x04);
     for(i=0;i<256;i++)
     {					 
      LCD_DataWrite(gImage_no_im[i]);

     }

    Memory_Select_SDRAM();//?g??????^SDRAM

    Set_Graphic_Cursor_Color_1(0xff);
    Set_Graphic_Cursor_Color_2(0x00);
    Graphic_Cursor_XY(0,0);
    Enable_Graphic_Cursor();
}


void Text_cursor_initial(void)
{

   //Disable_Text_Cursor();
    Enable_Text_Cursor_Blinking();
    //Disable_Text_Cursor_Blinking();
    Blinking_Time_Frames(10);
    //**[3E][3Fh]**//
    Text_Cursor_H_V(1,15);
  Enable_Text_Cursor();

}



unsigned int ASCII_Table[] =
       {
         /* Space ' ' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '!' */
         0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0000, 0x0000,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '"' */
         0x0000, 0x0000, 0x00CC, 0x00CC, 0x00CC, 0x00CC, 0x00CC, 0x00CC,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '#' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C60, 0x0C60,
         0x0C60, 0x0630, 0x0630, 0x1FFE, 0x1FFE, 0x0630, 0x0738, 0x0318,
         0x1FFE, 0x1FFE, 0x0318, 0x0318, 0x018C, 0x018C, 0x018C, 0x0000,
         /* '$' */
         0x0000, 0x0080, 0x03E0, 0x0FF8, 0x0E9C, 0x1C8C, 0x188C, 0x008C,
         0x0098, 0x01F8, 0x07E0, 0x0E80, 0x1C80, 0x188C, 0x188C, 0x189C,
         0x0CB8, 0x0FF0, 0x03E0, 0x0080, 0x0080, 0x0000, 0x0000, 0x0000,
         /* '%' */
         0x0000, 0x0000, 0x0000, 0x180E, 0x0C1B, 0x0C11, 0x0611, 0x0611,
         0x0311, 0x0311, 0x019B, 0x018E, 0x38C0, 0x6CC0, 0x4460, 0x4460,
         0x4430, 0x4430, 0x4418, 0x6C18, 0x380C, 0x0000, 0x0000, 0x0000,
         /* '&' */
         0x0000, 0x01E0, 0x03F0, 0x0738, 0x0618, 0x0618, 0x0330, 0x01F0,
         0x00F0, 0x00F8, 0x319C, 0x330E, 0x1E06, 0x1C06, 0x1C06, 0x3F06,
         0x73FC, 0x21F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ''' */
         0x0000, 0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '(' */
         0x0000, 0x0200, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x0060, 0x0060,
         0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030,
         0x0060, 0x0060, 0x00C0, 0x00C0, 0x0180, 0x0300, 0x0200, 0x0000,
         /* ')' */
         0x0000, 0x0020, 0x0060, 0x00C0, 0x0180, 0x0180, 0x0300, 0x0300,
         0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600,
         0x0300, 0x0300, 0x0180, 0x0180, 0x00C0, 0x0060, 0x0020, 0x0000,
         /* '*' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0,
         0x06D8, 0x07F8, 0x01E0, 0x0330, 0x0738, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '+' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x3FFC, 0x3FFC, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ',' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0180, 0x0180, 0x0100, 0x0100, 0x0080, 0x0000, 0x0000,
         /* '-' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '.' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '/' */
         0x0000, 0x0C00, 0x0C00, 0x0600, 0x0600, 0x0600, 0x0300, 0x0300,
         0x0300, 0x0380, 0x0180, 0x0180, 0x0180, 0x00C0, 0x00C0, 0x00C0,
         0x0060, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '0' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C18, 0x180C, 0x180C, 0x180C,
         0x180C, 0x180C, 0x180C, 0x180C, 0x180C, 0x180C, 0x0C18, 0x0E38,
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '1' */
         0x0000, 0x0100, 0x0180, 0x01C0, 0x01F0, 0x0198, 0x0188, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '2' */
         0x0000, 0x03E0, 0x0FF8, 0x0C18, 0x180C, 0x180C, 0x1800, 0x1800,
         0x0C00, 0x0600, 0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018,
         0x1FFC, 0x1FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '3' */
         0x0000, 0x01E0, 0x07F8, 0x0E18, 0x0C0C, 0x0C0C, 0x0C00, 0x0600,
         0x03C0, 0x07C0, 0x0C00, 0x1800, 0x1800, 0x180C, 0x180C, 0x0C18,
         0x07F8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '4' */
         0x0000, 0x0C00, 0x0E00, 0x0F00, 0x0F00, 0x0D80, 0x0CC0, 0x0C60,
         0x0C60, 0x0C30, 0x0C18, 0x0C0C, 0x3FFC, 0x3FFC, 0x0C00, 0x0C00,
         0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '5' */
         0x0000, 0x0FF8, 0x0FF8, 0x0018, 0x0018, 0x000C, 0x03EC, 0x07FC,
         0x0E1C, 0x1C00, 0x1800, 0x1800, 0x1800, 0x180C, 0x0C1C, 0x0E18,
         0x07F8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '6' */
         0x0000, 0x07C0, 0x0FF0, 0x1C38, 0x1818, 0x0018, 0x000C, 0x03CC,
         0x0FEC, 0x0E3C, 0x1C1C, 0x180C, 0x180C, 0x180C, 0x1C18, 0x0E38,
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '7' */
         0x0000, 0x1FFC, 0x1FFC, 0x0C00, 0x0600, 0x0600, 0x0300, 0x0380,
         0x0180, 0x01C0, 0x00C0, 0x00E0, 0x0060, 0x0060, 0x0070, 0x0030,
         0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '8' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C18, 0x0C18, 0x0C18, 0x0638,
         0x07F0, 0x07F0, 0x0C18, 0x180C, 0x180C, 0x180C, 0x180C, 0x0C38,
         0x0FF8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '9' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C1C, 0x180C, 0x180C, 0x180C,
         0x1C1C, 0x1E38, 0x1BF8, 0x19E0, 0x1800, 0x0C00, 0x0C00, 0x0E1C,
         0x07F8, 0x01F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ':' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ';' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0180, 0x0180, 0x0100, 0x0100, 0x0080, 0x0000, 0x0000, 0x0000,
         /* '<' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x1000, 0x1C00, 0x0F80, 0x03E0, 0x00F8, 0x0018, 0x00F8, 0x03E0,
         0x0F80, 0x1C00, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '=' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x1FF8, 0x0000, 0x0000, 0x0000, 0x1FF8, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '>' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0008, 0x0038, 0x01F0, 0x07C0, 0x1F00, 0x1800, 0x1F00, 0x07C0,
         0x01F0, 0x0038, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '?' */
         0x0000, 0x03E0, 0x0FF8, 0x0C18, 0x180C, 0x180C, 0x1800, 0x0C00,
         0x0600, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x00C0, 0x0000, 0x0000,
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '@' */
         0x0000, 0x0000, 0x07E0, 0x1818, 0x2004, 0x29C2, 0x4A22, 0x4411,
         0x4409, 0x4409, 0x4409, 0x2209, 0x1311, 0x0CE2, 0x4002, 0x2004,
         0x1818, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'A' */
         0x0000, 0x0380, 0x0380, 0x06C0, 0x06C0, 0x06C0, 0x0C60, 0x0C60,
         0x1830, 0x1830, 0x1830, 0x3FF8, 0x3FF8, 0x701C, 0x600C, 0x600C,
         0xC006, 0xC006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'B' */
         0x0000, 0x03FC, 0x0FFC, 0x0C0C, 0x180C, 0x180C, 0x180C, 0x0C0C,
         0x07FC, 0x0FFC, 0x180C, 0x300C, 0x300C, 0x300C, 0x300C, 0x180C,
         0x1FFC, 0x07FC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'C' */
         0x0000, 0x07C0, 0x1FF0, 0x3838, 0x301C, 0x700C, 0x6006, 0x0006,
         0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x6006, 0x700C, 0x301C,
         0x1FF0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'D' */
         0x0000, 0x03FE, 0x0FFE, 0x0E06, 0x1806, 0x1806, 0x3006, 0x3006,
         0x3006, 0x3006, 0x3006, 0x3006, 0x3006, 0x1806, 0x1806, 0x0E06,
         0x0FFE, 0x03FE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'E' */
         0x0000, 0x3FFC, 0x3FFC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x1FFC, 0x1FFC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x3FFC, 0x3FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'F' */
         0x0000, 0x3FF8, 0x3FF8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x1FF8, 0x1FF8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x0018, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'G' */
         0x0000, 0x0FE0, 0x3FF8, 0x783C, 0x600E, 0xE006, 0xC007, 0x0003,
         0x0003, 0xFE03, 0xFE03, 0xC003, 0xC007, 0xC006, 0xC00E, 0xF03C,
         0x3FF8, 0x0FE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'H' */
         0x0000, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x3FFC, 0x3FFC, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x300C, 0x300C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'I' */
         0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'J' */
         0x0000, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600,
         0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0618, 0x0618, 0x0738,
         0x03F0, 0x01E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'K' */
         0x0000, 0x3006, 0x1806, 0x0C06, 0x0606, 0x0306, 0x0186, 0x00C6,
         0x0066, 0x0076, 0x00DE, 0x018E, 0x0306, 0x0606, 0x0C06, 0x1806,
         0x3006, 0x6006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'L' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x1FF8, 0x1FF8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'M' */
         0x0000, 0xE00E, 0xF01E, 0xF01E, 0xF01E, 0xD836, 0xD836, 0xD836,
         0xD836, 0xCC66, 0xCC66, 0xCC66, 0xC6C6, 0xC6C6, 0xC6C6, 0xC6C6,
         0xC386, 0xC386, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'N' */
         0x0000, 0x300C, 0x301C, 0x303C, 0x303C, 0x306C, 0x306C, 0x30CC,
         0x30CC, 0x318C, 0x330C, 0x330C, 0x360C, 0x360C, 0x3C0C, 0x3C0C,
         0x380C, 0x300C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'O' */
         0x0000, 0x07E0, 0x1FF8, 0x381C, 0x700E, 0x6006, 0xC003, 0xC003,
         0xC003, 0xC003, 0xC003, 0xC003, 0xC003, 0x6006, 0x700E, 0x381C,
         0x1FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'P' */
         0x0000, 0x0FFC, 0x1FFC, 0x380C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x180C, 0x1FFC, 0x07FC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x000C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Q' */
         0x0000, 0x07E0, 0x1FF8, 0x381C, 0x700E, 0x6006, 0xE003, 0xC003,
         0xC003, 0xC003, 0xC003, 0xC003, 0xE007, 0x6306, 0x3F0E, 0x3C1C,
         0x3FF8, 0xF7E0, 0xC000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'R' */
         0x0000, 0x0FFE, 0x1FFE, 0x3806, 0x3006, 0x3006, 0x3006, 0x3806,
         0x1FFE, 0x07FE, 0x0306, 0x0606, 0x0C06, 0x1806, 0x1806, 0x3006,
         0x3006, 0x6006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'S' */
         0x0000, 0x03E0, 0x0FF8, 0x0C1C, 0x180C, 0x180C, 0x000C, 0x001C,
         0x03F8, 0x0FE0, 0x1E00, 0x3800, 0x3006, 0x3006, 0x300E, 0x1C1C,
         0x0FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'T' */
         0x0000, 0x7FFE, 0x7FFE, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'U' */
         0x0000, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x1818,
         0x1FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'V' */
         0x0000, 0x6003, 0x3006, 0x3006, 0x3006, 0x180C, 0x180C, 0x180C,
         0x0C18, 0x0C18, 0x0E38, 0x0630, 0x0630, 0x0770, 0x0360, 0x0360,
         0x01C0, 0x01C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'W' */
         0x0000, 0x6003, 0x61C3, 0x61C3, 0x61C3, 0x3366, 0x3366, 0x3366,
         0x3366, 0x3366, 0x3366, 0x1B6C, 0x1B6C, 0x1B6C, 0x1A2C, 0x1E3C,
         0x0E38, 0x0E38, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'X' */
         0x0000, 0xE00F, 0x700C, 0x3018, 0x1830, 0x0C70, 0x0E60, 0x07C0,
         0x0380, 0x0380, 0x03C0, 0x06E0, 0x0C70, 0x1C30, 0x1818, 0x300C,
         0x600E, 0xE007, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Y' */
         0x0000, 0xC003, 0x6006, 0x300C, 0x381C, 0x1838, 0x0C30, 0x0660,
         0x07E0, 0x03C0, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Z' */
         0x0000, 0x7FFC, 0x7FFC, 0x6000, 0x3000, 0x1800, 0x0C00, 0x0600,
         0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018, 0x000C, 0x0006,
         0x7FFE, 0x7FFE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '[' */
         0x0000, 0x03E0, 0x03E0, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060,
         0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060,
         0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x03E0, 0x03E0, 0x0000,
         /* '\' */
         0x0000, 0x0030, 0x0030, 0x0060, 0x0060, 0x0060, 0x00C0, 0x00C0,
         0x00C0, 0x01C0, 0x0180, 0x0180, 0x0180, 0x0300, 0x0300, 0x0300,
         0x0600, 0x0600, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ']' */
         0x0000, 0x03E0, 0x03E0, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300,
         0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300,
         0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x03E0, 0x03E0, 0x0000,
         /* '^' */
         0x0000, 0x0000, 0x01C0, 0x01C0, 0x0360, 0x0360, 0x0360, 0x0630,
         0x0630, 0x0C18, 0x0C18, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '_' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ''' */
         0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'a' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03F0, 0x07F8,
         0x0C1C, 0x0C0C, 0x0F00, 0x0FF0, 0x0CF8, 0x0C0C, 0x0C0C, 0x0F1C,
         0x0FF8, 0x18F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'b' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x03D8, 0x0FF8,
         0x0C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C38,
         0x0FF8, 0x03D8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'c' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x07F0,
         0x0E30, 0x0C18, 0x0018, 0x0018, 0x0018, 0x0018, 0x0C18, 0x0E30,
         0x07F0, 0x03C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'd' */
         0x0000, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1BC0, 0x1FF0,
         0x1C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C30,
         0x1FF0, 0x1BC0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'e' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x0FF0,
         0x0C30, 0x1818, 0x1FF8, 0x1FF8, 0x0018, 0x0018, 0x1838, 0x1C30,
         0x0FF0, 0x07C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'f' */
         0x0000, 0x0F80, 0x0FC0, 0x00C0, 0x00C0, 0x00C0, 0x07F0, 0x07F0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'g' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0DE0, 0x0FF8,
         0x0E18, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0E18,
         0x0FF8, 0x0DE0, 0x0C00, 0x0C0C, 0x061C, 0x07F8, 0x01F0, 0x0000,
         /* 'h' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x07D8, 0x0FF8,
         0x1C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818,
         0x1818, 0x1818, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'i' */
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'j' */
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00F8, 0x0078, 0x0000,
         /* 'k' */
         0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0C0C, 0x060C,
         0x030C, 0x018C, 0x00CC, 0x006C, 0x00FC, 0x019C, 0x038C, 0x030C,
         0x060C, 0x0C0C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'l' */
         0x0000, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'm' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3C7C, 0x7EFF,
         0xE3C7, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183,
         0xC183, 0xC183, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'n' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0798, 0x0FF8,
         0x1C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818,
         0x1818, 0x1818, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'o' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x0FF0,
         0x0C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C30,
         0x0FF0, 0x03C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'p' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03D8, 0x0FF8,
         0x0C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C38,
         0x0FF8, 0x03D8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0000,
         /* 'q' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1BC0, 0x1FF0,
         0x1C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C30,
         0x1FF0, 0x1BC0, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x0000,
         /* 'r' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07B0, 0x03F0,
         0x0070, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030,
         0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 's' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03E0, 0x03F0,
         0x0E38, 0x0C18, 0x0038, 0x03F0, 0x07C0, 0x0C00, 0x0C18, 0x0E38,
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 't' */
         0x0000, 0x0000, 0x0080, 0x00C0, 0x00C0, 0x00C0, 0x07F0, 0x07F0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x07C0, 0x0780, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'u' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1818, 0x1818,
         0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C38,
         0x1FF0, 0x19E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'v' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x180C, 0x0C18,
         0x0C18, 0x0C18, 0x0630, 0x0630, 0x0630, 0x0360, 0x0360, 0x0360,
         0x01C0, 0x01C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'w' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x41C1, 0x41C1,
         0x61C3, 0x6363, 0x6363, 0x6363, 0x3636, 0x3636, 0x3636, 0x1C1C,
         0x1C1C, 0x1C1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'x' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x381C, 0x1C38,
         0x0C30, 0x0660, 0x0360, 0x0360, 0x0360, 0x0360, 0x0660, 0x0C30,
         0x1C38, 0x381C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'y' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3018, 0x1830,
         0x1830, 0x1870, 0x0C60, 0x0C60, 0x0CE0, 0x06C0, 0x06C0, 0x0380,
         0x0380, 0x0380, 0x0180, 0x0180, 0x01C0, 0x00F0, 0x0070, 0x0000,
         /* 'z' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FFC, 0x1FFC,
         0x0C00, 0x0600, 0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018,
         0x1FFC, 0x1FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '{' */
         0x0000, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0,
         0x00C0, 0x0060, 0x0060, 0x0030, 0x0060, 0x0040, 0x00C0, 0x00C0,
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x0180, 0x0300, 0x0000, 0x0000,
         /* '|' */
         0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0000,
         /* '}' */
         0x0000, 0x0060, 0x00C0, 0x01C0, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0300, 0x0300, 0x0600, 0x0300, 0x0100, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x00C0, 0x0060, 0x0000, 0x0000,
         /* '~' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x10F0, 0x1FF8, 0x0F08, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
      };


unsigned int pattern16x16_16bpp[256] = { /* 0X10,0X10,0X00,0X10,0X00,0X10,0X01,0X1B, */
0XF800,0XF800,0XF800,0XF800,0XF800,0XF800,0XF800,0X9248,
0X89E6,0XF800,0XF800,0X8349,0X5B29,0XF800,0XF800,0XF800,
0XF800,0XF800,0XF800,0XF800,0XF800,0XF800,0X89E6,0XFDD6,
0XFE37,0X9247,0XF800,0X72C7,0XEFDC,0X636B,0XF800,0XF800,
0XF800,0XF800,0XF800,0XF800,0XF800,0X8948,0XFD93,0XFAA8,
0XD983,0XFDB3,0X9168,0XAA2B,0XF71F,0X72F0,0XF800,0XF800,
0XF800,0XF800,0XF800,0XF800,0XAA2B,0XFDFA,0XE1C4,0XD121,
0XD983,0XE1C4,0XFE3B,0X99CA,0XD5FC,0X8393,0XF800,0XF800,
0XF800,0XF800,0XF800,0XC1E7,0XFD56,0XEA29,0XAA88,0X6880,
0X7902,0XA247,0XC925,0XFDF8,0XC207,0XB9C7,0XF800,0XF800,
0XF800,0XF800,0XA104,0XFDD6,0XE208,0XD9C7,0X5800,0XFD33,
0XF4B1,0X70A0,0XD9C7,0XC966,0XFE17,0X98E3,0XF800,0XF800,
0XF800,0X8203,0XFD31,0XE205,0XC207,0X7800,0XB637,0XF7FF,
0XF7FF,0XB637,0X8000,0XB1A5,0XD9C4,0XFE15,0X6960,0XF800,
0X9264,0XFDB1,0XEA66,0XD9A4,0X7800,0XFCD1,0XEFDE,0XF7FF,
0XF7FF,0XE7BD,0XFCF2,0X8880,0XD9C4,0XD9C4,0XFEB5,0X8A23,
0X48E4,0X5105,0X4066,0X4066,0XCD58,0XFF5F,0XFFFE,0XFF9C,
0XFFFE,0XFF7C,0XFF7F,0XDDDA,0X3825,0X4887,0X5125,0X48E4,
0XA3AF,0XE5B7,0XEDBB,0XFE5E,0XFF3F,0X6A4D,0X62C9,0X62CA,
0X62EA,0X62EA,0X728D,0XFEFF,0XFE3D,0XEDBB,0XE597,0XABF0,
0X73CC,0XFFFD,0XEFDE,0XE79D,0XFFFF,0X94D4,0XFFFF,0XEF9F,
0XB597,0XFFFF,0X94D4,0XFFFF,0XF7FF,0XF7FE,0XFFFD,0X6BAC,
0X6B8B,0XFFFD,0XF7FE,0XFFFF,0XFFFF,0X4A6A,0XB5B8,0XC65A,
0XC63A,0XEF9F,0X5B0D,0XF7BF,0XFFFF,0XF7DE,0XFFFD,0X73CC,
0X6BE7,0XFFF9,0XFFF7,0XFF95,0XFFFB,0XAD50,0XDFFF,0XDFFF,
0XA69B,0XCFBF,0X9CAE,0XFFFB,0XFFF6,0XFFF7,0XFFF9,0X6BE7,
0X63A6,0XA5AE,0XCDEE,0XCDEE,0XCE34,0X6307,0X330D,0X32CC,
0X4B8F,0X434E,0X6307,0XCE54,0XC5AD,0XBD8D,0XADEF,0X6BE7,
0X7328,0XD614,0XFF5C,0XEEB9,0XEED9,0XF71A,0XFFDA,0XEF58,
0XE6F7,0XEF58,0XF73B,0XEF1A,0XEED9,0XFF7C,0XCDF3,0X7308,
0X5A65,0X49C3,0X3103,0X49C6,0X49C6,0X3964,0X3981,0X4203,
0X41E2,0X39A2,0X49C6,0X41A5,0X49C6,0X4164,0X4181,0X6AC7,
};

unsigned int f1[1600] = { /* 0X00,0X01,0XA0,0X00,0XA0,0X00, */
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X000C,0X1800,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X09FF,0X7FC0,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X1FFF,0X7FFC,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X7FFE,0X3FFF,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0003,
0XFFFC,0X1FFF,0XE000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X000F,0XFFF8,0X0FFF,0XF800,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X003F,0XFF87,0XF1FF,0XFC00,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0067,0XC807,0XF009,
0XF700,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0007,0XF000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0007,0XF000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0XF000,0X000F,0XF000,0X0007,0X0000,
0X0000,0X0000,0X0000,0X0000,0X000F,0X8000,0X000F,0XF000,
0X0003,0XF800,0X0000,0X0000,0X0000,0X0000,0X003F,0X0000,
0X000F,0XF800,0X0000,0X7E00,0X0000,0X0000,0X0000,0X0000,
0X01FE,0X0000,0X000F,0XF800,0X0000,0X3FC0,0X0000,0X0000,
0X0000,0X0000,0X07FC,0X0000,0X000F,0XF800,0X0000,0X1FF0,
0X0000,0X0000,0X0000,0X0000,0X3FF8,0X0000,0X000F,0XF800,
0X0000,0X0FF8,0X0000,0X0000,0X0000,0X0000,0X7FE0,0X0000,
0X000F,0XF000,0X0000,0X07FF,0X0000,0X0000,0X0000,0X0000,
0XFFC0,0X0000,0X0007,0XF000,0X0000,0X01FF,0X8000,0X0000,
0X0000,0X0001,0XFF80,0X0000,0X0007,0XF000,0X0000,0X00FF,
0XC000,0X0000,0X0000,0X0003,0XFF00,0X0000,0X0007,0XF000,
0X0000,0X007F,0XC000,0X0000,0X0000,0X0003,0XFE00,0X0000,
0X0007,0XF000,0X0000,0X003F,0XC000,0X0000,0X0000,0X0003,
0XFC00,0X0000,0X0007,0XF000,0X0000,0X001F,0XE000,0X0000,
0X0000,0X0004,0XF800,0X0000,0X0003,0XE000,0X0000,0X000F,
0XC000,0X0000,0X0000,0X0028,0X3000,0X0000,0X0007,0XF000,
0X0000,0X0004,0X0800,0X0000,0X0000,0X0078,0X1800,0X0000,
0X0003,0XE000,0X0000,0X0000,0X0F00,0X0000,0X0000,0X00F8,
0X3C00,0X0000,0X0001,0XC000,0X0000,0X001E,0X0F80,0X0000,
0X0000,0X01FC,0X7E00,0X0000,0X0001,0XC000,0X0000,0X003F,
0X1FC0,0X0000,0X0000,0X03FC,0XFF00,0X0000,0X0001,0XC000,
0X0000,0X007F,0X9FE0,0X0000,0X0000,0X03FF,0XFF80,0X0000,
0X0001,0XC000,0X0000,0X00FF,0XFFE0,0X0000,0X0000,0X03FD,
0XFFC0,0X0000,0X0001,0XC000,0X0000,0X01FF,0XDFE0,0X0000,
0X0000,0X07F8,0XFFE0,0X0000,0X0000,0X8000,0X0000,0X03FF,
0X8FF0,0X0000,0X0000,0X0FF0,0X7FF0,0X0000,0X0000,0X8000,
0X0000,0X07FF,0X07F0,0X0000,0X0000,0X0FE0,0X3FF8,0X0000,
0X0000,0X8000,0X0000,0X0FFE,0X03F8,0X0000,0X0000,0X1FC0,
0X1FF8,0X0000,0X0000,0X8000,0X0000,0X0FFC,0X01FC,0X0000,
0X0000,0X0F80,0X0FF8,0X0000,0X0000,0X0000,0X0000,0X0FF8,
0X00F8,0X0000,0X0000,0X1F00,0X07FC,0X0000,0X0000,0X0000,
0X0000,0X1FF0,0X007C,0X0000,0X0000,0X1E00,0X03FE,0X0000,
0X0000,0X8000,0X0000,0X3FE0,0X007C,0X0000,0X0000,0X3C00,
0X01FF,0X0000,0X0000,0X8000,0X0000,0X7FC0,0X003E,0X0000,
0X0000,0X3C00,0X00FF,0X0000,0X0000,0X8000,0X0000,0X7F80,
0X001E,0X0000,0X0000,0X7800,0X003F,0X8000,0X0000,0X8000,
0X0000,0XFE00,0X000F,0X0000,0X0000,0X3000,0X001F,0XC000,
0X0001,0XC000,0X0001,0XFC00,0X0006,0X0000,0X0000,0X6000,
0X000F,0XC000,0X0001,0XC000,0X0001,0XF800,0X0003,0X0000,
0X0000,0X4000,0X0001,0XC000,0X0003,0XE000,0X0001,0XC000,
0X0001,0X0000,0X0000,0XC000,0X0000,0XE000,0X0003,0XE000,
0X0003,0X8000,0X0001,0X0000,0X0000,0XC000,0X0000,0X3000,
0X0003,0XE000,0X0006,0X0000,0X0001,0X0000,0X0000,0X8000,
0X0000,0X1000,0X0003,0XE000,0X0004,0X0000,0X0000,0X8000,
0X0000,0X0000,0X0000,0X0000,0X0007,0XF000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X000F,0XF000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0300,
0X000F,0XF800,0X0060,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0180,0X001F,0XFC00,0X00C0,0X0000,0X0000,0X0000,
0X0002,0X0000,0X0000,0X00F8,0X001F,0XFC00,0X0F80,0X0000,
0X0000,0X2000,0X0006,0X0000,0X0000,0X00FC,0X001F,0XFC00,
0X1F80,0X0000,0X0000,0X3000,0X0004,0X0000,0X0000,0X00FF,
0XC01F,0XFC01,0XFF80,0X0000,0X0000,0X1000,0X000E,0X0000,
0X0000,0X00FF,0XE01F,0XFC03,0XFF80,0X0000,0X0000,0X3800,
0X001E,0X0000,0X0000,0X007F,0XFA1F,0XFC2F,0XFF00,0X0000,
0X0000,0X3800,0X001E,0X0000,0X0000,0X003F,0XFF3F,0XFE7F,
0XFE00,0X0000,0X0000,0X3C00,0X003C,0X0000,0X0000,0X003F,
0XFFFF,0XFFFF,0XFE00,0X0000,0X0000,0X3E00,0X003C,0X0000,
0X0000,0X001F,0XFFFF,0XFFFF,0XFC00,0X0000,0X0000,0X1E00,
0X003C,0X0000,0X0000,0X001F,0XFFFF,0XFFFF,0XFC00,0X0000,
0X0000,0X1E00,0X007C,0X0000,0X0000,0X001F,0XFFFF,0XFFFF,
0XFC00,0X0000,0X0000,0X1E00,0X007C,0X0000,0X0000,0X000F,
0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X1F00,0X00FC,0X0000,
0X0000,0X000F,0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X1F80,
0X00FC,0X0000,0X0000,0X0007,0XFFFF,0XFFFF,0XF000,0X0000,
0X0000,0X1F80,0X00F8,0X0000,0X0000,0X0007,0XFFFF,0XFFFF,
0XF000,0X0000,0X0000,0X0F80,0X00F8,0X0000,0X0000,0X0003,
0XFFFF,0XFFFF,0XE000,0X0000,0X0000,0X0F80,0X01F8,0X0000,
0X0000,0X0003,0XFFFF,0XFFFF,0XC000,0X0000,0X0000,0X1FC0,
0X01F8,0X0000,0X0000,0X0003,0XFFFF,0XFFFF,0XC000,0X0000,
0X0000,0X0FC0,0X00F8,0X0000,0X0000,0X0003,0XFFFF,0XFFFF,
0XE000,0X0000,0X0000,0X0F80,0X01F8,0X0000,0X0000,0X003F,
0XFFFF,0XFFFF,0XFE00,0X0000,0X0000,0X0F80,0X01F8,0X1C00,
0X0000,0X03FF,0XFFFF,0XFFFF,0XFFC0,0X0000,0X0000,0X0FC0,
0X01FF,0XFFF8,0X0000,0X07FF,0XFFFF,0XFFFF,0XFFF0,0X0000,
0X03FF,0XCFC0,0X01E7,0XFFFF,0X8000,0X5FFF,0XFFFF,0XFFFF,
0XFFFD,0X0000,0XBFFF,0XF3C0,0X01C7,0XFFFF,0XD001,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XC005,0XFFFF,0XF1C0,0X0007,0XFFFF,
0XFE0F,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XF83F,0XFFFF,0XF000,
0X0087,0XFFFF,0XFC07,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFC7F,
0XFFFF,0XF080,0X01C7,0XFFFF,0X0000,0X7FFF,0XFFFF,0XFFFF,
0XFFFF,0XC000,0XFFFF,0XF1C0,0X01E7,0XFFFE,0X0000,0X3FFF,
0XFFFF,0XFFFF,0XFFFE,0X0000,0X3FFF,0XF3C0,0X00F9,0X3E60,
0X0000,0X07FF,0XFFFF,0XFFFF,0XFFF0,0X0000,0X13FF,0XCF80,
0X01F8,0X0000,0X0000,0X00FF,0XFFFF,0XFFFF,0XFF80,0X0000,
0X0000,0X0F80,0X01F8,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,
0XFE00,0X0000,0X0000,0X0FC0,0X00F8,0X0000,0X0000,0X0003,
0XFFFF,0XFFFF,0XE000,0X0000,0X0000,0X0F80,0X00F8,0X0000,
0X0000,0X0001,0XFFFF,0XFFFF,0XC000,0X0000,0X0000,0X0F80,
0X01F8,0X0000,0X0000,0X0003,0XFFFF,0XFFFF,0XE000,0X0000,
0X0000,0X0FC0,0X01FC,0X0000,0X0000,0X0007,0XFFFF,0XFFFF,
0XF000,0X0000,0X0000,0X1FC0,0X00FC,0X0000,0X0000,0X0007,
0XFFFF,0XFFFF,0XF000,0X0000,0X0000,0X1F80,0X00FC,0X0000,
0X0000,0X0007,0XFFFF,0XFFFF,0XF000,0X0000,0X0000,0X1F80,
0X007C,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,0XF000,0X0000,
0X0000,0X1F00,0X007C,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,
0XF800,0X0000,0X0000,0X1F00,0X007C,0X0000,0X0000,0X001F,
0XFFFF,0XFFFF,0XFC00,0X0000,0X0000,0X1F00,0X003C,0X0000,
0X0000,0X001F,0XFFFF,0XFFFF,0XFC00,0X0000,0X0000,0X1E00,
0X003C,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,0XFE00,0X0000,
0X0000,0X1E00,0X001C,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,
0XFC00,0X0000,0X0000,0X1C00,0X001E,0X0000,0X0000,0X003F,
0XFE3F,0XFE3F,0XFE00,0X0000,0X0000,0X3C00,0X001C,0X0000,
0X0000,0X007F,0XF83F,0XFC0F,0XFF00,0X0000,0X0000,0X1C00,
0X000C,0X0000,0X0000,0X00FF,0XE01F,0XFC03,0XFF00,0X0000,
0X0000,0X1800,0X000C,0X0000,0X0000,0X00FF,0X001F,0XFC01,
0XFF80,0X0000,0X0000,0X1800,0X0004,0X0000,0X0000,0X00FE,
0X001F,0XFC00,0X3F80,0X0000,0X0000,0X3000,0X0002,0X0000,
0X0000,0X00F8,0X001F,0XFC00,0X0F80,0X0000,0X0000,0X2000,
0X0000,0X0000,0X0000,0X01C0,0X000F,0XF800,0X01C0,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0300,0X000F,0XF800,
0X0040,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X000F,0XF800,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0007,0XF000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X8000,0X0000,0X1000,0X0007,0XE000,0X0008,0X0000,
0X0000,0X8000,0X0000,0X8000,0X0000,0X3000,0X0003,0XE000,
0X0006,0X0000,0X0000,0X0000,0X0000,0X4000,0X0001,0XE000,
0X0003,0XE000,0X0003,0XC000,0X0001,0X0000,0X0000,0X6000,
0X0003,0XC000,0X0003,0XC000,0X0003,0XE000,0X0003,0X0000,
0X0000,0X7000,0X000F,0XC000,0X0003,0XC000,0X0001,0XF800,
0X0007,0X0000,0X0000,0X7800,0X001F,0X8000,0X0001,0XC000,
0X0000,0XFC00,0X000F,0X0000,0X0000,0X3C00,0X00FF,0X0000,
0X0001,0X8000,0X0000,0XFE00,0X001E,0X0000,0X0000,0X7C00,
0X01FF,0X0000,0X0000,0X8000,0X0000,0XFF00,0X001E,0X0000,
0X0000,0X3C00,0X03FF,0X0000,0X0000,0X8000,0X0000,0X7FE0,
0X001E,0X0000,0X0000,0X3E00,0X07FE,0X0000,0X0000,0X0000,
0X0000,0X3FF0,0X003C,0X0000,0X0000,0X1F00,0X0FFC,0X0000,
0X0000,0X0000,0X0000,0X1FF8,0X007C,0X0000,0X0000,0X0F80,
0X1FF8,0X0000,0X0000,0X0000,0X0000,0X1FFC,0X00F8,0X0000,
0X0000,0X0FC0,0X3FF8,0X0000,0X0000,0X0000,0X0000,0X0FFE,
0X01F8,0X0000,0X0000,0X0FE0,0X7FF0,0X0000,0X0000,0X8000,
0X0000,0X07FF,0X03F8,0X0000,0X0000,0X0FF0,0XFFE0,0X0000,
0X0000,0X8000,0X0000,0X03FF,0X87F8,0X0000,0X0000,0X07F9,
0XFFC0,0X0000,0X0000,0X8000,0X0000,0X01FF,0XCFF0,0X0000,
0X0000,0X07FF,0XFF80,0X0000,0X0000,0X8000,0X0000,0X00FF,
0X9FF0,0X0000,0X0000,0X03FF,0XFF00,0X0000,0X0001,0X8000,
0X0000,0X007F,0X9FE0,0X0000,0X0000,0X01FC,0XFE00,0X0000,
0X0001,0XC000,0X0000,0X003F,0X1FC0,0X0000,0X0000,0X00FC,
0X7C00,0X0000,0X0003,0XE000,0X0000,0X001E,0X1F80,0X0000,
0X0000,0X0078,0X3800,0X0000,0X0007,0XF000,0X0000,0X000C,
0X0F00,0X0000,0X0000,0X0038,0X1000,0X0000,0X0003,0XE000,
0X0000,0X0004,0X0E00,0X0000,0X0000,0X0008,0X7000,0X0000,
0X0003,0XE000,0X0000,0X0006,0X0800,0X0000,0X0000,0X0007,
0XF800,0X0000,0X0007,0XF000,0X0000,0X000F,0XE000,0X0000,
0X0000,0X0003,0XFC00,0X0000,0X0007,0XE000,0X0000,0X001F,
0XE000,0X0000,0X0000,0X0003,0XFE00,0X0000,0X0007,0XF000,
0X0000,0X003F,0XE000,0X0000,0X0000,0X0001,0XFF00,0X0000,
0X0007,0XF000,0X0000,0X007F,0XC000,0X0000,0X0000,0X0000,
0XFF80,0X0000,0X0007,0XF000,0X0000,0X00FF,0X8000,0X0000,
0X0000,0X0000,0X7FC0,0X0000,0X000F,0XF000,0X0000,0X01FF,
0X0000,0X0000,0X0000,0X0000,0X3FE0,0X0000,0X000F,0XF000,
0X0000,0X03FE,0X0000,0X0000,0X0000,0X0000,0X1FFC,0X0000,
0X000F,0XF000,0X0000,0X1FFC,0X0000,0X0000,0X0000,0X0000,
0X07FE,0X0000,0X000F,0XF000,0X0000,0X3FF0,0X0000,0X0000,
0X0000,0X0000,0X00FF,0X0000,0X000F,0XF000,0X0000,0X7F80,
0X0000,0X0000,0X0000,0X0000,0X003F,0X8000,0X000F,0XF000,
0X0000,0XFE00,0X0000,0X0000,0X0000,0X0000,0X000F,0XC000,
0X000F,0XF000,0X0001,0XF800,0X0000,0X0000,0X0000,0X0000,
0X0000,0XF000,0X0007,0XF000,0X0007,0X8000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0007,0XF000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X008C,0X0007,0XF000,
0X0880,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X007F,
0XF807,0XF00F,0XFF00,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X001F,0XFF94,0X14FF,0XFC00,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X000F,0XFFF8,0X0FFF,0XF800,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0001,0XFFFC,0X1FFF,
0XC000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0XFFFE,0X3FFF,0X8000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0FFF,0X7FF8,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X03FF,0X7FE0,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0012,0X2400,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
};

unsigned int f2[1600] = { /* 0X00,0X01,0XA0,0X00,0XA0,0X00, */
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X3FFE,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0003,0XFFFF,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X003F,0XFFFF,
0XE000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0001,
0X3077,0XFFFF,0X8FC0,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X017F,0XE000,0X7FFF,0X8FF0,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X1FFF,0X4000,0X00FE,0X0FFC,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X7FFF,0X0000,0X000E,
0X0FFC,0X0000,0X0000,0X0000,0X0000,0X0000,0X0003,0XFFFC,
0X0000,0X0000,0X07FF,0X8000,0X0000,0X0000,0X0000,0X0000,
0X0003,0XFFE0,0X0000,0X0003,0XC7FF,0X8000,0X0000,0X0000,
0X0000,0X0000,0X0007,0XFF00,0X0000,0X0007,0XF9FF,0XE000,
0X0000,0X0000,0X0000,0X0000,0X003F,0XFC00,0X0000,0X0007,
0XF81F,0XF000,0X0000,0X0000,0X0000,0X0000,0X00F0,0XE000,
0X0000,0X0007,0XF803,0XF800,0X0000,0X0000,0X0000,0X0000,
0X01F0,0X4000,0X0000,0X000F,0XF001,0XFC00,0X0000,0X0000,
0X0000,0X0000,0X07E0,0XC000,0X0000,0X000F,0XF000,0X3F00,
0X0000,0X0000,0X0000,0X0000,0X0FE3,0XE000,0X0000,0X000F,
0XF000,0X0300,0X0000,0X0000,0X0000,0X0000,0X1FF7,0XE000,
0X0000,0X000F,0XE000,0X0180,0X0000,0X0000,0X0000,0X0000,
0X7FDF,0XF000,0X0000,0X001F,0XE000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X7F8F,0XF800,0X0000,0X001F,0XE000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X7F0F,0XFC00,0X0000,0X001F,
0XE000,0X0000,0X0000,0X0000,0X0000,0X0000,0XFC07,0XFC00,
0X0000,0X001F,0XC000,0X0006,0X0000,0X0000,0X0000,0X0003,
0XF807,0XFE00,0X0000,0X003F,0XC000,0X0003,0X0000,0X0000,
0X0000,0X0003,0XF807,0XFE00,0X0000,0X001F,0XC000,0X0001,
0XC000,0X0000,0X0000,0X0003,0XE003,0XFE00,0X0000,0X001F,
0XC000,0X0001,0XE000,0X0000,0X0000,0X000F,0X8001,0XFF00,
0X0000,0X003F,0X8000,0X0001,0XF800,0X0000,0X0000,0X000F,
0X8001,0XFF80,0X0000,0X003F,0X0000,0X0000,0XFC00,0X0000,
0X0000,0X001C,0X0000,0XFF80,0X0000,0X003F,0X0000,0X0000,
0XFE00,0X0000,0X0000,0X0010,0X0000,0X7F80,0X0000,0X001E,
0X0000,0X0000,0X3F80,0X0000,0X0000,0X0030,0X0000,0X3F80,
0X0000,0X003E,0X0000,0X0000,0X1F80,0X0000,0X0000,0X0000,
0X0000,0X3FC0,0X0000,0X003C,0X0000,0X0000,0X1FC0,0X0000,
0X0000,0X0000,0X0000,0X1F80,0X0000,0X003C,0X0000,0X0000,
0X0FE0,0X0000,0X0000,0X0000,0X0000,0X0F80,0X0000,0X0018,
0X0000,0X0000,0X07F0,0X0000,0X0000,0X0100,0X0000,0X03C0,
0X0000,0X0038,0X0000,0X0000,0X03F8,0X0000,0X0000,0X0000,
0X0000,0X01E0,0X0000,0X0030,0X0000,0X0000,0X01F8,0X0000,
0X0000,0X0600,0X0000,0X00E0,0X0000,0X0030,0X0000,0X0000,
0X00FC,0X0000,0X0000,0X0C00,0X0000,0X00E0,0X0000,0X0000,
0X0000,0X0000,0X00FE,0X0000,0X0000,0X1C00,0X0000,0X0070,
0X0000,0X0000,0X0000,0X0000,0X007E,0X0000,0X0000,0X1800,
0X0000,0X0030,0X0000,0X0000,0X0000,0X0000,0X0044,0X0000,
0X0000,0X7800,0X0000,0X0010,0X0000,0X0000,0X0000,0X0000,
0X03C3,0X0000,0X0000,0XF800,0X0000,0X0000,0X0000,0X0040,
0X0000,0X0000,0X0FC3,0X8000,0X0000,0XF800,0X0000,0X0000,
0X0000,0X00C0,0X0000,0X0000,0X1FE3,0X8000,0X0003,0XF000,
0X0000,0X0004,0X0000,0X00C0,0X0000,0X0000,0X7FE3,0XC000,
0X0003,0XE000,0X0000,0X0002,0X0000,0X01C0,0X0000,0X0001,
0XFFFF,0XC000,0X0007,0XE000,0X0000,0X0002,0X0000,0X03C0,
0X0000,0X0003,0XFFF7,0XE000,0X0007,0XC000,0X0000,0X0003,
0X8000,0X07E0,0X0000,0X000F,0XFFE7,0XE000,0X001F,0XC000,
0X0000,0X0003,0XC000,0X07E0,0X0000,0X001F,0XFFC3,0XF000,
0X001F,0XC000,0X0000,0X0001,0XE000,0X0FE0,0X0000,0X003F,
0XFF01,0XF000,0X001F,0XC000,0X0000,0X0001,0XF000,0X1FE0,
0X0000,0X007F,0XFE01,0XF000,0X001F,0X8000,0X0000,0X0001,
0XFC00,0X3FE0,0X0000,0X00FF,0XF801,0XF800,0X003F,0X8000,
0X0000,0X0001,0XFC00,0X3FE0,0X0000,0X00FF,0XE001,0XF800,
0X003F,0X8000,0X0000,0X0001,0XFF80,0X7FE0,0X0000,0X03FF,
0X0001,0XF800,0X003F,0X8000,0X0000,0X0000,0XFF80,0XFFE0,
0X0000,0X07E4,0X0000,0XF800,0X003F,0X0000,0X0000,0X0001,
0XFFC1,0XFFF0,0X0000,0X0E00,0X0000,0XF800,0X003C,0XE000,
0X0000,0X0001,0XFFE1,0XFFF0,0X0000,0X1E00,0X0000,0X7800,
0X0020,0XFC00,0X0000,0X0000,0XFFFB,0XFFE0,0X0000,0X0000,
0X0000,0X3800,0X0020,0XFFC0,0X0000,0X0000,0XFFFF,0XFFF0,
0X0002,0X8000,0X0000,0X3C00,0X0061,0XFFFC,0X0000,0X0000,
0XFFFF,0XFFF0,0X003F,0X0000,0X0000,0X3C00,0X00E1,0XFFFC,
0X0000,0X0000,0XFFFF,0XFFF0,0X07FE,0X0000,0X0000,0X1C00,
0X00F9,0XFFFF,0XC000,0X0000,0X7FFF,0XFFF1,0XFFFC,0X0000,
0X0000,0X0C00,0X00FF,0XFFFF,0XC000,0X0000,0X7FFF,0XFFFF,
0XFFFC,0X0000,0X0000,0X0C00,0X01FC,0XFFFF,0XF800,0X0000,
0X7FFF,0XFFFF,0XFFF0,0X0000,0X0000,0X0C00,0X01FC,0X1FFF,
0XFC00,0X0000,0X7FFF,0XFFFF,0XFFE0,0X0000,0X0000,0X0400,
0X01FC,0X01FF,0XFF80,0X0003,0XFFFF,0XFFFF,0XFFE0,0X0000,
0X0000,0X0000,0X01F8,0X0019,0XFF80,0X13FF,0XFFFF,0XFFFF,
0XFFC0,0X0000,0X0000,0X0000,0X01F8,0X0000,0X0007,0XFFFF,
0XFFFF,0XFFFF,0XFF80,0X0000,0X0000,0X0000,0X01F8,0X0000,
0X0003,0XFFFF,0XFFFF,0XFFFF,0XFF00,0X0000,0X0000,0X0000,
0X01F8,0X0000,0X0001,0XFFFF,0XFFFF,0XFFFF,0XFE00,0X0000,
0X0000,0X0000,0X01F0,0X0000,0X0000,0X7FFF,0XFFFF,0XFFFF,
0XFC00,0X0000,0X0000,0X0000,0X01F0,0X0000,0X0000,0X7FFF,
0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X0200,0X01F0,0X0000,
0X0000,0X1FFF,0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X0200,
0X00E0,0X0000,0X0000,0X07FF,0XFFFF,0XFFFF,0XE000,0X0000,
0X0000,0X0200,0X01E0,0X0000,0X0000,0X03FF,0XFFFF,0XFFFF,
0XE000,0X0000,0X0000,0X0700,0X00F0,0X0000,0X0000,0X00FF,
0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X0700,0X00F0,0X0000,
0X0000,0X00FF,0XFFFF,0XFFFF,0XFC00,0X0000,0X0000,0X0700,
0X0060,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,0XFF00,0X0000,
0X0000,0X0700,0X00E0,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,
0XFF80,0X0000,0X0000,0X0700,0X0060,0X0000,0X0000,0X000F,
0XFFFF,0XFFFF,0XFFC0,0X0000,0X0000,0X0780,0X0060,0X0000,
0X0000,0X0007,0XFFFF,0XFFFF,0XFFF0,0X0000,0X0000,0X0780,
0X0040,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,0XFFF0,0X0000,
0X0000,0X0F80,0X0060,0X0000,0X0000,0X001F,0XFFFF,0XFFFF,
0XFFFE,0X0000,0X0000,0X0F80,0X0060,0X0000,0X0000,0X003F,
0XFFFF,0XFFFF,0XFFFF,0X0000,0X0000,0X0FC0,0X0020,0X0000,
0X0000,0X007F,0XFFFF,0XFFFF,0XFFFF,0X0000,0X0000,0X0F80,
0X0000,0X0000,0X0000,0X00FF,0XFFFF,0XFFFF,0XFFFF,0XE000,
0X0000,0X1FC0,0X0000,0X0000,0X0000,0X01FF,0XFFFF,0XFFFF,
0XFFFF,0XE000,0X0000,0X1FC0,0X0020,0X0000,0X0000,0X01FF,
0XFFFF,0XFFFF,0XFFE4,0X02FF,0XE400,0X1FC0,0X0000,0X0000,
0X0000,0X03FF,0XFFFF,0XFFFF,0XE400,0X00FF,0XFFC0,0X1F80,
0X0010,0X0000,0X0000,0X07FF,0XFFFF,0XFFFE,0X0000,0X007F,
0XFFFC,0X3F80,0X0030,0X0000,0X0000,0X0FFF,0XFFFF,0XFFFE,
0X0000,0X000F,0XFFFF,0XBF80,0X0010,0X0000,0X0000,0X0FFF,
0XFFFF,0XFFFE,0X0000,0X0007,0XFFFF,0X9F80,0X0018,0X0000,
0X0000,0X3FFF,0X87FF,0XFFFF,0X0000,0X0000,0XFFFF,0X8F00,
0X001C,0X0000,0X0000,0X3FF0,0X07FF,0XFFFF,0X0000,0X0000,
0X3FFF,0X8700,0X003C,0X0000,0X0000,0X7B00,0X0FFF,0XFFFF,
0X0000,0X0000,0X1FFF,0X8200,0X001C,0X0000,0X0001,0X0000,
0X07FF,0XFFFF,0X8000,0X0000,0X01FF,0X0000,0X001E,0X0000,
0X0000,0X0000,0X07FF,0XEFFF,0X8000,0X0000,0X001F,0X0000,
0X001E,0X0000,0X0010,0X0000,0X07FF,0XCFFF,0X8000,0X0000,
0X0003,0X9C00,0X001F,0X0000,0X00F0,0X0000,0X07FF,0X83FF,
0X8000,0X0000,0X0000,0XFE00,0X001F,0X8000,0X1FE0,0X0000,
0X07FF,0X01FF,0X8000,0X0000,0X0000,0XFC00,0X001F,0X8000,
0XFFC0,0X0000,0X07FF,0X00FF,0X8000,0X0000,0X0001,0XFC00,
0X000F,0X0003,0XFF00,0X0000,0X03FE,0X007F,0X8000,0X0000,
0X0001,0XFC00,0X001F,0X8017,0XFE00,0X0000,0X07FC,0X001F,
0XC000,0X0000,0X0001,0XFC00,0X000F,0X807F,0XFE00,0X0000,
0X07F8,0X001F,0XC000,0X0000,0X0001,0XF800,0X000F,0XC0FF,
0XFC00,0X0000,0X03F8,0X0007,0XC000,0X0000,0X0003,0XF800,
0X000F,0XC1FF,0XF800,0X0000,0X03F0,0X0003,0XC000,0X0000,
0X0003,0XF000,0X0007,0XE3FF,0XF000,0X0000,0X07E0,0X0001,
0XC000,0X0000,0X0003,0XF000,0X0003,0XFFFF,0XE000,0X0000,
0X03E0,0X0000,0XE000,0X0000,0X0003,0XE000,0X0003,0XEFFF,
0X8000,0X0000,0X03C0,0X0000,0X2000,0X0000,0X0007,0XC000,
0X0003,0XE7FF,0X0000,0X0000,0X0380,0X0000,0X2000,0X0000,
0X0007,0XC000,0X0003,0XC3FE,0X0000,0X0000,0X0300,0X0000,
0X0000,0X0000,0X000F,0X8000,0X0001,0XC3E0,0X0000,0X0000,
0X0300,0X0000,0X0000,0X0000,0X001F,0X0000,0X0000,0X41C0,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X001E,0X0000,
0X0000,0X3100,0X0000,0X0000,0X0000,0X0000,0X0400,0X0000,
0X001E,0X0000,0X0000,0X7F00,0X0000,0X0000,0X0000,0X0000,
0X0600,0X0000,0X0018,0X0000,0X0000,0X7F00,0X0000,0X0000,
0X0000,0X0000,0X0700,0X0000,0X0030,0X0000,0X0000,0X1F80,
0X0000,0X0000,0X0400,0X0000,0X0300,0X0000,0X0020,0X0000,
0X0000,0X1F80,0X0000,0X0000,0X0C00,0X0000,0X03E0,0X0000,
0X0040,0X0000,0X0000,0X0FC0,0X0000,0X0000,0X0C00,0X0000,
0X03F0,0X0000,0X0000,0X0000,0X0000,0X0FF0,0X0000,0X0000,
0X1C00,0X0000,0X03F0,0X0000,0X0000,0X0000,0X0000,0X07F8,
0X0000,0X0000,0X1C00,0X0000,0X03F8,0X0000,0X0000,0X0000,
0X0000,0X03F8,0X0000,0X0000,0X3C00,0X0000,0X03FC,0X0000,
0X0000,0X0000,0X0000,0X01FC,0X0000,0X0000,0X3C00,0X0000,
0X01FE,0X0000,0X0400,0X0000,0X0000,0X00FC,0X0000,0X0000,
0X7C00,0X0000,0X01FE,0X0000,0X0C00,0X0000,0X0000,0X007E,
0X0000,0X0000,0XFC00,0X0000,0X01FF,0X0000,0X3000,0X0000,
0X0000,0X001F,0X8000,0X0000,0XFC00,0X0000,0X00FF,0X0000,
0XF000,0X0000,0X0000,0X000F,0X0000,0X0001,0XFC00,0X0000,
0X00FF,0X8001,0XF000,0X0000,0X0000,0X0007,0X8000,0X0001,
0XF800,0X0000,0X007F,0XC007,0XF000,0X0000,0X0000,0X0003,
0X8000,0X0003,0XF800,0X0000,0X007F,0XC01F,0XC000,0X0000,
0X0000,0X0000,0XC000,0X0003,0XFC00,0X0000,0X003F,0XE01F,
0XC000,0X0000,0X0000,0X0000,0X2000,0X0003,0XFC00,0X0000,
0X003F,0XE03F,0X8000,0X0000,0X0000,0X0000,0X0000,0X0003,
0XF800,0X0000,0X001F,0XF07F,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0007,0XF800,0X0000,0X001F,0XF9FE,0X0000,0X0000,
0X0000,0X0000,0X0200,0X0007,0XF800,0X0000,0X000F,0XFBFE,
0X0000,0X0000,0X0000,0X0000,0X0100,0X0007,0XF800,0X0000,
0X0007,0XE7FC,0X0000,0X0000,0X0000,0X0000,0X00E0,0X0007,
0XF000,0X0000,0X0003,0XC7F0,0X0000,0X0000,0X0000,0X0000,
0X007E,0X000F,0XF000,0X0000,0X0003,0X07E0,0X0000,0X0000,
0X0000,0X0000,0X003E,0X000F,0XF000,0X0000,0X0001,0X07E0,
0X0000,0X0000,0X0000,0X0000,0X001F,0XC00F,0XF000,0X0000,
0X0007,0X8E00,0X0000,0X0000,0X0000,0X0000,0X000F,0XFC1F,
0XE000,0X0000,0X001F,0XF200,0X0000,0X0000,0X0000,0X0000,
0X0007,0XFFDF,0XE000,0X0000,0X01FF,0XF000,0X0000,0X0000,
0X0000,0X0000,0X0001,0XFFF3,0XE000,0X0000,0X07FF,0XE000,
0X0000,0X0000,0X0000,0X0000,0X0000,0XFFF0,0X6000,0X0000,
0X1FFF,0X8000,0X0000,0X0000,0X0000,0X0000,0X0000,0X7FF0,
0X7800,0X0000,0XFFFF,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X1FF0,0XFF80,0X0001,0XFFF8,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X1FF8,0XFFFB,0X0003,0XFF40,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X01FF,0XFFFF,0XF104,
0X6000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0003,
0XFFFF,0XFA00,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0001,0XFFFF,0XC000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X3FF2,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
};

unsigned int f3[1600] = { /* 0X00,0X01,0XA0,0X00,0XA0,0X00, */
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X07D4,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X7FFF,0XA000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0001,
0XFFFF,0XFE00,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X003D,0XFFFF,0XFE0A,0XFA80,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X03FC,0X7FFF,0XC007,0XFFF8,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X07F8,0X1FD0,0X0000,
0X7FFF,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X1FF8,
0X2000,0X0000,0X1FFF,0XC000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X3FF0,0XF000,0X0000,0X07FF,0XE000,0X0000,0X0000,
0X0000,0X0000,0X0000,0XFFEF,0XF000,0X0000,0X02FF,0XF000,
0X0000,0X0000,0X0000,0X0000,0X0001,0XFF0F,0XF000,0X0000,
0X003F,0XF800,0X0000,0X0000,0X0000,0X0000,0X0003,0XFE07,
0XF800,0X0000,0X000F,0XF000,0X0000,0X0000,0X0000,0X0000,
0X000F,0XF807,0XF800,0X0000,0X0003,0XC3E0,0X0000,0X0000,
0X0000,0X0000,0X000F,0XC007,0XFC00,0X0000,0X0001,0X01F0,
0X0000,0X0000,0X0000,0X0000,0X003F,0X0007,0XFC00,0X0000,
0X0000,0X03FC,0X0000,0X0000,0X0000,0X0000,0X0060,0X0007,
0XFC00,0X0000,0X0001,0XC3FC,0X0000,0X0000,0X0000,0X0000,
0X0040,0X0007,0XFC00,0X0000,0X0003,0XF3FF,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0003,0XFC00,0X0000,0X0007,0XFFFF,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0003,0XFC00,0X0000,
0X0007,0XFCFF,0X8000,0X0000,0X0000,0X0000,0X0000,0X0003,
0XFE00,0X0000,0X000F,0XF83F,0XC000,0X0000,0X0000,0X0000,
0X3800,0X0001,0XFE00,0X0000,0X000F,0XF01F,0XE000,0X0000,
0X0000,0X0000,0X6000,0X0001,0XFE00,0X0000,0X001F,0XF007,
0XE000,0X0000,0X0000,0X0000,0XC000,0X0000,0XFE00,0X0000,
0X001F,0XE003,0XF000,0X0000,0X0000,0X0003,0XC000,0X0000,
0XFE00,0X0000,0X003F,0XE001,0XF000,0X0000,0X0000,0X000F,
0X8000,0X0000,0XFE00,0X0000,0X007F,0XC000,0XF800,0X0000,
0X0000,0X003F,0X8000,0X0000,0X7E00,0X0000,0X007F,0XC000,
0X3C00,0X0000,0X0000,0X007F,0X0000,0X0000,0X3C00,0X0000,
0X007F,0X8000,0X0E00,0X0000,0X0000,0X00FE,0X0000,0X0000,
0X1E00,0X0000,0X007F,0X0000,0X0600,0X0000,0X0000,0X01FC,
0X0000,0X0000,0X1E00,0X0000,0X00FF,0X0000,0X0600,0X0000,
0X0000,0X03F8,0X0000,0X0000,0X0E00,0X0000,0X00FE,0X0000,
0X0100,0X0000,0X0000,0X07F8,0X0000,0X0000,0X0E00,0X0000,
0X01FC,0X0000,0X0000,0X0000,0X0000,0X07F0,0X0000,0X0000,
0X0E00,0X0000,0X00F8,0X0000,0X0000,0X0000,0X0000,0X0FF0,
0X0000,0X0000,0X0600,0X0000,0X01F0,0X0000,0X0000,0X0000,
0X0000,0X1FE0,0X0000,0X0000,0X0200,0X0000,0X01F0,0X0000,
0X0020,0X0000,0X0000,0X1FC0,0X0000,0X0000,0X0200,0X0000,
0X03C0,0X0000,0X0018,0X0000,0X0000,0X1FC0,0X0000,0X0000,
0X0000,0X0000,0X03C0,0X0000,0X0018,0X0000,0X0000,0X1F80,
0X0000,0X0000,0X0000,0X0000,0X0380,0X0000,0X000C,0X0000,
0X0000,0X20C0,0X0000,0X0000,0X0100,0X0000,0X0600,0X0000,
0X001E,0X0000,0X0000,0X20F0,0X0000,0X0000,0X0000,0X0000,
0X0600,0X0000,0X000F,0X0000,0X0000,0XE0F8,0X0000,0X0000,
0X0180,0X0000,0X0400,0X0000,0X000F,0X8000,0X0001,0XF1FE,
0X0000,0X0000,0X0080,0X0000,0X0000,0X0000,0X000F,0X8000,
0X0001,0XF1FF,0X8000,0X0000,0X01C0,0X0000,0X0000,0X0000,
0X0007,0XC000,0X0003,0XFBFF,0XE000,0X0000,0X01E0,0X0000,
0X1000,0X0000,0X0003,0XE000,0X0003,0XFFFF,0XF000,0X0000,
0X01F0,0X0000,0X3000,0X0000,0X0003,0XF000,0X0007,0XF9FF,
0XF800,0X0000,0X01F0,0X0000,0X6000,0X0000,0X0003,0XF000,
0X0007,0XF07F,0XFC00,0X0000,0X01F8,0X0000,0XF000,0X0000,
0X0001,0XF000,0X0007,0XE03F,0XFF00,0X0000,0X01FC,0X0003,
0XE000,0X0000,0X0001,0XFC00,0X000F,0XE00F,0XFF80,0X0000,
0X01FC,0X0007,0XE000,0X0000,0X0000,0XFC00,0X000F,0XC003,
0XFF80,0X0000,0X01FE,0X000F,0XE000,0X0000,0X0000,0XFC00,
0X000F,0XC000,0XFFE0,0X0000,0X03FF,0X003F,0XE000,0X0000,
0X0000,0X7C00,0X000F,0X8000,0X7FF0,0X0000,0X01FF,0X807F,
0XE000,0X0000,0X0000,0X7E00,0X000F,0X8000,0X07F0,0X0000,
0X03FF,0X80FF,0XE000,0X0000,0X0000,0X7F00,0X000F,0X0000,
0X007C,0X0000,0X03FF,0XC1FF,0XC000,0X0000,0X0000,0X3F00,
0X000E,0X0000,0X0006,0X0000,0X03FF,0XC7FF,0XC000,0X0000,
0X0001,0X6F00,0X000F,0X0000,0X0000,0X0000,0X03FF,0XE7FF,
0XC000,0X0000,0X001F,0XC200,0X000E,0X0000,0X0000,0X0000,
0X03FF,0XFFFF,0XC000,0X0000,0X00FF,0XC000,0X000E,0X0000,
0X0000,0X3E80,0X01FF,0XFFFF,0XC000,0X0000,0X0FFF,0XE300,
0X000C,0X0000,0X0000,0X1FFE,0X03FF,0XFFFF,0X8000,0X0000,
0X3FFF,0XE380,0X000C,0X0000,0X0000,0X1FFF,0XF3FF,0XFFFF,
0X8000,0X0000,0X7FFF,0XE780,0X0018,0X0000,0X0000,0X07FF,
0XFFFF,0XFFFF,0X8000,0X0001,0XFFFF,0XFF80,0X0008,0X0000,
0X0000,0X07FF,0XFFFF,0XFFFF,0X8000,0X0003,0XFFFF,0X8F80,
0X0018,0X0000,0X0000,0X03FF,0XFFFF,0XFFFF,0X0000,0X001F,
0XFFFF,0X8FC0,0X0008,0X0000,0X0000,0X01FF,0XFFFF,0XFFFF,
0X8000,0X003F,0XFFF8,0X0FC0,0X0000,0X0000,0X0000,0X00FF,
0XFFFF,0XFFFF,0XFFA8,0X00F0,0XBC80,0X07C0,0X0000,0X0000,
0X0000,0X007F,0XFFFF,0XFFFF,0XFFFF,0XFC00,0X0000,0X07C0,
0X0000,0X0000,0X0000,0X007F,0XFFFF,0XFFFF,0XFFFF,0XF000,
0X0000,0X07C0,0X0000,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,
0XFFFF,0XC000,0X0000,0X0FC0,0X0010,0X0000,0X0000,0X001F,
0XFFFF,0XFFFF,0XFFFF,0XC000,0X0000,0X0FC0,0X0010,0X0000,
0X0000,0X000F,0XFFFF,0XFFFF,0XFFFF,0X0000,0X0000,0X07C0,
0X0030,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,0XFFFC,0X0000,
0X0000,0X07C0,0X0010,0X0000,0X0000,0X0007,0XFFFF,0XFFFF,
0XFFF0,0X0000,0X0000,0X0780,0X0038,0X0000,0X0000,0X0003,
0XFFFF,0XFFFF,0XFFE0,0X0000,0X0000,0X0780,0X0038,0X0000,
0X0000,0X0003,0XFFFF,0XFFFF,0XFFC0,0X0000,0X0000,0X0380,
0X0078,0X0000,0X0000,0X000F,0XFFFF,0XFFFF,0XFF00,0X0000,
0X0000,0X0380,0X0038,0X0000,0X0000,0X003F,0XFFFF,0XFFFF,
0XFE00,0X0000,0X0000,0X0780,0X0078,0X0000,0X0000,0X00FF,
0XFFFF,0XFFFF,0XF800,0X0000,0X0000,0X0380,0X0078,0X0000,
0X0000,0X01FF,0XFFFF,0XFFFF,0XF000,0X0000,0X0000,0X0100,
0X007C,0X0000,0X0000,0X03FF,0XFFFF,0XFFFF,0XF800,0X0000,
0X0000,0X0180,0X00FC,0X0000,0X0000,0X0FFF,0XFFFF,0XFFFF,
0XFE00,0X0000,0X0000,0X0300,0X007C,0X0000,0X0000,0X0FFF,
0XFFFF,0XFFFF,0XFE00,0X0000,0X0000,0X0100,0X00FC,0X0000,
0X0000,0X3FFF,0XFFFF,0XFFFF,0XFF00,0X0000,0X0000,0X0000,
0X0078,0X0000,0X0000,0XFFFF,0XFFFF,0XFFFF,0XFF00,0X0000,
0X0000,0X0100,0X00FC,0X0000,0X0001,0XFFFF,0XFFFF,0XFFFF,
0XFFC0,0X0000,0X0000,0X0000,0X00FC,0X0000,0X4003,0XFFFF,
0XFFFF,0XFFFF,0XFFC0,0X0000,0X0000,0X0000,0X007C,0X001F,
0XE780,0X0AFF,0XFFFF,0XFFFF,0XFFE0,0X0000,0X0000,0X0200,
0X007C,0X07FF,0XFF80,0X0000,0X3FFF,0XFFFF,0XFFE0,0X0000,
0X0000,0X0200,0X007E,0X0FFF,0XFC00,0X0000,0X3FFF,0XFFFF,
0XFFF8,0X0000,0X0000,0X0000,0X007E,0XFFFF,0XF000,0X0000,
0X3FFF,0XFFFF,0XFFF8,0X0000,0X0000,0X0200,0X007D,0XFFFF,
0XC000,0X0000,0X7FFF,0XFFFF,0XFFFC,0X0000,0X0000,0X0600,
0X0078,0XFFFF,0XC000,0X0000,0X3FFF,0XFFFD,0XFFFC,0X0000,
0X0000,0X0600,0X0038,0XFFFF,0X0000,0X0000,0X7FFF,0XFFF0,
0X07FF,0X0000,0X0000,0X0E00,0X0030,0XFFF8,0X0000,0X0000,
0X7FFF,0XFFF0,0X003F,0X8000,0X0000,0X0E00,0X0000,0XFFC0,
0X0000,0X0000,0X7FFF,0XFFF0,0X0000,0X8000,0X0000,0X1E00,
0X0018,0X7F80,0X0000,0X0000,0XFFFE,0XFFF8,0X0000,0X0000,
0X0000,0X1E00,0X001E,0XC000,0X0000,0X0000,0X7FF8,0XFFF0,
0X0000,0X0E00,0X0000,0X1E00,0X001F,0X8000,0X0000,0X0000,
0XFFE0,0X7FF0,0X0000,0X07C0,0X0000,0X3E00,0X000F,0X8000,
0X0000,0X0000,0XFFE0,0X7FF0,0X0000,0X03FC,0X0000,0X3C00,
0X001F,0XC000,0X0000,0X0000,0XFF80,0X3FF0,0X0000,0X01FF,
0X0000,0X7E00,0X000F,0XC000,0X0000,0X0000,0XFF00,0X1FF0,
0X0000,0X00FF,0XF000,0X7C00,0X0007,0XC000,0X0000,0X0001,
0XFE00,0X0FF0,0X0000,0X007F,0XF800,0XFE00,0X0007,0XE000,
0X0000,0X0000,0XF800,0X0FE0,0X0000,0X003F,0XFE00,0XFC00,
0X0007,0XF000,0X0000,0X0001,0XF800,0X07F0,0X0000,0X000F,
0XFF81,0XFC00,0X0007,0XF000,0X0000,0X0001,0XF000,0X07E0,
0X0000,0X0007,0XFFE3,0XFC00,0X0001,0XF800,0X0000,0X0001,
0XC000,0X03F0,0X0000,0X0007,0XFFF3,0XF800,0X0001,0XF800,
0X0000,0X0001,0X8000,0X01E0,0X0000,0X0001,0XFFF7,0XF800,
0X0000,0XF800,0X0000,0X0001,0X0000,0X01F0,0X0000,0X0000,
0X7FF3,0XF000,0X0000,0XF800,0X0000,0X0000,0X0000,0X00E0,
0X0000,0X0000,0X3FF1,0XF000,0X0000,0X3C00,0X0000,0X0000,
0X0000,0X0060,0X0000,0X0000,0X0FE0,0XE000,0X0000,0X3E00,
0X0000,0X0008,0X0000,0X0020,0X0000,0X0000,0X03C0,0XE000,
0X0000,0X1C00,0X0000,0X000C,0X0000,0X0020,0X0000,0X0000,
0X00C0,0X8000,0X0000,0X1E00,0X0000,0X0018,0X0000,0X0000,
0X0000,0X0000,0X0040,0X0000,0X0000,0X0600,0X0000,0X0070,
0X0000,0X0000,0X0000,0X0000,0X007F,0X0000,0X0000,0X0600,
0X0000,0X0070,0X0000,0X0000,0X0000,0X0000,0X007F,0X0000,
0X0000,0X0300,0X0000,0X00F0,0X0000,0X0000,0X0000,0X0000,
0X00FF,0X0000,0X0000,0X0000,0X0000,0X01F0,0X0000,0X0008,
0X0000,0X0000,0X01FE,0X0000,0X0000,0X0080,0X0000,0X03E0,
0X0000,0X0008,0X0000,0X0000,0X01FC,0X0000,0X0000,0X0000,
0X0000,0X07E0,0X0000,0X000C,0X0000,0X0000,0X03FC,0X0000,
0X0000,0X0000,0X0000,0X0FE0,0X0000,0X000C,0X0000,0X0000,
0X03F8,0X0000,0X0000,0X0010,0X0000,0X1FE0,0X0000,0X000C,
0X0000,0X0000,0X07F8,0X0000,0X0000,0X0010,0X0000,0X1FC0,
0X0000,0X000F,0X0000,0X0000,0X07E0,0X0000,0X0000,0X000C,
0X0000,0X3FC0,0X0000,0X000F,0X8000,0X0000,0X0FC0,0X0000,
0X0000,0X000E,0X0000,0X7FC0,0X0000,0X001F,0X8000,0X0000,
0X3F80,0X0000,0X0000,0X0007,0X8000,0X7FC0,0X0000,0X001F,
0X8000,0X0000,0X3F00,0X0000,0X0000,0X0007,0XC000,0XFF80,
0X0000,0X000F,0XC000,0X0000,0X7C00,0X0000,0X0000,0X0003,
0XE000,0XFF00,0X0000,0X000F,0XE000,0X0000,0X7800,0X0000,
0X0000,0X0001,0XF001,0XFF00,0X0000,0X001F,0XE000,0X0000,
0XF000,0X0000,0X0000,0X0001,0XFC01,0XFE00,0X0000,0X000F,
0XE000,0X0001,0X8000,0X0000,0X0000,0X0000,0X7F03,0XFE00,
0X0000,0X000F,0XF000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X7FC7,0XFC00,0X0000,0X000F,0XF000,0X0006,0X0000,0X0000,
0X0000,0X0000,0X3FE7,0XFC00,0X0000,0X000F,0XF000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X3FFF,0XF800,0X0000,0X000F,
0XF000,0X0000,0X0000,0X0000,0X0000,0X0000,0X1FF9,0XF000,
0X0000,0X000F,0XF800,0X0040,0X0000,0X0000,0X0000,0X0000,
0X1FF8,0X7000,0X0000,0X0007,0XF800,0X0780,0X0000,0X0000,
0X0000,0X0000,0X07F8,0X2000,0X0000,0X0007,0XF800,0X1E00,
0X0000,0X0000,0X0000,0X0000,0X01F0,0X3000,0X0000,0X0007,
0XF801,0XFE00,0X0000,0X0000,0X0000,0X0000,0X0078,0XFC00,
0X0000,0X0007,0XFC07,0XFC00,0X0000,0X0000,0X0000,0X0000,
0X000F,0XFE00,0X0000,0X0003,0XFC0F,0XF000,0X0000,0X0000,
0X0000,0X0000,0X0003,0XFF80,0X0000,0X0003,0XFC3F,0XF000,
0X0000,0X0000,0X0000,0X0000,0X0003,0XFFE0,0X0000,0X0001,
0XFFFF,0XC000,0X0000,0X0000,0X0000,0X0000,0X0001,0XFFFA,
0X0000,0X0001,0X83FF,0X8000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X7FFF,0X0000,0X0003,0X03FF,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X1FFF,0XC000,0X07DF,0X87FC,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X01FF,0XF001,0XFFFF,
0XC7E0,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X000F,
0XBE17,0XFFFF,0XFEC0,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X000B,0XFFFF,0XE200,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0XFFFF,0X2000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X02B2,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
};



unsigned char gImage_busy_im[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,
0XAA,0XAA,0X54,0X00,0X00,0X05,0X6A,0XAA,0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,
0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X44,0X44,0X45,0XAA,0XAA,0XAA,0XAA,0X94,0X11,0X11,0X05,0XAA,0XAA,
0XAA,0XAA,0X95,0X04,0X44,0X15,0XAA,0XAA,0XAA,0XAA,0XA5,0X41,0X10,0X56,0XAA,0XAA,
0XAA,0XAA,0XA9,0X50,0X41,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0X54,0X05,0X6A,0XAA,0XAA,
0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X94,0X45,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X50,0X01,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X40,0X40,0X5A,0XAA,0XAA,
0XAA,0XAA,0XA5,0X00,0X10,0X16,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X04,0X44,0X05,0XAA,0XAA,0XAA,0XAA,0X94,0X11,0X11,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X44,0X44,0X45,0XAA,0XAA,0XAA,0XAA,0X95,0X11,0X11,0X15,0XAA,0XAA,
0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,0XAA,0XAA,0X54,0X00,0X00,0X05,0X6A,0XAA,
0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};


unsigned char gImage_no_im[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0X55,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA9,0X40,0X00,0X5A,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XA9,0X00,0X15,0X55,0X00,0X1A,0XAA,
0XAA,0XA4,0X00,0X6A,0XAA,0X50,0X06,0XAA,0XAA,0XA4,0X00,0X6A,0XAA,0XA4,0X06,0XAA,
0XAA,0X90,0X00,0X1A,0XAA,0XA9,0X01,0XAA,0XAA,0X90,0X10,0X06,0XAA,0XA9,0X01,0XAA,
0XAA,0X40,0X64,0X01,0XAA,0XAA,0X40,0X6A,0XAA,0X40,0X69,0X00,0X6A,0XAA,0X40,0X6A,
0XAA,0X40,0X6A,0X40,0X1A,0XAA,0X40,0X6A,0XAA,0X40,0X6A,0X90,0X06,0XAA,0X40,0X6A,
0XAA,0X40,0X6A,0XA4,0X01,0XAA,0X40,0X6A,0XAA,0X40,0X6A,0XA9,0X00,0X6A,0X40,0X6A,
0XAA,0X40,0X6A,0XAA,0X40,0X1A,0X40,0X6A,0XAA,0X90,0X1A,0XAA,0X90,0X05,0X01,0XAA,
0XAA,0X90,0X1A,0XAA,0XA4,0X00,0X01,0XAA,0XAA,0XA4,0X06,0XAA,0XA9,0X00,0X06,0XAA,
0XAA,0XA4,0X01,0X6A,0XAA,0X40,0X06,0XAA,0XAA,0XA9,0X00,0X15,0X55,0X00,0X1A,0XAA,
0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0XA9,0X40,0X00,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0X55,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};

unsigned char gImage_arrow_il[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X46,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X41,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X00,0X1A,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X06,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X00,0X01,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XAA,
0X40,0X00,0X15,0X55,0X5A,0XAA,0XAA,0XAA,0X40,0X10,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X64,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0X41,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,
0X46,0XA9,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0X5A,0XA9,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,
0X6A,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0X90,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X1A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA9,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};

unsigned char gImage_pen_il[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X96,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X91,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XA4,0X15,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,
0XA9,0X01,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X46,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X6A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XA4,0X05,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X01,0X46,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X6A,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA4,0X05,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X01,0X46,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X69,0XAA,0XAA,
0XAA,0XAA,0XAA,0XA4,0X01,0X14,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X44,0X1A,0XAA,
0XAA,0XAA,0XAA,0XAA,0X40,0X11,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X04,0X41,0XAA,
0XAA,0XAA,0XAA,0XAA,0XA4,0X01,0X10,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X44,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X11,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X04,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X01,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};


unsigned int pic_80x80[]={
/*
Image size:80 X 80 X 16BPP=12800 bytes.
Data format: 1 WORD(1 pixel)=RRRRRGGGGGGBBBBB
*/
0x6c51,0x7472,0x7492,0x7472,0x7451,0x7451,0x7472,0x7c92,
0x6c31,0x63cf,0x534d,0x4b2d,0x5b8f,0x7c72,0x9535,0x9d75,
0x9d95,0x9d95,0x9d75,0x9d95,0xa5d7,0xae18,0xa5d7,0x9575,
0x7c92,0x430c,0x2a29,0x32aa,0x430c,0x42ec,0x326a,0x19e7,
0x2248,0x2a69,0x538f,0x6c33,0x536f,0x4b6e,0x2207,0x2206,
0x3289,0x3a8b,0x3a8d,0x2a2c,0x1169,0x21e9,0x2227,0x4b8c,
0x4b6d,0x3b0c,0x19e8,0x0145,0x11c7,0x226a,0x2a6a,0x224a,
0x19e9,0x11c8,0x2a6b,0x4b8f,0x6411,0x74b3,0x8d35,0x9576,
0x9577,0x9db8,0xa5d8,0x9577,0x84d4,0x63f1,0x430d,0x324a,
0x2a2a,0x3aac,0x536f,0x4b4f,0x430e,0x3acd,0x2a4b,0x3acd,
0x6c31,0x7472,0x7cb2,0x7cb2,0x7492,0x7472,0x7492,0x7c92,
0x6c10,0x63cf,0x536e,0x534d,0x63cf,0x7c92,0x8d35,0x9555,
0x9d96,0x9d95,0x9d75,0x9d95,0xadf7,0xb638,0xae17,0x9db6,
0x7471,0x4b2c,0x2a49,0x326a,0x32ab,0x3aab,0x326a,0x2208,
0x19e7,0x3aeb,0x5bd0,0x7473,0x63f1,0x3acc,0x19a6,0x2a27,
0x3aca,0x326a,0x3a8d,0x21cb,0x1169,0x1188,0x3aca,0x434b,
0x4b6d,0x3b0c,0x19e8,0x0965,0x19e8,0x2a8a,0x2a8b,0x226a,
0x11c8,0x11a8,0x2a6b,0x538f,0x6431,0x7cb3,0x8d35,0x9576,
0x9db8,0x9db8,0x9db8,0x9556,0x84d4,0x6c11,0x430d,0x2a2a,
0x2a4b,0x3acc,0x4b4f,0x430e,0x42ed,0x3aac,0x2a4b,0x3acd,
0x7472,0x7c92,0x7cb3,0x84f4,0x8d14,0x8d14,0x84f4,0x84f4,
0x7472,0x7451,0x6c31,0x6c11,0x7c72,0x8cf4,0x9555,0x9d76,
0x9d75,0x9d75,0x9d75,0x9d95,0xa5d7,0xae18,0xa5f7,0x9db6,
0x7492,0x538e,0x42ec,0x32ab,0x328b,0x3acb,0x3acc,0x2a69,
0x32ca,0x32aa,0x6c52,0x6c33,0x6c32,0x5b8f,0x21e7,0x2a07,
0x3a8a,0x324a,0x21ca,0x2a0c,0x1149,0x1188,0x4b4d,0x53ad,
0x5bce,0x4b4d,0x2228,0x1186,0x1a08,0x2a6a,0x224a,0x1a09,
0x19c8,0x19e8,0x32ac,0x5bd0,0x6c52,0x7cb4,0x8d35,0x8d56,
0x9577,0x9577,0x9556,0x8d36,0x84d5,0x6c32,0x4b2e,0x2a4a,
0x2209,0x324b,0x3acd,0x3aac,0x3aac,0x328c,0x220a,0x326c,
0x7cb3,0x7472,0x7472,0x84d3,0x9555,0x9d96,0x9d96,0x9576,
0x8d14,0x8d15,0x9535,0x9555,0x9576,0x9d96,0x9d96,0x9d55,
0x9534,0x9554,0x9d75,0x9d75,0x9d75,0x9d75,0x9555,0x8d34,
0x7cd3,0x6c31,0x5baf,0x4b4d,0x430d,0x4b6e,0x536e,0x430c,
0x538e,0x4b4c,0x7473,0x7474,0x7433,0x534e,0x42cb,0x21c6,
0x42ca,0x63af,0x21ca,0x5b71,0x2a2c,0x3aac,0x4b4d,0x5bce,
0x6410,0x538e,0x326a,0x2208,0x2a6a,0x32cb,0x2a8b,0x2a4a,
0x2229,0x224a,0x430d,0x63f1,0x6c52,0x7cb3,0x84f4,0x84d4,
0x84d4,0x84d4,0x84d4,0x84f5,0x8d15,0x7c93,0x536f,0x328b,
0x1988,0x21e9,0x3a8c,0x3aac,0x42ed,0x42cd,0x2a2b,0x326b,
0x84d3,0x6c30,0x63cf,0x7451,0x8d14,0x9d96,0x9d96,0x9d76,
0x9d96,0xa5d7,0xb618,0xb639,0xae18,0xadd7,0x9d76,0x9535,
0x8d14,0x9555,0x9d96,0x9d76,0x8d14,0x84b2,0x7c72,0x7c92,
0x7cb3,0x7472,0x6c31,0x63f0,0x5bd0,0x6c31,0x6c52,0x5baf,
0x3289,0x7471,0x63f1,0x6bf2,0x7453,0x3a4b,0x4aec,0x2a08,
0xcedb,0xe7df,0x5b70,0x5b50,0x4ace,0x6bd1,0x6bf0,0x5bae,
0x6c30,0x5bcf,0x430c,0x3acb,0x4b4e,0x53af,0x4b8f,0x4b6e,
0x32ac,0x32ac,0x4b4e,0x6411,0x6c52,0x7472,0x7472,0x6c31,
0x6c11,0x7452,0x7c93,0x8cf5,0x8d36,0x84d4,0x5bb0,0x3aac,
0x21c8,0x2a4a,0x4b0e,0x536f,0x5bd0,0x5b90,0x3acd,0x3acd,
0x8cf4,0x6c30,0x5b8e,0x6c10,0x84d3,0x9555,0x9d76,0x9d76,
0xadd7,0xb659,0xc6bb,0xc6bb,0xbe7a,0xae18,0xa576,0x9535,
0x9514,0x9d55,0x9d96,0x9d75,0x84d3,0x7410,0x63f0,0x6c10,
0x7451,0x6c31,0x7452,0x7472,0x7452,0x7cb3,0x7cd4,0x7452,
0x8d34,0x5baf,0x5bb0,0x9557,0xe7bf,0x7c93,0x3208,0xd73c,
0xe7be,0xf7ff,0x94f5,0x6bb1,0xa578,0xd71e,0xdf5e,0x9555,
0x7c92,0x7451,0x5baf,0x538e,0x6411,0x6c52,0x6c32,0x6c32,
0x5bb0,0x538f,0x63f1,0x7473,0x7cb3,0x7cb3,0x7492,0x6bf0,
0x6bf1,0x7452,0x84b4,0x8d36,0x9556,0x84d4,0x5bb0,0x3aac,
0x324b,0x42cd,0x63d1,0x6c12,0x7473,0x6c32,0x4b2e,0x4b2e,
0x9535,0x7c92,0x7410,0x7c72,0x8cf4,0x9535,0x9555,0x9d76,
0xadd7,0xb639,0xc69a,0xc69a,0xbe39,0xadd7,0x9d76,0x9535,
0x9555,0x9d55,0x9d55,0x9535,0x7c92,0x6bf0,0x63af,0x63af,
0x6c11,0x7431,0x7cb3,0x84d4,0x7c93,0x7cb3,0x84f4,0x84d4,
0x84d3,0x6c31,0x84d4,0x530e,0xadf9,0xf7ff,0x8493,0xefbf,
0xefbf,0xffff,0xdf3d,0xd71d,0xf7ff,0xf7ff,0xdf5e,0x7c51,
0x9555,0x9514,0x84b3,0x7c72,0x84d4,0x7cd4,0x7cb4,0x84f4,
0x7cb4,0x7473,0x7cb3,0x8515,0x8d36,0x9556,0x8cf4,0x7452,
0x6c11,0x7c93,0x8d15,0x9d77,0x9557,0x84d4,0x63d0,0x42ed,
0x4aed,0x536f,0x6c32,0x7453,0x7c94,0x7453,0x536f,0x538f,
0x9555,0x84d3,0x8492,0x84d3,0x8d14,0x8cf4,0x8d14,0x9535,
0x9d76,0xadd8,0xb639,0xb618,0xadb7,0xa576,0x9d35,0x9514,
0x9d76,0x9555,0x9514,0x8cf4,0x7c72,0x6bf0,0x5b8f,0x5b8e,
0x7431,0x7c72,0x8cf5,0x8d15,0x7cb3,0x7c93,0x84d4,0x8d15,
0x9576,0x9d97,0x8493,0x63b0,0x8cd5,0xf7df,0xefdf,0xf7bf,
0xffff,0xe75c,0xd6fb,0xffff,0xe75d,0xdf3d,0xe73e,0xadd8,
0xadb7,0xa5b7,0x9556,0x9555,0x9d96,0x9d97,0x9577,0x9dd8,
0x8d56,0x84f4,0x84f5,0x9556,0x9d97,0x9db7,0x9556,0x7c93,
0x7432,0x84d4,0x9d56,0x9d97,0x9d77,0x84d4,0x6bd1,0x4b2e,
0x534f,0x63b0,0x7473,0x7c73,0x7cb4,0x7c73,0x63b0,0x63f1,
0x94f4,0x9535,0x9d55,0x9d75,0xa576,0xa596,0x9d55,0x8cd3,
0x7c72,0x8cf4,0xa576,0xa596,0x9d55,0x9514,0x94f4,0x9514,
0xa576,0xadd7,0xa597,0x9514,0x8492,0x6bf0,0x63af,0x6bf0,
0x7c72,0x8cd4,0x9556,0x9535,0x7c72,0x6bf0,0x84b3,0xa5d7,
0xa5d7,0xa597,0x8cd4,0x73d0,0xbe19,0xffff,0xef7e,0xf7bf,
0xf7be,0xfffe,0xfffe,0xf7bd,0xf7be,0xf7df,0xf7bf,0xe75e,
0xf7ff,0xadd7,0xb618,0xb659,0x9d76,0xadf9,0xb65a,0x9577,
0x8d36,0x8d15,0x8d15,0x9556,0xa5d8,0xa5d8,0x9535,0x7c72,
0x7411,0x8cd4,0x9d97,0xa5b8,0x9d77,0x84d4,0x6c11,0x5b8f,
0x5b6f,0x63f1,0x7c93,0x84f5,0x84d4,0x7473,0x6c32,0x6c32,
0xa596,0x8cd3,0x8492,0x9514,0xa596,0xa576,0x9514,0x8cf3,
0x8cd3,0x8cb3,0x8cb3,0x8492,0x8492,0x8cb3,0x94f4,0x9d35,
0xadd7,0xb639,0xb619,0xa597,0x9515,0x8472,0x7411,0x7c51,
0x94f4,0x8cd4,0x8cf4,0x9515,0x8493,0x7411,0x7c92,0x9d76,
0xa596,0xa576,0x7c31,0xbe19,0xf7ff,0xef7f,0xffff,0xffff,
0xffde,0xf7bc,0xffdc,0xfffc,0xfffd,0xffdd,0xffbe,0xffdf,
0xf7df,0xb5f8,0x9d55,0xadb7,0xb619,0xadf8,0xa5b8,0xb65a,
0x9d77,0x9556,0x8cf5,0x84d4,0x8d15,0x9535,0x8cf4,0x7c72,
0x7c52,0x8cf5,0xa597,0xadd8,0xa5b8,0x9536,0x7c73,0x6bf1,
0x6bd1,0x7432,0x84b4,0x8d36,0x8d36,0x84d5,0x7cb4,0x7cb4,
0x9514,0x9514,0x9d55,0xa576,0xa596,0xa576,0xa576,0xa576,
0xa576,0x9d35,0x94f4,0x94f4,0x9d35,0xad96,0xb5d8,0xb618,
0xbe18,0xc67a,0xbe59,0xb5f8,0xa596,0x9514,0x94d4,0x9514,
0x9514,0xadb7,0xa576,0x7c31,0x6baf,0x8452,0x9d56,0xadb7,
0xadd8,0xa576,0x94d4,0xb5f8,0xe75d,0xffbf,0xffdf,0xef3d,
0xffdd,0xffbb,0xffda,0xffda,0xffda,0xffdc,0xffbe,0xffdf,
0xffff,0x9d35,0x9d15,0xbe59,0xbe39,0xb5f8,0xadd8,0xadd8,
0xa5b8,0x9d76,0x84d4,0x7c73,0x7452,0x7c72,0x7c72,0x7c52,
0x8493,0x9515,0xa597,0xadf8,0xadf9,0xa5b7,0x9515,0x8493,
0x7c73,0x8494,0x8cf5,0x9d57,0x9d77,0x9d57,0x9536,0x9536,
0x8cd3,0x8cb3,0x94d3,0x9d55,0xa576,0x9d55,0x9514,0x9d14,
0xa556,0xa596,0xb5f8,0xce9b,0xe75d,0xf7df,0xffff,0xffff,
0xf7df,0xf7df,0xdf3d,0xc65a,0xb5b7,0x9d35,0x94d4,0x9d14,
0x9d15,0xa576,0x94f4,0x6baf,0x632d,0x7c10,0x94d4,0x9d14,
0xadb7,0xb5d8,0xad96,0x9d14,0xad55,0xe71c,0xffff,0xffde,
0xffbc,0xffdb,0xffd9,0xf7b7,0xf798,0xffda,0xffbd,0xf77d,
0xdedb,0x8431,0x8452,0xa576,0xa576,0xb619,0xbe5a,0xa5b8,
0xa5b8,0x9556,0x84b4,0x7c52,0x7432,0x7c52,0x7c52,0x7431,
0x8cd4,0x9515,0xa597,0xadd8,0xb619,0xb619,0xa5b8,0x9536,
0x9516,0x8d15,0x9536,0x9d77,0xa5b8,0xa5b8,0xa5b8,0xa5b8,
0x9d35,0x94f4,0x9d35,0xadb7,0xa555,0x8cb2,0x9d55,0xce9a,
0xef9e,0xefbf,0xf7df,0xffff,0xffff,0xf7df,0xef9e,0xe75d,
0xf7df,0xf7df,0xe75d,0xce9a,0xc639,0xb5d7,0xad96,0xb5b7,
0xb5d7,0x8451,0x7bf0,0xa555,0xc619,0xb5b7,0xad96,0xbe19,
0xbe19,0xd6dc,0xb5b6,0xc638,0xa4f3,0x8c51,0xff7e,0xffbe,
0xffbb,0xf798,0xf775,0xf795,0xffb6,0xff98,0xff9b,0xff9d,
0xffff,0xffdf,0xdedc,0xad97,0xad97,0xb5f8,0xa577,0xadb8,
0xa5b8,0x9536,0x84b3,0x7c73,0x8493,0x8cd4,0x8cf4,0x8cf4,
0x9d56,0xa597,0xadd8,0xb619,0xb619,0xb619,0xadf9,0xa5b8,
0xa5b8,0xa597,0x9d77,0xa598,0xa5d8,0xadf9,0xadf9,0xae19,
0xa555,0xadb6,0xadb7,0xa575,0xa576,0xbe59,0xe75d,0xffff,
0xefbf,0xefbf,0xf7df,0xf7df,0xf7df,0xf7df,0xffff,0xffff,
0xef9e,0xf7df,0xffdf,0xffdf,0xffff,0xffff,0xffff,0xffff,
0xe73d,0xbdf8,0xbdd7,0xef5d,0xffff,0xf7df,0xf79e,0xffff,
0xffff,0xffff,0xf7be,0xffff,0xef3c,0xd639,0xff9e,0xffde,
0xff9a,0xe6b4,0xde91,0xef52,0xf794,0xef35,0xf739,0xffde,
0xef5d,0xffff,0xffdf,0xbe19,0x9d15,0xa597,0xadb8,0xadb8,
0xadb8,0x9536,0x8cd4,0x8cd4,0x9515,0x9d76,0xa5b7,0xadd8,
0xadf8,0xbe39,0xbe5a,0xb639,0xadf9,0xadd8,0xadd8,0xadd8,
0xb619,0xadd8,0xa5b8,0xa5b8,0xadf9,0xadf9,0xae19,0xb63a,
0xa576,0x9d14,0x8cb3,0x9d14,0xcebb,0xffff,0xffff,0xe73d,
0xffff,0xf7ff,0xf7bf,0xf79f,0xef9e,0xf79f,0xf7bf,0xf7bf,
0xef5d,0xf79e,0xf79e,0xf7bf,0xffdf,0xf7bf,0xef7e,0xef7e,
0xffff,0xffff,0xffff,0xf7be,0xf77e,0xef7d,0xf79e,0xf7bf,
0xef7e,0xdedc,0xf7be,0xf77d,0xf79d,0xffff,0xf75d,0xf75c,
0xff59,0xe6b4,0xce4f,0xd68e,0xe6f1,0xf735,0xff59,0xffbd,
0xffff,0xf79e,0xffdf,0xd69b,0x94d4,0x9d15,0xa597,0x9515,
0xa598,0x9d56,0x9536,0x9d56,0xa576,0xa597,0xadb7,0xb5f8,
0xb5f8,0xbe5a,0xbe5a,0xb5f8,0x9d56,0x9d36,0x9d56,0xa597,
0xadb8,0xa598,0xa597,0xa5b8,0xa5b8,0xa5b8,0xa5b8,0xa5d8,
0x9d35,0x8cb2,0xad76,0xef7e,0xffff,0xe73d,0xe71d,0xffff,
0xf7bf,0xefbf,0xf7be,0xf7df,0xffff,0xffff,0xf7bf,0xf79f,
0xf7bf,0xffdf,0xff9f,0xff7e,0xff9f,0xff7f,0xf75e,0xf77e,
0xf75e,0xf75e,0xf77e,0xff9f,0xff7e,0xf77e,0xf77e,0xf79f,
0xf79f,0xffdf,0xf7be,0xfffe,0xf79d,0xef1c,0xf75d,0xff7c,
0xf75a,0xf758,0xdeb2,0xc5ce,0xd671,0xffd8,0xfffc,0xf77c,
0xffdf,0xe71c,0xb5d7,0x8c93,0x8cb3,0x94f4,0x94d4,0xadd8,
0x9d56,0x9d76,0xa597,0xadd8,0xa5b7,0x9d76,0x9d56,0x9d76,
0xa597,0xb5f8,0xb619,0xa597,0x8cd4,0x84b3,0x8cf5,0x9d56,
0x9536,0x9536,0x9d56,0x9d97,0x9d97,0x9d56,0x9556,0x9556,
0x9d55,0x94b2,0xff7f,0xff7f,0xef3e,0xffbf,0xff3f,0xeedc,
0xe77d,0xf7ff,0xef7d,0xffde,0xf7de,0xf7de,0xe6dc,0xff9f,
0xf7bf,0xff9f,0xff3f,0xfefe,0xfede,0xfebd,0xf6fd,0xf71e,
0xf75f,0xff7f,0xff5f,0xff1e,0xf6fe,0xf73f,0xf77f,0xf79f,
0xffbf,0xffbf,0xf79e,0xf79e,0xf77e,0xf77e,0xff7e,0xff9e,
0xf73c,0xffbd,0xffbc,0xe6f8,0xf7bb,0xfffd,0xe73a,0xffff,
0xf7be,0xdf1c,0xa555,0x8471,0x7410,0x8cb3,0x84b3,0xa5b7,
0xa596,0x9d76,0xa596,0xadd7,0xa5b7,0x9535,0x84b3,0x84b3,
0x9535,0x9d97,0xa5b7,0x9535,0x7c72,0x7c93,0x84d4,0x7c93,
0x84b3,0x84b3,0x8d15,0x9d76,0x9d97,0x9d76,0x9535,0x8d35,
0xa555,0xd6db,0xffdf,0xff7f,0xff7f,0xeefe,0xee3b,0xf6bc,
0xe77d,0xf7ff,0xf79d,0xffbe,0xefbd,0xef9e,0xe6dc,0xff5e,
0xf77f,0xf75e,0xfefe,0xfe7d,0xfe1c,0xf5fb,0xf67c,0xff1e,
0xff3f,0xff5f,0xff3f,0xfede,0xfebe,0xf6ff,0xf75f,0xef7f,
0xffbf,0xffdf,0xffbf,0xffbf,0xff9f,0xff9f,0xff7e,0xff7e,
0xd67a,0xef5d,0xffff,0xf79e,0xef7d,0xef7d,0xfffe,0xe77c,
0xffff,0xf7df,0xf7ff,0xd71c,0x9d34,0x8cd3,0x9d75,0x9514,
0x9d76,0x9d55,0x9d96,0xadd7,0xa5b7,0x8d14,0x7c93,0x7c92,
0x84d4,0x8d15,0x8d35,0x84b3,0x6c31,0x7472,0x84d3,0x7c92,
0x7472,0x7472,0x7cb3,0x84f4,0x8d35,0x8d15,0x8d14,0x8d14,
0xceba,0xffff,0xff5e,0xf6fe,0xf71e,0xeebd,0xee5c,0xf6bc,
0xef7d,0xefde,0xffbe,0xff9e,0xe73c,0xe73d,0xeedc,0xf6fe,
0xff7f,0xf6fe,0xfe9d,0xfe5c,0xfdfb,0xedda,0xee3b,0xf6dd,
0xff3f,0xff3f,0xfeff,0xfe9e,0xf67d,0xf6be,0xef1f,0xef3e,
0xf79f,0xff9e,0xffbf,0xffbf,0xff9f,0xff7f,0xf75e,0xf75e,
0xd65a,0xe71c,0xd679,0xffff,0xffff,0xfffe,0xfffe,0xf7fe,
0xefbe,0xe77e,0xffff,0xf7ff,0xef9e,0x9d55,0x9514,0xa5b7,
0x9d56,0x9535,0x9d76,0x9d97,0x9556,0x8cf4,0x7c93,0x7c93,
0x8cf4,0x8cf4,0x8cf4,0x7c93,0x7431,0x7c93,0x8cf4,0x84b3,
0x7452,0x7431,0x7452,0x84b3,0x8d14,0x8d15,0x8d15,0x9535,
0xf7bf,0xffdf,0xff3e,0xffbf,0xeefe,0xeede,0xf67c,0xe63b,
0xf77e,0xf7be,0xff9e,0xf75d,0xdedc,0xe6fc,0xf6fe,0xf6de,
0xff1e,0xee7c,0xee1b,0xfe3c,0xfe3c,0xf5fb,0xe61b,0xe65c,
0xff5f,0xff1f,0xfebe,0xf65d,0xee3d,0xee7d,0xeede,0xef1e,
0xf75e,0xff7e,0xff9f,0xff9f,0xff9f,0xff9f,0xff5e,0xf75e,
0xeefc,0xf75e,0xb555,0xbdd7,0xffdf,0xffff,0xe75c,0xf7de,
0xef9e,0xffff,0xe75d,0xad96,0xadb7,0xa555,0x9d76,0x9d56,
0x9d36,0x9535,0x9535,0x9536,0x9516,0x8cf5,0x8cf5,0x8d15,
0x9535,0x8d15,0x8d14,0x84b3,0x7c92,0x8cf4,0x9535,0x8cf4,
0x7c72,0x7451,0x7452,0x84b3,0x9535,0x9556,0x9556,0x9d76,
0xffff,0xf75e,0xff1e,0xff3f,0xcdfa,0xcdd9,0xd599,0xee9d,
0xff9f,0xf7bf,0xff9f,0xf6fc,0xe6bb,0xeedd,0xff1f,0xfebe,
0xf67c,0xee1b,0xedda,0xf5db,0xf5db,0xe5ba,0xe5fa,0xe65c,
0xff3f,0xfeff,0xf67d,0xee1c,0xee1c,0xee3d,0xeebe,0xf6ff,
0xff7f,0xff7f,0xff9f,0xff9f,0xff9f,0xff9f,0xff9f,0xff9f,
0xe6dc,0xf75e,0xdebb,0x7bcf,0xdeda,0xffff,0xffff,0xf7be,
0xf7ff,0xffff,0xdf3d,0xc659,0x8cd3,0xa576,0xbe5a,0x9d76,
0x9535,0x9515,0x9536,0x9d57,0x9d77,0x9d77,0x9d77,0x9d77,
0x9556,0x8d15,0x8d15,0x8cf4,0x84d4,0x9535,0x9d76,0x9535,
0x8cf4,0x7c93,0x7451,0x7c72,0x8cf4,0x9555,0x9d76,0x9d97,
0xff9e,0xff9f,0xf6fd,0xe65b,0xde3b,0xd5da,0xcd78,0xff7f,
0xff9f,0xff9f,0xff5e,0xee7b,0xeebc,0xf6bd,0xfede,0xfe7d,
0xedda,0xf5da,0xf5db,0xf5ba,0xe579,0xe599,0xee5c,0xff1f,
0xf6ff,0xf69d,0xee3d,0xee1c,0xedfc,0xf63d,0xf69e,0xfeff,
0xff3f,0xf75e,0xff5e,0xff5f,0xff5f,0xff5f,0xff7f,0xff7f,
0xeedd,0xf73d,0xff7e,0xc5d7,0x9cb2,0xd699,0xdefa,0xdefb,
0xf7ff,0xdf3c,0xadb6,0xceba,0xb5f8,0xa576,0xa596,0xadb7,
0x9515,0x9536,0x9d57,0xa5b8,0xae1a,0xb61a,0xadd9,0xa5b8,
0x9d97,0x9d76,0x9d97,0x9d97,0x9d97,0xadf8,0xae19,0xa5d8,
0xa5b7,0x8d14,0x7c72,0x7452,0x84d3,0x9555,0xa5b7,0xadf8,
0xeefc,0xff9f,0xff7f,0xff1e,0xf6fe,0xde1b,0xf67d,0xff3f,
0xff5e,0xff7f,0xfefd,0xe5f9,0xfebd,0xfebd,0xfe7d,0xf5db,
0xed79,0xf59a,0xf5ba,0xed79,0xe579,0xedfb,0xfebe,0xff3f,
0xee5c,0xe61c,0xedfc,0xf61c,0xf63d,0xfe5e,0xfe9e,0xfedf,
0xf6fe,0xf71e,0xf71e,0xf71e,0xf71e,0xf71e,0xf71e,0xf71e,
0xff7f,0xf73e,0xef1d,0xffdf,0x9471,0x8c71,0x8c50,0x9d13,
0xd6da,0xdf5c,0x8cd2,0x73ef,0x8cb2,0x9514,0x9d55,0x9514,
0x9d76,0x9d56,0x9d76,0xa5b8,0xb61a,0xb65b,0xae1a,0xa5b8,
0xa5b8,0xa5b7,0xadf8,0xb639,0xb639,0xb65a,0xbe5a,0xae19,
0xa5b7,0x9515,0x7c93,0x84b3,0x9535,0xa5b7,0xadf8,0xae19,
0xf6fc,0xff5e,0xff1e,0xddfa,0x9c13,0x93b1,0xff3f,0xff1f,
0xff1e,0xff5e,0xfebc,0xe598,0xfebd,0xfe9d,0xfdfb,0xed59,
0xed18,0xf518,0xed18,0xe538,0xedba,0xfe9d,0xfede,0xfe9d,
0xe5da,0xe5ba,0xedbb,0xf61d,0xfe5e,0xfe5e,0xfe7e,0xfebf,
0xfefe,0xf71e,0xf71e,0xf71e,0xf6fe,0xf6fe,0xf6de,0xeedd,
0xf71e,0xeefd,0xf75e,0xf73d,0xef5d,0x9cf3,0xb595,0xad74,
0x9d33,0xa594,0xa553,0x9d33,0x9d54,0x9d13,0xceda,0xadd7,
0xadd7,0xa576,0x9535,0x9d56,0xadd8,0xb63a,0xae19,0xa5b8,
0x9d77,0x9d96,0xadf8,0xb619,0xae18,0xae18,0xadf8,0x9d97,
0x9535,0x84d3,0x7c93,0x8cf4,0xa597,0xadf8,0xae18,0xadf8,
0xff3e,0xe67b,0xffbf,0xd5fa,0x38e6,0x2844,0xfedd,0xfefe,
0xfebc,0xff1d,0xbc32,0xed77,0xfe9c,0xfd98,0xdc95,0xdc34,
0xdc14,0xf4b7,0xdc96,0xed59,0xfe3c,0xfe7d,0xfebe,0xf63c,
0xeddb,0xfe1c,0xfe3d,0xfe1c,0xf5fc,0xfe3d,0xfe7f,0xfedf,
0xfeff,0xf6fe,0xf6de,0xf6fe,0xf6fe,0xf6fe,0xf6de,0xeebd,
0xe67c,0xf71e,0xf73e,0xff7e,0xffbf,0xe71b,0xbdd6,0xb595,
0xc657,0xad94,0xbe36,0xbe36,0xb616,0xced9,0xceb9,0xe77d,
0xc6ba,0xadf7,0x9d55,0x9d55,0xa597,0xa5b7,0xa5b7,0xa5d8,
0x9d96,0xadf8,0xae19,0xa5d8,0xa5b7,0xa5d8,0xa5d8,0xa5b7,
0x9535,0x8493,0x7c52,0x8cf4,0xa597,0xadf8,0xadf8,0xadf8,
0xd5d8,0xff9f,0xff5f,0xbd16,0x2023,0x2823,0x8b6f,0xff7f,
0xfebc,0xc432,0xd472,0xfe9b,0xfd97,0xdc12,0xd3d1,0xdbf3,
0xf475,0xec76,0xecd7,0xf599,0xf5fb,0xfe5c,0xfe7d,0xed9a,
0xf59a,0xfddb,0xfdfc,0xfdfc,0xf5dc,0xf5fc,0xfe1d,0xfe7e,
0xf69e,0xfeff,0xff1f,0xfedf,0xf6de,0xfede,0xfeff,0xf6de,
0xf6fe,0xf73e,0xf71d,0xef3d,0xffbf,0xffdf,0xdeda,0xbdd6,
0xc657,0xb5f5,0xc677,0xc677,0xc698,0xdf3a,0xd71a,0xe77c,
0xdf3b,0xbe58,0x9d75,0x9534,0x9534,0x9535,0x9555,0x9d96,
0x9d76,0xa5b7,0xa5d8,0x9d97,0x9d76,0x9d97,0xa5b7,0x9d97,
0x9556,0x84d4,0x8493,0x8d15,0x9d96,0x9d97,0x9d76,0x9d76,
0xddd8,0xff9f,0xff5e,0xde3a,0x2823,0x3064,0xa432,0xff7e,
0xe514,0xab0c,0xfd95,0xfdf7,0xdbcf,0xd36e,0xdb8f,0xdb90,
0xf434,0xe3d3,0xf4d7,0xfdb9,0xfdfa,0xfe7c,0xfe1b,0xed38,
0xf539,0xf539,0xf57a,0xf5bb,0xf5bb,0xedbb,0xf5dc,0xfe3d,
0xf67d,0xfeff,0xff1f,0xfedf,0xf69e,0xf6be,0xfedf,0xf6de,
0xff3f,0xff3f,0xf71e,0xef1d,0xf77e,0xffff,0xf7be,0xd699,
0xbe16,0xbe36,0xc677,0xc698,0xdf3a,0xe79c,0xe79c,0xe79c,
0xd6fa,0xbe58,0xadd6,0x9d75,0x9d55,0x9555,0x9d76,0xa5d7,
0xadf8,0xb619,0xadf8,0x9d97,0x9d56,0x9d76,0xa597,0xa597,
0x9d76,0x9515,0x8cf4,0x9515,0x9535,0x8cf4,0x84b3,0x84b3,
0xd557,0xe619,0xff9f,0xff7f,0x4106,0x1800,0xfedb,0xf639,
0xb32c,0xd3ef,0xfdf6,0xe40e,0xd30b,0xeb8d,0xe38e,0xe38f,
0xe391,0xdb92,0xf4f6,0xf599,0xfe5b,0xfe7c,0xed58,0xf518,
0xfd19,0xf4d8,0xecf8,0xed5a,0xe55a,0xdd39,0xed7a,0xf61d,
0xfebf,0xfedf,0xfeff,0xfeff,0xfebf,0xf69e,0xf6be,0xfedf,
0xfeff,0xf71e,0xff5f,0xf75e,0xef1d,0xf79e,0xffff,0xef7d,
0xbdf6,0xbe16,0xc657,0xc657,0xd71a,0xdf1b,0xdf5c,0xd6fa,
0xb637,0xb617,0xb617,0xb618,0xadf7,0xa5b6,0xa5b6,0xa5d7,
0xadf8,0xadf8,0xa597,0x9535,0x8d15,0x9535,0x9d76,0xa597,
0xa597,0x9d56,0x9515,0x9515,0x8cd4,0x7c72,0x7431,0x7411,
0xb453,0xc515,0xff9f,0xff5e,0x8b2e,0x3863,0xff5d,0xedd6,
0xaaca,0xf470,0xfd32,0xdb6b,0xeb4b,0xeb0b,0xeb4b,0xf38e,
0xe32f,0xec13,0xfd37,0xf5b8,0xfebc,0xfe3a,0xdc54,0xf4d7,
0xfcf8,0xf497,0xec97,0xe4f8,0xdd18,0xd4f8,0xe559,0xf5fc,
0xff1f,0xfebf,0xf67e,0xfedf,0xfeff,0xf69e,0xf69e,0xfeff,
0xfeff,0xf6de,0xff5f,0xff7f,0xef1d,0xef3d,0xffdf,0xffdf,
0xce79,0xad95,0xbe17,0xb5d6,0xbe37,0xb5f7,0xbe58,0xadd6,
0xadd6,0xb5f7,0xb638,0xbe59,0xb639,0xadd7,0x9d76,0x9555,
0x9535,0x8cf4,0x8493,0x7c52,0x7c52,0x84b3,0x9535,0x9d76,
0xa5b7,0x9d76,0x9535,0x8d15,0x84d4,0x7c72,0x7c52,0x7c52,
0xc4d5,0x8b4e,0xd5f9,0xff5e,0xd556,0xb431,0xff5c,0xf616,
0xbb0b,0xfc6f,0xfc8f,0xebab,0xf309,0xeac9,0xfb6b,0xdaca,
0xdaee,0xf434,0xfd37,0xfdf9,0xfe9b,0xed78,0xd413,0xf496,
0xf415,0xec15,0xe436,0xe4b7,0xe538,0xe579,0xf5db,0xfe5e,
0xfedf,0xee3d,0xe5dc,0xee3d,0xf69f,0xf67e,0xf69e,0xfeff,
0xff3f,0xf6de,0xf6fe,0xff7f,0xff7e,0xf77e,0xff9e,0xffbf,
0xdf1c,0x8c71,0xa555,0x9d34,0xa555,0xa576,0xb5f7,0xadb7,
0xadd7,0xadd7,0xb5f8,0xb619,0xb619,0xadd8,0x9d76,0x9535,
0x8cd4,0x8493,0x7411,0x6bd0,0x6bd0,0x7452,0x8cd4,0x9535,
0x9d97,0x9d76,0x9535,0x9515,0x8cf4,0x8cf4,0x8cf4,0x8cf4,
0xd537,0x5147,0x72ed,0xff7e,0xee3a,0xf65a,0xff5c,0xf616,
0xc30a,0xfc6f,0xfc4d,0xeb89,0xeaa7,0xfb08,0xfb29,0xd267,
0xe32e,0xf413,0xfd36,0xfe3a,0xedb8,0xd473,0xdc13,0xec14,
0xebb4,0xf3f5,0xf477,0xf519,0xf5bb,0xfe3c,0xfe7d,0xfe9f,
0xfe9f,0xee3e,0xddbc,0xddbb,0xe61d,0xf67e,0xfebf,0xfebf,
0xff3f,0xff1f,0xf6fe,0xf71e,0xff9f,0xffbf,0xf77e,0xf79e,
0xe73d,0x52ab,0x7c10,0x8431,0x94b3,0xa556,0xb5f8,0xbe19,
0xadd8,0xadb7,0xa597,0xadb7,0xadd8,0xadb8,0x9d76,0x9515,
0x8493,0x7431,0x63af,0x5b6e,0x638f,0x7411,0x84b3,0x9515,
0x9d76,0x9d56,0x9535,0x9535,0x9535,0x9535,0x9556,0x9535,
0xccd5,0x5967,0x4987,0xff7e,0xfefc,0xe5d7,0xfeb9,0xfeb8,
0xcb4a,0xfc8e,0xf3ea,0xe348,0xf2e7,0xfae8,0xe245,0xf32b,
0xf3b0,0xec13,0xfd57,0xfe5b,0xd494,0xb34f,0xdbf3,0xd351,
0xf435,0xfcb7,0xfd5a,0xfddb,0xfe5d,0xfe9d,0xfe7d,0xfe5d,
0xfebf,0xfebf,0xee3d,0xdd9b,0xe5dc,0xfebf,0xff1f,0xfedf,
0xf6de,0xff5f,0xff3f,0xeefd,0xff7f,0xff9f,0xf77e,0xf7bf,
0xe75e,0x2967,0x5b0d,0x634e,0x7411,0x9d15,0xadb7,0xb5f9,
0xadd8,0xa597,0x9d56,0x9d56,0xa577,0x9d76,0x8cf4,0x7c73,
0x63d0,0x536f,0x4aed,0x42ed,0x532e,0x6bf0,0x84b4,0x9535,
0x9d77,0x9d76,0x9556,0x9535,0x9555,0x9d56,0x9556,0x9535,
0xc472,0x4082,0x5187,0xff5e,0xfedc,0xee18,0xee16,0xfef8,
0xc389,0xfcce,0xeba8,0xdac5,0xfb89,0xda45,0xda26,0xe2eb,
0xe38f,0xec53,0xfd57,0xfe1a,0xaa8c,0xaa8c,0xc371,0xe475,
0xf558,0xfddb,0xfe3c,0xfe7d,0xfede,0xfebe,0xf63d,0xedfc,
0xeddb,0xfebf,0xfe9e,0xedfc,0xf65d,0xfedf,0xfede,0xfede,
0xff3f,0xff1e,0xf6fe,0xf71e,0xef1e,0xe6fd,0xef3e,0xf79f,
0xe77f,0x10e5,0x1946,0x4b0d,0x6bf1,0x6c31,0x9d97,0xb639,
0xadd8,0x9d36,0x9d56,0xa576,0x9d36,0x9535,0x7cb3,0x5b8f,
0x430e,0x328c,0x222b,0x2a4b,0x42ee,0x5bd1,0x7c73,0x8cf5,
0x9d77,0x9556,0x9556,0x8d15,0x9556,0x9556,0x7cb3,0x84f4,
0xf618,0xa38e,0x82ab,0xfedc,0xfefc,0xe5f7,0xfe78,0xfe77,
0xc389,0xec6b,0xfd0e,0xca63,0xc204,0xdaa8,0xeb0a,0xeb4d,
0xfc93,0xfcf5,0xfd98,0xe412,0xb2cd,0xdc13,0xecf6,0xf557,
0xfdfa,0xfe5c,0xfe5c,0xf63c,0xf65c,0xf65c,0xeddb,0xe5ba,
0xdd9a,0xfe7d,0xfe9e,0xfe7e,0xfebe,0xfede,0xfede,0xff3f,
0xff1f,0xff3f,0xff7f,0xff7f,0xf73e,0xeefd,0xe6fd,0xe73e,
0xffff,0x29a8,0x1967,0x2a09,0x5baf,0x7cb3,0x9556,0x9536,
0x9d77,0xa597,0xa597,0x9d56,0x8cd4,0x7c92,0x5b8e,0x2a6a,
0x11e9,0x09c9,0x11a9,0x19ea,0x2a4b,0x42ed,0x5bb1,0x7453,
0x84b4,0x84d4,0x8cf5,0x84d4,0x8514,0x84f4,0x6c51,0x7472,
0xfe39,0xf65a,0xc4b4,0xfedc,0xfedc,0xcd56,0xfefb,0xfe36,
0x9265,0xb2e6,0xfdb1,0xebeb,0xebcc,0xeb8c,0xe34c,0xdb2d,
0xd3d0,0xec94,0xfdb9,0xc350,0xd3f2,0xf517,0xf558,0xf599,
0xfe1b,0xfe3b,0xf63b,0xf63c,0xf63c,0xf63c,0xf5fb,0xedbb,
0xe59a,0xee1c,0xf65c,0xfebe,0xfeff,0xf6be,0xee7d,0xf6be,
0xf6bd,0xff1f,0xff7f,0xff7f,0xf73f,0xeefe,0xeefe,0xef3f,
0xe75f,0x1927,0x21a8,0x2a09,0x4b0d,0x63d0,0x7c93,0x9556,
0x9515,0xadb8,0xa598,0x8cf4,0x7c52,0x5baf,0x328a,0x1187,
0x0147,0x0188,0x11a9,0x19c9,0x19ea,0x2a2b,0x42ee,0x5bb0,
0x7453,0x7c93,0x8cf5,0x84d4,0x7cd3,0x7472,0x5baf,0x63f0,
0xdd56,0xfe9b,0xee3a,0xff1e,0xfedd,0xcd77,0xfefc,0xdd54,
0x71a4,0x79a3,0xe48e,0xdc0c,0xfcf1,0xdbce,0xdbcf,0xdbf1,
0xe474,0xed37,0xfd98,0xcbf2,0xfd99,0xfd99,0xf578,0xfe3b,
0xfe1b,0xf61b,0xf63b,0xf63b,0xedfb,0xedda,0xed9a,0xe559,
0xedfb,0xe5db,0xe5da,0xee5c,0xfeff,0xff1f,0xf6de,0xee7d,
0xeebd,0xeebd,0xf6fe,0xff3f,0xff1e,0xeedd,0xf6fe,0xff7f,
0xd69d,0x0884,0x31e9,0x428c,0x532e,0x6bf1,0x7453,0x7c73,
0x8cf5,0xadb8,0xa598,0x8cd4,0x7432,0x4b0d,0x19e8,0x0967,
0x11a9,0x11ea,0x1a0b,0x1a0a,0x19c9,0x19ca,0x328c,0x534f,
0x6c12,0x7c73,0x8d15,0x84d4,0x7cb3,0x6c52,0x538f,0x5bcf,
0xed98,0xe598,0xf65b,0xfefe,0xfefe,0xee3b,0xfefd,0xbcb2,
0x6985,0x3820,0xc3ed,0xb34b,0xcbef,0xecf4,0xfdf9,0xfe3b,
0xfebd,0xfe9d,0xcc74,0xc413,0xfdda,0xfdb9,0xfdda,0xfe5c,
0xfe7c,0xf61b,0xf63b,0xf61b,0xe599,0xe559,0xed59,0xdd18,
0xe59a,0xdd79,0xdd79,0xddba,0xee5c,0xff1f,0xff1f,0xe67c,
0xeebd,0xde5b,0xe67c,0xff3f,0xff3f,0xee9d,0xeebd,0xff5f,
0xd67c,0x18c6,0x3a2a,0x322a,0x29e9,0x534e,0x7c94,0x7c73,
0x8cd5,0x9d57,0x9d57,0x8cf5,0x7432,0x3aab,0x0967,0x0987,
0x11e9,0x19ea,0x19ea,0x19c9,0x1189,0x19a9,0x2a4b,0x42ed,
0x63d1,0x7473,0x8d15,0x7cd4,0x7c93,0x6c52,0x5baf,0x5bd0,
0xed98,0xd4d5,0xf63b,0xfe9d,0xfe7d,0xfe9d,0xff1e,0xe5b8,
0x82aa,0x3000,0xfe58,0xff3c,0xfe18,0xfe7b,0xfe1a,0xedb9,
0xfe5c,0xff3f,0xd4f6,0xe558,0xfe1b,0xfe1b,0xfe3c,0xf63b,
0xfe7c,0xedfa,0xf63b,0xfe3c,0xedba,0xfdda,0xfe3c,0xfe1c,
0xfdfb,0xf61c,0xfe3c,0xee1b,0xe5fb,0xee7c,0xf6bd,0xe63b,
0xde3b,0xd5fa,0xde5c,0xff3f,0xff7f,0xf6de,0xf6de,0xff7f,
0xa4b5,0x0002,0x2127,0x4a8c,0x532e,0x530e,0x6390,0x532e,
0x7412,0x7412,0x7432,0x7453,0x5b90,0x222a,0x0967,0x11a8,
0x11c9,0x11c9,0x11c9,0x11c9,0x11a9,0x11a9,0x220a,0x328c,
0x63f1,0x7c94,0x8d36,0x84f4,0x7c93,0x6c52,0x538f,0x5baf,
0xdcd6,0xd4b5,0xedb9,0xf61b,0xf63c,0xfe5c,0xfefe,0xfefd,
0xff9e,0xbc92,0xfebb,0xff3d,0xedf9,0xd537,0xe5da,0xfe7d,
0xe5fb,0xfeff,0xfe7d,0xfeff,0xfede,0xfe9e,0xfebd,0xfe9d,
0xf63b,0xe5b9,0xf61b,0xfe7c,0xf5ba,0xfdba,0xfe1c,0xfddb,
0xfdbb,0xfddb,0xfe7e,0xfebe,0xfe7d,0xfe9d,0xfefe,0xf6bd,
0xde3b,0xe65b,0xeebd,0xff3f,0xff5f,0xff3f,0xff3f,0xff9f,
0xeefe,0x62ce,0x2127,0x1906,0x530e,0x5b6f,0x7432,0x7453,
0x6bf1,0x5b70,0x5b4f,0x4b2e,0x326b,0x11a8,0x09a8,0x11c9,
0x1a0a,0x220b,0x222b,0x224b,0x220a,0x19c9,0x21e9,0x222a,
0x536f,0x6c32,0x84f5,0x7cb4,0x7493,0x7473,0x5bb0,0x5baf,
0xdc94,0xdcd6,0xdcb6,0xed78,0xfe7d,0xfe7d,0xfe5c,0xfe7b,
0xedf9,0xfefc,0xd536,0xf63a,0xff3f,0xd578,0xee5c,0xddba,
0xff5f,0xee9d,0xee5c,0xf69d,0xf67d,0xf67d,0xfe9d,0xfebe,
0xf65c,0xe5b9,0xf63b,0xfe3b,0xe517,0xdc75,0xdc34,0xcb72,
0xcb93,0xc372,0xe496,0xfe1c,0xfe9e,0xfede,0xff3f,0xff1f,
0xfedd,0xff5f,0xff7f,0xf71e,0xeedd,0xf71e,0xff5f,0xff5f,
0xf71f,0xffdf,0xdebd,0x5aed,0x5b2f,0x6390,0x8494,0x8cd5,
0x8cf6,0x7412,0x5b4f,0x3a8c,0x1188,0x0967,0x11e9,0x11e9,
0x2a6c,0x328d,0x32cd,0x32cd,0x2a6c,0x19ea,0x11a8,0x11a8,
0x220a,0x430e,0x6432,0x6c52,0x7493,0x7cb4,0x6411,0x6411,
0xecf6,0xcbf2,0xdc95,0xf579,0xfdba,0xfe1c,0xfe1b,0xfdfa,
0xfdfa,0xedb9,0xdd77,0xdd98,0xee1b,0xfe7d,0xf63d,0xdddb,
0xf6de,0xf6de,0xe63c,0xf69e,0xf67d,0xfebe,0xee3b,0xf67c,
0xfe9d,0xfebd,0xf61b,0xed79,0xf538,0xe495,0xdbd3,0xe3f4,
0xec15,0xe3d4,0xe3f4,0xec96,0xfd59,0xf559,0xed99,0xfe5c,
0xfebd,0xee7c,0xe63b,0xe67c,0xeefe,0xf75f,0xf71e,0xe6dd,
0xf73f,0xef1f,0xb578,0x73d1,0x4a8c,0x6bb0,0x8cf5,0xa598,
0x9537,0x8cd5,0x6bd2,0x3a6c,0x1168,0x0147,0x09c9,0x1a2a,
0x32ad,0x3aee,0x4b4f,0x432f,0x32ac,0x220a,0x19c9,0x19c9,
0x220a,0x328c,0x53b0,0x7494,0x7cd5,0x7493,0x6c52,0x6c52,
0xfe1a,0xd433,0xd413,0xecd6,0xfd38,0xfd79,0xfd79,0xfdba,
0xed38,0xed98,0xe558,0xd4f6,0xdd38,0xf5fb,0xfe3d,0xe5db,
0xee9e,0xeebe,0xee9e,0xf69e,0xee5d,0xfe9d,0xf67d,0xfe7d,
0xf63b,0xfe7c,0xf5fa,0xe517,0xecf7,0xf496,0xec14,0xf414,
0xfc15,0xfbf5,0xebd4,0xebf4,0xe414,0xd3b3,0xcbf3,0xdcf7,
0xfe3b,0xf63b,0xe63b,0xe67b,0xeedd,0xef1d,0xef1d,0xeefd,
0xeedd,0xce1a,0xc5fa,0x2106,0x31e9,0x8494,0x8cb4,0xadf9,
0xa598,0x8cf6,0x6391,0x29ea,0x0947,0x0988,0x11ea,0x1a0a,
0x2a6c,0x3aee,0x4b70,0x4b4f,0x32ad,0x19ea,0x1188,0x0968,
0x11c9,0x222a,0x432e,0x6432,0x74b4,0x7494,0x7494,0x7cd4,
0xfe3a,0xecf6,0xe494,0xd3f2,0xdbf3,0xfd18,0xfd59,0xf4d7,
0xecf7,0xed38,0xe538,0xd4b6,0xd4b6,0xed59,0xfdfc,0xfe3d,
0xf67d,0xee9e,0xf6df,0xf69e,0xf65d,0xf65c,0xfe9e,0xfe7c,
0xfe5c,0xfe9d,0xfdda,0xecf7,0xecb6,0xfcb6,0xf435,0xf3d4,
0xf393,0xfbb4,0xf393,0xeb72,0xdb51,0xcaf0,0xcb71,0xe475,
0xf599,0xf5fa,0xfe7c,0xfedd,0xff1e,0xf73e,0xf73e,0xf73e,
0xd61a,0xffdf,0xffdf,0xce5a,0x52ac,0x6bb0,0x8cb4,0x9d36,
0x9d57,0x7c74,0x4ace,0x1988,0x0927,0x11a9,0x120a,0x11c9,
0x2a6c,0x3aee,0x4b70,0x4b2f,0x32ac,0x1a0a,0x11a9,0x11a8,
0x0988,0x11c9,0x328c,0x5390,0x6c53,0x7494,0x7cb4,0x84f5,
0xfdd9,0xf537,0xf4f6,0xd3d2,0xcb50,0xf475,0xfcd7,0xf476,
0xfd18,0xecb7,0xe4d7,0xed18,0xe4d8,0xdc97,0xf519,0xfe3d,
0xfe9e,0xe63c,0xf69e,0xf67d,0xfe9e,0xee1c,0xfe7d,0xf63c,
0xfe9d,0xfe3b,0xf599,0xecf6,0xf4b6,0xfc96,0xfc14,0xf3b3,
0xeb11,0xf332,0xf332,0xeb31,0xeb31,0xe351,0xebd3,0xfcd7,
0xf4d6,0xf578,0xfe3b,0xfedd,0xff3e,0xff3e,0xff5e,0xff5e,
0xf73e,0xffbf,0xf77f,0xffff,0xbe3a,0x5b0d,0x9516,0x8494,
0x7c32,0x5b2f,0x320a,0x1988,0x1989,0x19c9,0x120a,0x11ea,
0x220a,0x2a2b,0x2a4b,0x220a,0x11c9,0x1188,0x0988,0x0988,
0x11a8,0x11c9,0x222a,0x430e,0x5bd1,0x6432,0x6c53,0x7473,
0xfe5c,0xdc95,0xdc34,0xe434,0xdb92,0xdb72,0xebf4,0xfcb7,
0xfcd8,0xec97,0xec97,0xf4f9,0xecb8,0xdc15,0xe435,0xf519,
0xfebf,0xddfb,0xee1c,0xf63d,0xfebe,0xedfb,0xf61b,0xf61b,
0xfe7c,0xed78,0xe4d6,0xecd6,0xfcb6,0xfc35,0xf3b3,0xf372,
0xeb31,0xf331,0xeaf0,0xeaf0,0xf352,0xfb92,0xfbd3,0xfc55,
0xec14,0xf496,0xfd58,0xfdfa,0xfe9c,0xfedd,0xff1e,0xff3e,
0xffdf,0xf73d,0xef1d,0xe73d,0xffff,0xadb7,0x5b2e,0x7c32,
0x532e,0x3a2b,0x21a9,0x21c9,0x21ea,0x19ca,0x1a0b,0x2a8c,
0x222b,0x220b,0x19ea,0x11a9,0x11a9,0x19c9,0x11c9,0x09a8,
0x11c9,0x11c9,0x19e9,0x2a8c,0x432f,0x5390,0x5bb1,0x5bb1,
0xfede,0xd495,0xcbd2,0xd3b2,0xd371,0xe392,0xebd4,0xfc56,
0xf477,0xfcb8,0xfcf9,0xf4b8,0xec77,0xec36,0xebf5,0xd3f4,
0xfebf,0xee1c,0xdd7a,0xe5ba,0xfe7e,0xf61c,0xedda,0xfe1b,
0xfe7d,0xe537,0xdc95,0xf4d6,0xfc95,0xf3d3,0xeb72,0xeb31,
0xf371,0xf371,0xeb10,0xeaf0,0xfb72,0xfb93,0xfb72,0xf372,
0xeb92,0xebf3,0xec54,0xecf6,0xf599,0xfe5b,0xfedd,0xff1d,
0xff3d,0xffbf,0xffff,0xffff,0xefbf,0xefff,0x6bd0,0x530d,
0x426b,0x29c9,0x2188,0x21c9,0x21ea,0x19ca,0x2a6c,0x432f,
0x430e,0x42ee,0x3aad,0x328c,0x32cd,0x3aee,0x2aac,0x1a2a,
0x11e9,0x11c9,0x11c9,0x222a,0x32ad,0x432e,0x4b4f,0x4b4f,
0xfe7d,0xd4d6,0xcbf3,0xc371,0xcb31,0xebf4,0xf415,0xf415,
0xfc78,0xfcf9,0xfd1a,0xfcb9,0xf477,0xf436,0xebd5,0xd393,
0xfe7e,0xfe7e,0xd539,0xd4f8,0xf5fc,0xfe5d,0xf5da,0xfe7d,
0xfdda,0xdcb5,0xdc54,0xec75,0xebd3,0xe372,0xf352,0xf351,
0xf392,0xfbd2,0xf371,0xf351,0xfb92,0xfb93,0xfb31,0xf310,
0xe2f0,0xe351,0xebd3,0xec75,0xed17,0xf5b9,0xfe3b,0xfebc,
0xff3d,0xeefc,0xff9e,0xef5d,0xefbf,0xefdf,0xe79f,0x426a,
0x3a2a,0x320a,0x320a,0x320b,0x2a0b,0x2a6c,0x3b0f,0x53b1,
0x5bb1,0x5bb1,0x5390,0x4b70,0x5390,0x53b1,0x434f,0x32ad,
0x1a2a,0x1a0a,0x11e9,0x1a0a,0x328c,0x430e,0x4b4f,0x4b4f,
0xfe1c,0xcc55,0xcbf3,0xd3f4,0xd393,0xdb93,0xebd5,0xfc98,
0xfcb9,0xfcb9,0xfcd9,0xfcfa,0xfc78,0xf3d5,0xeb73,0xe3b4,
0xfe1d,0xfedf,0xd4f8,0xc476,0xe579,0xfe9d,0xfdfb,0xfe9d,
0xdc54,0xd392,0xd392,0xe392,0xd310,0xe330,0xfbb2,0xfbb2,
0xfbb2,0xfc33,0xfc13,0xfbb2,0xfb92,0xfb71,0xfaef,0xf2af,
0xda0c,0xe2af,0xf392,0xf454,0xf4d6,0xf537,0xed98,0xedf9,
0xeedc,0xf77d,0xe6fc,0xffff,0xefbe,0xf7ff,0xe77e,0x532d,
0x3209,0x42ac,0x4acd,0x42ac,0x3aad,0x4b4f,0x5bf1,0x6412,
0x7494,0x7c94,0x7473,0x6c32,0x6c32,0x6c73,0x6432,0x53b0,
0x2a8b,0x224b,0x1a0a,0x222a,0x328c,0x430e,0x4b6f,0x5390,
0xf579,0xdc96,0xd3f4,0xdc15,0xec56,0xec36,0xec36,0xf457,
0xf457,0xfc78,0xfc98,0xfc37,0xf3b5,0xfbb5,0xf333,0xca90,
0xecf9,0xfedf,0xe4f9,0xcc35,0xd4b6,0xfe7c,0xfefd,0xfdb9,
0xba6d,0xd28e,0xa8e7,0xb9aa,0xeb91,0xf3d2,0xfbf4,0xfb71,
0xfbd3,0xfb91,0xfb70,0xfb91,0xfb91,0xfb2f,0xf28d,0xea0b,
0xea0b,0xf28d,0xf330,0xfbf3,0xf474,0xe4b5,0xed78,0xfe9c,
0xe69b,0xf77e,0xe71c,0xffff,0xefbf,0xf7ff,0xefbf,0x426a,
0x3a6a,0x532d,0x532e,0x530e,0x6390,0x6c11,0x6c32,0x7cb4,
0x7c93,0x84d4,0x84d4,0x7cb3,0x7cb3,0x7cb3,0x7472,0x6410,
0x4b4e,0x2a6a,0x2a6b,0x32ab,0x222a,0x32ac,0x53af,0x53af,
0xe4d7,0xd455,0xd414,0xdc35,0xe436,0xe416,0xe416,0xec57,
0xe3f6,0xec16,0xfc57,0xf3f5,0xf394,0xf374,0xeaf2,0xc24f,
0xe477,0xfdfd,0xe4d8,0xb351,0xd496,0xe537,0xfe1a,0xfe3b,
0xd2ce,0x9866,0xc98a,0xeacf,0xf3b2,0xfc13,0xfbd2,0xf371,
0xfbf2,0xfbf2,0xfbd1,0xfb90,0xf2ed,0xea6b,0xea2a,0xea4a,
0xea2a,0xea6c,0xe2ad,0xe32f,0xec13,0xecd6,0xf5b9,0xfedd,
0xf71d,0xf79e,0xef5d,0xffff,0xf79f,0xf7df,0xffff,0x94f4,
0x31e9,0x4aec,0x636f,0x63d0,0x6bf1,0x6c11,0x7432,0x7c93,
0x84d4,0x9514,0x9535,0x8cf4,0x84f4,0x8cf4,0x84d3,0x7472,
0x6410,0x4b4d,0x4b6e,0x538e,0x430c,0x4b4d,0x5bf0,0x53cf,
0xcc95,0xcc75,0xd455,0xdc76,0xe456,0xe415,0xec16,0xf457,
0xebf6,0xf416,0xfc57,0xf416,0xebb4,0xf394,0xeb12,0xc26f,
0xcbb4,0xff1f,0xecd8,0xaad0,0xbbd3,0xfe9d,0xed57,0xfe5b,
0xe3b2,0x8045,0xfbb2,0xfcd6,0xeb90,0xeb91,0xf3b1,0xfc53,
0xf3d0,0xebcf,0xeb8e,0xe32c,0xd269,0xca07,0xd208,0xe26a,
0xe24a,0xe28c,0xd2ad,0xd30f,0xe454,0xf578,0xfe5c,0xfefe,
0xff7f,0xf79e,0xf79f,0xffff,0xf7bf,0xef9e,0xffff,0xdf5d,
0x7c31,0x6bd0,0x6bb0,0x6c11,0x7452,0x7c93,0x84d4,0x84b3,
0x8cf4,0x9d55,0xa5b7,0x9d96,0x9d96,0x9d76,0x9555,0x84f4,
0x84d3,0x6c51,0x7492,0x7cb3,0x6c31,0x6c51,0x7492,0x6c51,
0xc4b5,0xccb5,0xd4b6,0xdcb7,0xe476,0xe436,0xec36,0xf457,
0xf457,0xf436,0xf457,0xec16,0xe3b4,0xebb4,0xe373,0xcad1,
0xc332,0xfd9b,0xfddc,0xd435,0x81cb,0xf538,0xf538,0xfd79,
0xfe1b,0xbaae,0xe3b1,0xfc95,0xebb1,0xfc12,0xeb4f,0xeb6f,
0xd32b,0xcac9,0xc268,0xc247,0xc227,0xca07,0xca07,0xd228,
0xd209,0xdaac,0xdaed,0xd370,0xecb5,0xfdfb,0xfe7d,0xfede,
0xff7f,0xf77e,0xffdf,0xffff,0xf7bf,0xef7e,0xef9f,0xf7ff,
0x9d56,0x7411,0x63af,0x6bd0,0x6bf0,0x7432,0x7c93,0x7452,
0x8cf4,0x9d96,0xb618,0xb659,0xb639,0xae18,0xa5b7,0x9d96,
0x9d76,0x9535,0x9576,0x9d97,0x8d55,0x8d56,0x9576,0x8d35,
0xcd36,0xcd16,0xd4d6,0xdcd7,0xe497,0xe476,0xec56,0xec57,
0xec57,0xec16,0xe416,0xdbd5,0xdb94,0xe3b4,0xe3b4,0xd352,
0xaa4e,0xfd19,0xfedf,0xaa8e,0x91ec,0xec96,0xfd59,0xe475,
0xfe1b,0xfdd9,0xbb2f,0xcb6f,0xd34f,0xe36f,0xca8c,0xca8a,
0xc268,0xba06,0xb9c4,0xb9e5,0xc206,0xca06,0xc9e7,0xc9e7,
0xc9a8,0xda8c,0xe32f,0xe3b2,0xf4d7,0xfddb,0xfe5d,0xfe9d,
0xff7f,0xf77e,0xffbf,0xff9f,0xf79f,0xffdf,0xef7f,0xf7df,
0x7c11,0x6bd0,0x7c52,0x8493,0x7431,0x6c11,0x7c93,0x7cb3,
0x9535,0xa596,0xb639,0xb659,0xb659,0xb639,0xae18,0xadf8,
0xadf8,0xa5d7,0xa5d8,0xa5b8,0xa5b7,0xa5d8,0xadf8,0xa5f8,
0xcd58,0xcd37,0xd4f7,0xd4f7,0xe4d7,0xe4b8,0xec98,0xec77,
0xec98,0xe457,0xe436,0xdc15,0xdbd5,0xe3f5,0xec16,0xe3b4,
0xba2e,0xf415,0xfddb,0xcb31,0xba8f,0xec36,0xfcb7,0xf4f8,
0xcc54,0xfefe,0xc3f1,0xb2ed,0xc2cd,0xcacc,0xc26b,0xdb0c,
0xc226,0xc226,0xc226,0xc226,0xc206,0xc1c6,0xc9e7,0xd208,
0xc9e9,0xe2cd,0xeb70,0xebf3,0xf4d7,0xfdbb,0xfe5d,0xfedf,
0xff5f,0xff7e,0xff7f,0xf75e,0xff9f,0xffff,0xffff,0xf7df,
0x73f0,0x7411,0x8cb3,0x9515,0x8cf4,0x84f4,0x8d35,0x9556,
0x9535,0x9d55,0x9d96,0x9d96,0x9d97,0xa5b7,0xa5b7,0xa5d7,
0xa5d7,0xa5d7,0xa597,0x9d56,0x9d56,0x9d77,0xa597,0xa5b7,
0xd559,0xd579,0xdd59,0xe559,0xed5a,0xed5a,0xed5a,0xed3a,
0xed3a,0xed19,0xecf9,0xecf9,0xec98,0xf498,0xfc77,0xf3f5,
0xd28f,0x8846,0xd2f0,0xfe5e,0xca6f,0xb1cd,0xd312,0xfd5a,
0xdcf7,0xfe5b,0xd4d4,0xa2ac,0xcb0d,0xd30d,0xc24a,0xc269,
0xb9e6,0xba06,0xba06,0xb9e6,0xb1a5,0xa965,0xb9a6,0xc209,
0xca6b,0xe32f,0xebb2,0xec34,0xf4f7,0xfdbb,0xfe7d,0xff1f,
0xff7f,0xff7f,0xff7f,0xff9f,0xff9f,0xffdf,0xffff,0xef7e,
0x7c11,0x8472,0x8473,0x8493,0x8cf4,0x8d15,0x8d15,0x8d35,
0x9535,0x9535,0x9514,0x9514,0x9515,0x9515,0x9535,0x9555,
0x9d76,0xa597,0x9d56,0x9515,0x9515,0x9535,0x9515,0x9535,
0xdd5a,0xe59b,0xedbc,0xf5dc,0xf5fc,0xf5fd,0xf61d,0xf5fd,
0xf5fd,0xeddc,0xf5dc,0xfdbc,0xfd3a,0xfcd9,0xfc78,0xf3b4,
0xc9cc,0xa8a7,0xa0c8,0xfbb3,0xfcf9,0xfb95,0xb9cd,0xdb93,
0xfdfb,0xe578,0xfe9b,0xa2ed,0xaa2a,0xb1c8,0x9105,0xa186,
0x9985,0x9964,0x9144,0x8924,0x8924,0x9124,0x9966,0xa1c8,
0xba8c,0xcb2f,0xd3d2,0xdc54,0xed38,0xf5da,0xf67d,0xff1f,
0xff7f,0xff7f,0xff7f,0xffdf,0xf79f,0xef5e,0xf79f,0xdefc,
0x6b8f,0x8cb3,0x9515,0x8cd4,0x8cf4,0x84f4,0x84f4,0x9556,
0x9555,0x9535,0x9535,0x9555,0x9d56,0x9535,0x8d14,0x8cf4,
0x9d56,0xa597,0x9d77,0x9535,0x9d36,0x9d36,0x9515,0x9515,
0xe53a,0xed7b,0xfe3e,0xedbd,0xedfd,0xfedf,0xf6bf,0xf6df,
0xeebe,0xf69e,0xfe7e,0xfddc,0xfd5a,0xfcd9,0xfd5b,0xe2f1,
0xb8c8,0xb066,0xa066,0xa067,0xc96c,0xfbd6,0xfbd6,0xa12a,
0xc3b2,0xfe3b,0xfedd,0xecf5,0x6001,0x88a4,0xa187,0xa208,
0xaaaa,0xd430,0xd451,0xcc31,0xe514,0xf576,0xed35,0xf556,
0xe4f5,0xe536,0xdcf5,0xd536,0xee1a,0xee9c,0xe6dc,0xf75f,
0xff9f,0xff7f,0xff9f,0xf75e,0xef5e,0xe71d,0xf79f,0xb5b7,
0x7c11,0x94f4,0x9d76,0x9515,0x7c93,0x6c11,0x7451,0x84f4,
0x84d4,0x9535,0xa5d7,0xb639,0xb639,0xadf8,0xa5b7,0xa597,
0xa597,0xadb7,0xadb8,0xadb8,0xa577,0x9d56,0x9d36,0x9d15,
0xf59d,0xf57c,0xfdfe,0xf61e,0xfebf,0xff1f,0xe69f,0xef1f,
0xffbf,0xff5f,0xee3c,0xfe1c,0xfe3d,0xfd59,0xfc56,0xd22e,
0xb887,0xb845,0xb066,0xa846,0xa005,0xa807,0xe20f,0xfc77,
0xa20c,0xbb91,0xfe5b,0xfe1a,0x99c9,0xe390,0xec11,0xfcf5,
0xfe19,0xfe39,0xfe39,0xfebb,0xff5e,0xfefc,0xfebb,0xff5e,
0xfefd,0xff1e,0xfefd,0xf6bc,0xeedc,0xe71d,0xe75d,0xf7bf,
0xff9f,0xffdf,0xffff,0xe6dc,0xdedc,0xe71d,0xe73d,0x8452,
0x94f4,0xa576,0xa597,0x8d15,0x7c93,0x6bf0,0x63d0,0x6c31,
0x84d3,0x9d76,0xb639,0xbe9a,0xbe7a,0xb619,0xb5f8,0xb618,
0xadf8,0xadf8,0xadf8,0xadd8,0xadb8,0xadb8,0xa597,0xa577,
0xfd7d,0xfdbe,0xfdff,0xf61e,0xf69f,0xf6df,0xe6be,0xffbf,
0xdebd,0xd63a,0xfefd,0xfebd,0xfe3b,0xf495,0x9928,0xa908,
0xb866,0xc066,0xb025,0xb026,0xb847,0xa806,0xb047,0xc9cd,
0xfcb7,0xcb71,0xdbf3,0xfe5c,0xfcf6,0xfd16,0xec73,0xfdb8,
0xfdf9,0xedd9,0xfe9c,0xfedd,0xee3a,0xf69c,0xff5f,0xff1e,
0xfefd,0xf6bc,0xff1d,0xffbf,0xff9f,0xef5e,0xefbf,0xffff,
0xef5e,0xd69b,0xce5a,0xc619,0xb5b8,0xa515,0xa556,0x8452,
0xadb7,0xadd8,0xa597,0x9515,0x84b3,0x7431,0x63f0,0x6bf0,
0x84d3,0xa576,0xbe59,0xc69a,0xbe5a,0xb619,0xb619,0xbe39,
0xadf8,0xa5b7,0x9d76,0x9d56,0x9d56,0x9d56,0x9535,0x94f5,
0xf59d,0xfe3f,0xfe3f,0xedfd,0xf67f,0xf6bf,0xe6bd,0xf73f,
0xf77f,0xffdf,0xff9e,0xdd55,0xab2d,0xbb0d,0x9966,0xa926,
0xb886,0xd0c7,0xc086,0xb004,0xb005,0xb026,0xa805,0x9004,
0xc1cc,0xfc96,0xd330,0xfd17,0xfd79,0xfd37,0xfd98,0xfe3a,
0xfe3b,0xf61b,0xf67c,0xfedd,0xff1e,0xff9f,0xff7f,0xee9c,
0xff9f,0xff1f,0xff1e,0xf71e,0xeefd,0xf79f,0xf7bf,0xdefc,
0xb5d8,0x9cd4,0xa536,0xbe19,0xad77,0x7bf1,0x8472,0xa576,
0xa597,0xa597,0x9535,0x84d3,0x84b3,0x7472,0x7431,0x7c92,
0x8cf4,0x9d76,0xb5f8,0xb618,0xadb7,0xa597,0xa5b7,0xa5d7,
0xa5b7,0x9535,0x8cd4,0x84d4,0x8cf5,0x8cf5,0x8cd4,0x84b3,
0xfe5f,0xfe7f,0xf61e,0xf63e,0xfebf,0xf6be,0xe69c,0xeebb,
0xff7d,0xf71b,0xd5b5,0xdd94,0x92a8,0x68e1,0x8984,0x9924,
0xb8e6,0xb004,0xb845,0xc086,0xb004,0xb025,0xb887,0xa826,
0x9004,0xfc15,0xeb72,0xfcf7,0xfd17,0xcc33,0xcc53,0xcc94,
0xccd5,0xd537,0xdd98,0xf67c,0xff5f,0xf6bd,0xee9c,0xffbf,
0xf6de,0xff7f,0xff9f,0xf6fe,0xf71e,0xffdf,0xe6dc,0x83f0,
0x5aec,0x6baf,0x94f5,0xad97,0x9d35,0x94d4,0x8cb3,0x8cd4,
0x9514,0x9514,0x84b3,0x7c51,0x7451,0x6c10,0x7451,0x84d3,
0x9d56,0xa596,0xa597,0x9d56,0x9515,0x9515,0x9515,0x8d14,
0x84d4,0x7452,0x6bf0,0x6c11,0x7452,0x7c72,0x7c72,0x7452,
0xfedf,0xf67f,0xee1d,0xf67e,0xee3c,0xe5fa,0xfebc,0xff5d,
0xee99,0x6247,0x5162,0x7a86,0x8285,0x8a44,0x7140,0x8901,
0xb925,0xa803,0xb844,0xd0c7,0xb865,0xb086,0xb8a7,0x9803,
0xb025,0xb067,0xc1ac,0xfcf8,0xfd99,0xe537,0xbc54,0xac32,
0xf63a,0xc4f5,0xd5b8,0xd5d8,0x9c52,0xacd5,0xe69c,0xde7c,
0xe69d,0xe69d,0xeebd,0xeebd,0xe69c,0xc5b8,0x736f,0x20e5,
0x31a8,0x52ec,0x7c31,0x8cb3,0x8cf4,0xa597,0x94f4,0x84b3,
0x9514,0x9514,0x8cd3,0x7c92,0x7431,0x6bf0,0x7431,0x8cd3,
0xadd7,0xadd7,0xa597,0x9535,0x8cf4,0x8cd4,0x84b3,0x7c52,
0x7452,0x63d0,0x5b8f,0x63af,0x63d0,0x63d0,0x5baf,0x63d0,
0xee9e,0xee9e,0xee5d,0xf67d,0xedfa,0xf61a,0xfebc,0xe5d7,
0x4902,0x4901,0x8b28,0x6a23,0x7222,0xa366,0x7a00,0x7920,
0xa0c3,0xb885,0xc8a6,0xb824,0x9801,0xa003,0xb085,0xa845,
0xb826,0xb006,0xd1cc,0xfc55,0xe4d6,0xdd99,0xbcd6,0x8b90,
0x834d,0x28c2,0x3944,0x41c6,0x18c3,0x6b6e,0xa556,0x52ac,
0x62ed,0x422a,0x41ea,0x5aad,0x62ee,0x39e9,0x2146,0x2987,
0x31e8,0x426b,0x636e,0x8472,0x8cd4,0xa597,0x9514,0xa5b7,
0xa5b7,0xa5b7,0x9d76,0x9535,0x84d3,0x7431,0x7431,0x8cf4,
0xadb7,0xb5d8,0xadb7,0x9d55,0x9515,0x8cd4,0x7c52,0x63cf,
0x6bf0,0x63af,0x5b8f,0x5baf,0x5b8f,0x534e,0x4b4e,0x538e,
0xde5c,0xeebd,0xe69c,0xee9c,0xfebc,0xff7e,0xfe38,0x69c5,
0x69e4,0x8b07,0x5180,0x6a42,0x9ba7,0x8261,0xa304,0x70c0,
0x8020,0x9001,0x9801,0xa002,0xb0a5,0xb0a5,0xa844,0xb065,
0xc066,0xb867,0xa8a8,0xfc55,0xfdda,0xbcd5,0x9412,0x730e,
0x5a8a,0x4a28,0x4a48,0x4a89,0x426a,0x428a,0x428a,0x3229,
0x52ed,0x636f,0x634f,0x52ee,0x4aac,0x31e9,0x1926,0x1967,
0x08c4,0x29c8,0x532d,0x7431,0x7c52,0x9535,0x84b3,0xa5b7,
0xadf8,0xae18,0xadf8,0xa5b7,0x9555,0x7c92,0x7c72,0x8cf4,
0x9d76,0xadb7,0xadd8,0xa596,0x9515,0x84b3,0x7411,0x5b6e,
0x3a6b,0x326a,0x3a8b,0x3acc,0x3acc,0x328b,0x3aab,0x430d,
0xe65b,0xee9c,0xef1c,0xf77d,0xe698,0xb4d0,0x5962,0x6182,
0x9307,0x5140,0x72c3,0x7b04,0x7261,0x9b05,0x99e2,0x7860,
0x88a0,0x88a1,0x7820,0x8021,0x9843,0xb044,0xb025,0xb825,
0xc066,0xc109,0xa908,0xf4b5,0xfebc,0xff5e,0xeefd,0xa4d4,
0x6b6e,0x7c10,0x8c92,0x8472,0x63af,0x4acc,0x42ab,0x4aed,
0x5b4f,0x5b6f,0x534f,0x4aee,0x42cd,0x322b,0x19a8,0x19a8,
0x1988,0x21e9,0x42cc,0x5bb0,0x63f0,0x63cf,0x6c10,0x7c92,
0x9535,0xa5b7,0xb639,0xb659,0xa5f8,0x9555,0x84f4,0x7cb3,
0x8d15,0x9d56,0xa5b7,0x9d96,0x8cf5,0x84b3,0x6bf1,0x4aed,
0x2a2a,0x1987,0x1167,0x19c8,0x19c9,0x21e9,0x2a2a,0x2a4a,
0xde7a,0xdebb,0xa533,0x6b8b,0x18e0,0x0800,0x30e0,0x9367,
0x7262,0x6200,0x72c2,0x6a61,0x7221,0x9263,0x9960,0x90e0,
0x8100,0xa2c6,0x9ae7,0x7183,0x6800,0x7801,0xa085,0xb0c7,
0xb908,0xb96a,0xa189,0xd413,0xfebc,0xff9f,0xeefd,0xffbf,
0xd6fd,0xadb8,0x94f5,0x9536,0x8cf5,0x63d0,0x4aed,0x42ed,
0x4aed,0x4aed,0x42ed,0x430e,0x4b0e,0x328c,0x19a9,0x1188,
0x2a0a,0x326b,0x4b0e,0x63f1,0x6bf1,0x5b8f,0x534e,0x63af,
0x84b3,0x9535,0xa5d7,0xa5f8,0x9db7,0x8d35,0x84f4,0x84f4,
0x9d97,0xa597,0xa5b8,0x9d76,0x8cf5,0x7c93,0x6bf1,0x4b0d,
0x21e9,0x1167,0x1147,0x1188,0x19a8,0x21e9,0x2a4b,0x2a4b,
0x5b0b,0x3a48,0x0040,0x00a0,0x08a0,0x10e0,0x4221,0x5ac2,
0x41c0,0x72a2,0x82a2,0x8a21,0x9a42,0x91a0,0x78c0,0x7900,
0x8aa3,0x72e4,0x6b45,0x6326,0x72c7,0x6a06,0x6966,0x7105,
0x8127,0x8948,0xbacf,0xb371,0xac54,0xf73f,0xff1f,0xeeff,
0xe73f,0xb63a,0xa597,0xa5b8,0x9536,0x6bf1,0x532e,0x534e,
0x4b0e,0x326b,0x19a9,0x19a9,0x220a,0x19c9,0x1168,0x1188,
0x2a2b,0x3aac,0x538f,0x7473,0x7cb4,0x7431,0x6bf0,0x6bf0,
0x84b3,0x9535,0x9db7,0x9d96,0x8d35,0x84f4,0x7cd4,0x84f4,
0x9d77,0x9d56,0x9556,0x8d15,0x7c93,0x7c73,0x63d1,0x42cc,
0x21e9,0x19a8,0x1188,0x19a8,0x21e9,0x328c,0x42ee,0x3acd,
0x0080,0x00a0,0x00a0,0x1961,0x1940,0x1160,0x3260,0x42a1,
0x6323,0x7b43,0x7220,0x79a0,0x89e1,0x89a0,0x89e1,0x9ac4,
0x9b46,0x7305,0x6c08,0x642a,0x63ca,0x52e8,0x5aa9,0x7aeb,
0x7209,0x58a4,0xa2ae,0xc434,0x93d2,0xbd58,0xbd38,0xb538,
0x94d5,0x9d56,0xa5b8,0x9d77,0x8494,0x63d0,0x5b90,0x63b0,
0x6c33,0x4b2f,0x220a,0x1188,0x19a9,0x1168,0x1188,0x220a,
0x220a,0x328c,0x5bb0,0x8cf5,0x9d98,0x9576,0x8d15,0x8cf4,
0x84d3,0x8d14,0x8d14,0x84d3,0x6c51,0x6c11,0x6c51,0x7c93,
0x84d4,0x84d4,0x84d4,0x84b4,0x7c93,0x7c73,0x63f1,0x42cd,
0x2a2a,0x220a,0x220a,0x2a0a,0x326c,0x4b4f,0x5bd1,0x5bb1,
0x1963,0x00a0,0x0900,0x1960,0x2a22,0x42e3,0x42c2,0x42c1,
0x6322,0x5a60,0x6240,0x8b24,0x9b65,0x8ae3,0x8304,0x8ac4,
0x7962,0x8247,0x9caf,0x63ec,0x4b29,0x3ac8,0x532b,0x634c,
0x93cf,0x8aec,0x930d,0xbc93,0xacf4,0x9cb3,0x83d0,0x8c52,
0x7411,0x7452,0x7c73,0x7c73,0x6c12,0x63b0,0x63d1,0x6c32,
0x7cd5,0x6c53,0x5370,0x3acd,0x2a4c,0x11a9,0x0968,0x19a9,
0x21c9,0x326b,0x5b90,0x8d15,0xa5f9,0xadf9,0x9d77,0x9535,
0x7c72,0x7c72,0x6c11,0x536e,0x42ec,0x42cc,0x4b4e,0x5bb0,
0x7452,0x7c72,0x84d4,0x8cf4,0x84d4,0x84b4,0x6c12,0x4b0e,
0x3aac,0x328c,0x328c,0x328c,0x3acd,0x5390,0x6433,0x6412,
0x0060,0x0040,0x0900,0x0920,0x2a42,0x4324,0x4343,0x5bc4,
0x6ba4,0x41e0,0x51c0,0x7a81,0x7a41,0x7a21,0x8a63,0x89e3,
0x8081,0x70a3,0x938d,0x6b6c,0x638c,0x5bac,0x6c2f,0x742f,
0xbdd5,0xb4f3,0x9bce,0xb4b2,0x9cf2,0x9512,0x94b1,0x7bef,
0x7411,0x532e,0x42cc,0x42ed,0x42cc,0x326b,0x4b2e,0x7453,
0x7cd5,0x7cf5,0x7474,0x5390,0x3acd,0x19c9,0x0968,0x19a9,
0x19a9,0x2a2a,0x534f,0x84d4,0xadd9,0xae19,0xa5b8,0x9d56,
0x84b3,0x7452,0x63af,0x42cc,0x2a09,0x21e9,0x324a,0x3aac,
0x6bf1,0x7452,0x8cf5,0x8d15,0x84d4,0x7cb4,0x6c32,0x4b4e,
0x4b4e,0x430d,0x42ed,0x42ed,0x42ee,0x536f,0x5bf2,0x5bd1,
0x1101,0x08e0,0x00e0,0x00c0,0x1a61,0x22c2,0x2b02,0x3b42,
0x3aa0,0x39a0,0x7241,0x8a22,0x9140,0xa100,0x9840,0x9800,
0xb8c5,0x9064,0xbb8f,0x9bcf,0x9451,0x8c71,0x9d74,0xb617,
0xa512,0x6b0a,0x6aa9,0xad12,0x9531,0x84f0,0x94f1,0x8cb1,
0x6bf0,0x42ed,0x2a0a,0x21e9,0x1988,0x1147,0x2a2b,0x5b90,
0x7494,0x84f6,0x7cb5,0x5bb1,0x3acd,0x222b,0x19ea,0x222b,
0x19c9,0x2a2a,0x4b0e,0x7c93,0xa5d8,0xb63a,0xadf9,0x9d77,
0x8d15,0x84b3,0x6c11,0x532e,0x3a8c,0x324b,0x328b,0x3aac,
0x63d0,0x7c72,0x8cf5,0x84f5,0x7c73,0x7473,0x6c32,0x538f,
0x536f,0x430e,0x42ed,0x3aed,0x32ad,0x3acd,0x432f,0x432f,
0x0020,0x0901,0x00e0,0x0940,0x4346,0x3304,0x4385,0x4b23,
0x4220,0x61c0,0xa264,0x9921,0xa040,0xb8a1,0xb000,0xc0a3,
0xa022,0x90a3,0xd471,0xbcd2,0xce16,0xc637,0x9532,0x532a,
0x1902,0x31a4,0x39c5,0x6b8c,0x9593,0xa636,0x8cf2,0x94f3,
0x7c93,0x5bb0,0x326b,0x19c8,0x1188,0x19a8,0x21e9,0x2a2a,
0x4b4f,0x6412,0x6412,0x4b4f,0x328c,0x21ea,0x11a9,0x19a9,
0x21ea,0x2a2a,0x4b0d,0x7453,0x9d98,0xae19,0xa5b8,0x9556,
0x8cf4,0x84b3,0x7452,0x63d0,0x5b6f,0x534f,0x536f,0x5b8f,
0x63f1,0x7c73,0x8d15,0x84f4,0x7452,0x7452,0x7452,0x63f1,
0x4b2e,0x328c,0x326c,0x328c,0x2a4b,0x222b,0x2a6c,0x328c,
0x00c0,0x08e1,0x10e1,0x0840,0x10a0,0x4a84,0x6326,0x5201,
0x8243,0x9982,0xb0a1,0xc841,0xc842,0xc062,0xa881,0x8880,
0x78c0,0x79a1,0xb50d,0xbdf1,0xced5,0x6beb,0x2204,0x19c3,
0x3aa7,0x63ac,0x8cd2,0x9d54,0x8d54,0x8514,0x94d5,0x94f6,
0x8cd5,0x5bb0,0x326b,0x19c9,0x19a8,0x19a8,0x19c9,0x21ea,
0x220a,0x2a2a,0x2a2a,0x220a,0x21ea,0x220a,0x220a,0x220a,
0x2a4b,0x324a,0x3a8c,0x536f,0x84b4,0x9d97,0xa5b7,0x9d56,
0x9556,0x9515,0x84b4,0x7452,0x7432,0x7c73,0x7473,0x7452,
0x7c93,0x8d15,0x9556,0x8d15,0x7473,0x6c11,0x5b90,0x4b4e,
0x324b,0x220a,0x21e9,0x220a,0x222b,0x222b,0x222b,0x2a4c,
0x00c1,0x00a0,0x0860,0x0840,0x0860,0x2940,0x5a63,0x7aa5,
0x91e3,0xb102,0xc821,0xd801,0xd022,0xb882,0x90a2,0x70e0,
0x71c0,0x8365,0x8489,0x5bc6,0x21c0,0x42e5,0x4b69,0x748e,
0x6c4e,0x7cf1,0x9554,0x9575,0x8575,0x8536,0x94b6,0x8c55,
0x63d1,0x4b0e,0x326b,0x2a4a,0x2a2a,0x220a,0x2a2a,0x2a4b,
0x2a2a,0x2a4a,0x2a4b,0x222a,0x220a,0x222a,0x2a4b,0x2a4b,
0x328b,0x3a8b,0x3aac,0x534e,0x7452,0x9555,0xa597,0xa597,
0x8cd4,0x84d4,0x84b4,0x84d4,0x8cd5,0x84f5,0x84b4,0x7c94,
0x7c73,0x84d5,0x8d16,0x84f5,0x7cb4,0x6c32,0x536f,0x3acc,
0x220a,0x19e9,0x19c9,0x19ea,0x222b,0x2a4b,0x2a4c,0x328c,
0x0061,0x0060,0x0080,0x00c0,0x0100,0x0940,0x39e1,0x7ac6,
0xa245,0xb943,0xc842,0xc801,0xc022,0xa8a2,0x8102,0x6161,
0x7282,0x4a40,0x2a40,0x53e7,0x4b87,0x8d2f,0x746d,0x644d,
0x8511,0x8d73,0x9594,0x8d54,0x8595,0x8d96,0x8cb5,0x7bd2,
0x5b4f,0x4b2e,0x430d,0x430d,0x42ed,0x3aac,0x3acc,0x430d,
0x42cd,0x430d,0x4b0e,0x42ed,0x3aac,0x3aac,0x3acc,0x3acc,
0x4b2e,0x532d,0x5b6e,0x63d0,0x7c92,0x9555,0xa5b7,0xa5b7,
0x8cd3,0x84b3,0x8cd4,0x9536,0x9536,0x8cf5,0x84d4,0x84d5,
0x84d4,0x8cf5,0x8d15,0x8d15,0x8d15,0x7cb4,0x5bb0,0x3aac,
0x21ea,0x19c9,0x11a9,0x19ea,0x222b,0x2a4c,0x2a8c,0x32ad,
0x0800,0x10a1,0x00e1,0x0120,0x01a1,0x0980,0x2120,0x5120,
0x9162,0xa8e2,0xb8a2,0xb8c2,0xa923,0x9984,0x8204,0x6a44,
0x51a0,0x41e1,0x5366,0x642a,0x63ea,0x42c7,0x4b29,0x748f,
0x8d32,0x9db4,0xa5d5,0x9594,0x8db5,0x8db6,0x8cd4,0x6bb0,
0x63b0,0x5bb0,0x63d0,0x63d1,0x538f,0x4b2e,0x4b4e,0x5bb0,
0x5bd0,0x6c32,0x7453,0x6c32,0x5bb0,0x536f,0x4b2e,0x4b0d,
0x534e,0x5b8f,0x6bf0,0x7c93,0x9556,0xadf8,0xb5f8,0xadd8,
0x9d35,0x8cf4,0x8cf4,0x9556,0x9536,0x84b4,0x7c93,0x84d5,
0x7cb4,0x7c93,0x7473,0x7452,0x7453,0x6c32,0x4b4f,0x326b,
0x220a,0x19e9,0x19c9,0x19ea,0x220b,0x2a4b,0x2a6c,0x2a8c,
0x1801,0x18a2,0x0901,0x0140,0x0180,0x00e0,0x1820,0x4020,
0x8942,0xa183,0xaa04,0xa245,0x8244,0x6203,0x49c1,0x4981,
0x6163,0x8ae9,0x6b2a,0x29e5,0x530a,0x3227,0x42c9,0x6c0e,
0x9553,0xae15,0xb636,0x9db4,0x7d52,0x7d32,0x7490,0x63ae,
0x63d0,0x6c11,0x7452,0x7473,0x63f1,0x536f,0x536f,0x63d0,
0x7473,0x84f4,0x9556,0x8d35,0x7c93,0x63f0,0x536f,0x4b2d,
0x42ec,0x534d,0x6bf0,0x84d3,0xa5d7,0xbe59,0xbe59,0xb618,
0x9d55,0x9514,0x9515,0x9d76,0x9556,0x84b3,0x7c93,0x84f5,
0x84d5,0x7cb4,0x6c32,0x5bb0,0x5b90,0x5b90,0x4b2e,0x328c,
0x2a2a,0x220a,0x19c9,0x19c9,0x19ea,0x1a0a,0x1a0b,0x1a0a,
0x1862,0x1081,0x00c0,0x0120,0x0120,0x00a0,0x28a0,0x6161,
0x9a44,0xa265,0x9a85,0x8a43,0x69c1,0x5140,0x50e0,0x50a0,
0x5000,0xb36e,0x836d,0x4248,0x5b2c,0x42ea,0x3aa9,0x5bcd,
0x84b1,0x9d33,0x9d33,0x7c70,0x53ed,0x540d,0x640e,0x63ef,
0x63d0,0x6c32,0x84d4,0x8d15,0x7cb4,0x6c11,0x63f1,0x6c31,
0x84d4,0x9576,0xa5f8,0xa5b7,0x8d15,0x7452,0x63d0,0x5b8f,
0x536e,0x5baf,0x7431,0x8cf3,0xa596,0xadf7,0xadd7,0x9d75,
0x84b3,0x84b3,0x9535,0xa5b7,0x9d76,0x84d4,0x7c93,0x84d5,
0x8d36,0x84f5,0x7453,0x63b0,0x538f,0x538f,0x4b4f,0x42ed,
0x2a6b,0x2a4b,0x220a,0x19e9,0x19ea,0x1a0a,0x1a0a,0x19ea,
0x08a1,0x0080,0x00a0,0x08e0,0x10e0,0x2940,0x5202,0x6a63,
0x7222,0x7201,0x71c0,0x7940,0x80e0,0x98a0,0xb081,0xb0a3,
0x8883,0xc3af,0x7b2b,0x4ac9,0x4b4b,0x330a,0x1a47,0x32ea,
0x5b8d,0x6bce,0x738e,0x5b2c,0x3b0b,0x436d,0x63f0,0x7452,
0x7c93,0x84d4,0x9556,0x9d97,0x9556,0x84d4,0x7cb3,0x84d4,
0x9556,0xa5d8,0xb639,0xa5d8,0x8d15,0x7c92,0x7431,0x6c31,
0x6c31,0x7c92,0x84f3,0x9534,0x9534,0x9534,0x84b2,0x7430,
0x6bef,0x7431,0x8cd3,0x9d56,0x9535,0x84b3,0x7452,0x7c73,
0x7c94,0x7c93,0x6c32,0x5bb0,0x538f,0x538f,0x4b4f,0x42ed,
0x3acd,0x3acd,0x328c,0x2a6c,0x2a6c,0x32ad,0x32ad,0x2a6c,
0x00c1,0x00c0,0x08e0,0x08a0,0x10c0,0x4a23,0x6284,0x49a0,
0x5a00,0x61e0,0x69a0,0x8160,0xa0e0,0xb880,0xd000,0xc801,
0xa084,0xc38e,0x838c,0x42a8,0x432b,0x3b4b,0x3b8c,0x3b4c,
0x4b2c,0x5b2c,0x630d,0x52cc,0x432d,0x53af,0x6c73,0x84f5,
0x9577,0x9d97,0x9db7,0xa5d8,0x9d97,0x8d15,0x8d15,0x9535,
0x9d97,0xadf8,0xb639,0xa5b7,0x8cf4,0x7c72,0x7451,0x7451,
0x7431,0x84d3,0x9534,0x9555,0x8d13,0x7c71,0x6bcf,0x5b4d,
0x5b6d,0x6bcf,0x7c72,0x84d3,0x7c93,0x7431,0x6bf1,0x6bf1,
0x7452,0x7453,0x6c52,0x6c11,0x6c12,0x6412,0x5bd1,0x4b4f,
0x4b4f,0x4b4f,0x430f,0x3aee,0x430f,0x4b50,0x4b70,0x434f
};
