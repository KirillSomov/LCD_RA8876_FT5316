
#ifndef RA8876_H
#define RA8876_H


#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "SPI.h"
#include "Timer.h"


#define uchar      unsigned char
#define uint       unsigned int
#define ulong      unsigned long


/*==== [HW_(3)]  SDRAM  =====*/
//#define IS42SM16160D
//#define IS42S16320B
//#define IS42S16400F 
//#define M12L32162A 
//#define M12L2561616A 
//#define W9825G6JH
#define W9812G6JH
//#define MT48LC4M16A
//#define K4S641632N
//#define K4S281632K

//------------------------------------------------------------------------------
// Software Setting
//------------------------------------------------------------------------------
/*==== [SW_(1)]  PLL  =====*/
#define OSC_FREQ     10   // crystal clcok
#define DRAM_FREQ    100  // SDRAM clock frequency, unti: MHz		  
#define CORE_FREQ    100  // Core (system) clock frequency, unit: MHz 
#define SCAN_FREQ    50   // Panel Scan clock frequency, unit: MHz	 

//=====[05h] b3-b1=====//
//#define SCAN_DIVK1      // 000	   
#define SCAN_DIVK2	  // 001   
//#define SCAN_DIVK4 	  // 010
//#define SCAN_DIVK8 	  // 011 
//#define SCAN_DIVK16 	  // 100
//#define SCAN_DIVK32	  // 101
//#define SCAN_DIVK64	  // 110
//#define SCAN_DIVK128	  // 111
//=====[05h] b0
#define SCAN_DIVM1   	  // 0
//#define SCAN_DIVM2 	  // 1 

//=====[07h] b3-b1
//#define DRAM_DIVK1 	  // 000 
#define DRAM_DIVK2        // 001
//#define DRAM_DIVK4 	  // 010
//#define DRAM_DIVK8 	  // 011
//=====[07h] b0
//#define DRAM_DIVM1 	  // 0
//#define DRAM_DIVM2	  // 1

//=====[09h] b3-b1
//#define CORE_DIVK1 	  // 000
#define CORE_DIVK2	  // 001
//#define CORE_DIVK4   	  // 010
//#define CORE_DIVK8 	  // 011  
//=====[09h] b0			  
#define CORE_DIVM1   	  // 0
//#define CORE_DIVM2	  // 1


//------------------------------------------------------------------------------
// RA8876 LAYERS ADDRESSES
//------------------------------------------------------------------------------
#define PAGE0_START_ADDR 0
#define PAGE1_START_ADDR (1024 * 600 * 2 * 1)
#define PAGE2_START_ADDR (1024 * 600 * 2 * 2)
#define PAGE3_START_ADDR (1024 * 600 * 2 * 3)
#define PAGE4_START_ADDR (1024 * 600 * 2 * 4)
#define PAGE5_START_ADDR (1024 * 600 * 2 * 5)
#define PAGE6_START_ADDR (1024 * 600 * 2 * 6)
#define PAGE7_START_ADDR (1024 * 600 * 2 * 7)
#define PAGE8_START_ADDR (1024 * 600 * 2 * 8)
#define PAGE9_START_ADDR (1024 * 600 * 2 * 9)


#define color256_black   0x00
#define color256_white   0xff
#define color256_red     0xe0
#define color256_green   0x1c
#define color256_blue    0x03
#define color256_yellow  color256_red|color256_green
#define color256_cyan    color256_green|color256_blue
#define color256_purple  color256_red|color256_blue
 
#define color65k_black   0x0000
#define color65k_white   0xffff
#define color65k_red     0xf800
#define color65k_green   0x07e0
#define color65k_blue    0x001f
#define color65k_yellow  color65k_red|color65k_green
#define color65k_cyan    color65k_green|color65k_blue
#define color65k_purple  color65k_red|color65k_blue

#define color65k_grayscale1    2113
#define color65k_grayscale2    2113*2
#define color65k_grayscale3    2113*3
#define color65k_grayscale4    2113*4
#define color65k_grayscale5    2113*5
#define color65k_grayscale6    2113*6
#define color65k_grayscale7    2113*7
#define color65k_grayscale8    2113*8
#define color65k_grayscale9    2113*9
#define color65k_grayscale10   2113*10
#define color65k_grayscale11   2113*11
#define color65k_grayscale12   2113*12
#define color65k_grayscale13   2113*13
#define color65k_grayscale14   2113*14
#define color65k_grayscale15   2113*15
#define color65k_grayscale16   2113*16
#define color65k_grayscale17   2113*17
#define color65k_grayscale18   2113*18
#define color65k_grayscale19   2113*19
#define color65k_grayscale20   2113*20
#define color65k_grayscale21   2113*21
#define color65k_grayscale22   2113*22
#define color65k_grayscale23   2113*23
#define color65k_grayscale24   2113*24
#define color65k_grayscale25   2113*25
#define color65k_grayscale26   2113*26
#define color65k_grayscale27   2113*27
#define color65k_grayscale28   2113*28
#define color65k_grayscale29   2113*29
#define color65k_grayscale30   2113*30

 
#define color16M_black   0x00000000
#define color16M_white   0x00ffffff
#define color16M_red     0x00ff0000
#define color16M_green   0x0000ff00
#define color16M_blue    0x000000ff
#define color16M_yellow  color16M_red|color16M_green
#define color16M_cyan    color16M_green|color16M_blue
#define color16M_purple  color16M_red|color16M_blue


#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0


#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216
#define Line10         240
#define Line11         264
#define Line12         288
#define Line13         312
#define Line14         336
#define Line15         360
#define Line16         384
#define Line17         408
#define Line18         432
#define Line19         456
#define Line20         480
#define Line21         504
#define Line22         528
#define Line23         552
#define Line24         576




#define	cSetb0		0x01
#define	cSetb1		0x02
#define	cSetb2		0x04
#define	cSetb3		0x08
#define	cSetb4		0x10
#define	cSetb5		0x20
#define	cSetb6		0x40
#define	cSetb7		0x80

#define	cClrb0		0xfe
#define	cClrb1		0xfd
#define	cClrb2		0xfb
#define	cClrb3		0xf7
#define	cClrb4		0xef
#define	cClrb5		0xdf
#define	cClrb6		0xbf
#define	cClrb7		0x7f

//--
 
extern  unsigned int f3[1600];
extern  unsigned int f2[1600];
extern  unsigned int f1[1600];
extern  unsigned int pattern16x16_16bpp[256]; 
extern unsigned int ASCII_Table[];
extern  unsigned int pic_80x80[];
extern unsigned char gImage_no_im[256];
extern unsigned char gImage_busy_im[256];
extern unsigned char gImage_pen_il[256];
extern unsigned char gImage_arrow_il[256];


// ===========================================
// SERVICE
// ===========================================
void delay_us(unsigned int i);
void Delay_ms(uint i);
void Delay10ms(uint i);
void Delay100ms(uint i);


// ===========================================
// LOW LEVEL FUNCTIONS
// ===========================================
void LCD_CmdWrite(unsigned char cmd);
void LCD_DataWrite(unsigned char Data);
void LCD_RegisterWrite(unsigned char Cmd,unsigned char Data);
unsigned char LCD_RegisterRead(unsigned char Cmd);
unsigned char LCD_StatusRead(void);


// ===========================================
// RESET
// ===========================================
//**[00h]**//
void RA8876_SW_Reset(void);
void RA8876_HW_Reset(void);


// ===========================================
// INIT
// ===========================================
void RA8876_initial(void);
//Set PLL
void RA8876_PLL_Initial(void); 
//Set SDRAM 
void RA8876_SDRAM_initail(void);


// ===========================================
// POWER MODE
// ===========================================
//**[DFh]**//
void Power_Normal_Mode(void);
void Power_Saving_Standby_Mode(void);
void Power_Saving_Suspend_Mode(void);
void Power_Saving_Sleep_Mode(void);


// ===========================================
// PWM
// ===========================================
//**[84h]**//
void Set_PWM_Prescaler_1_to_256(unsigned short WX);
//**[85h]**//
void Select_PWM1_Clock_Divided_By_1(void);
void Select_PWM1_Clock_Divided_By_2(void);
void Select_PWM1_Clock_Divided_By_4(void);
void Select_PWM1_Clock_Divided_By_8(void);
void Select_PWM0_Clock_Divided_By_1(void);
void Select_PWM0_Clock_Divided_By_2(void);
void Select_PWM0_Clock_Divided_By_4(void);
void Select_PWM0_Clock_Divided_By_8(void);
//[85h].[bit3][bit2]
void Select_PWM1_is_ErrorFlag(void);
void Select_PWM1(void);
void Select_PWM1_is_Osc_Clock(void);
//[85h].[bit1][bit0]
void Select_PWM0_is_GPIO_C7(void);
void Select_PWM0(void);
void Select_PWM0_is_Core_Clock(void);
//**[86h]**//
//[86h]PWM1
void Enable_PWM1_Inverter(void);
void Disable_PWM1_Inverter(void);
void Auto_Reload_PWM1(void);
void One_Shot_PWM1(void);
void Start_PWM1(void);
void Stop_PWM1(void);
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void);
void Disable_PWM0_Dead_Zone(void);
void Enable_PWM0_Inverter(void);
void Disable_PWM0_Inverter(void);
void Auto_Reload_PWM0(void);
void One_Shot_PWM0(void);
void Start_PWM0(void);
void Stop_PWM0(void);
//**[87h]**//
void Set_Timer0_Dead_Zone_Length(unsigned char temp);
//**[88h][89h]**//
void Set_Timer0_Compare_Buffer(unsigned short WX);
//**[8Ah][8Bh]**//
void Set_Timer0_Count_Buffer(unsigned short WX);
//**[8Ch][8Dh]**//
void Set_Timer1_Compare_Buffer(unsigned short WX);
//**[8Eh][8Fh]**//
void Set_Timer1_Count_Buffer(unsigned short WX);


// ===========================================
// STATUS
// ===========================================
void Check_Mem_WR_FIFO_not_Full(void);
void Check_Mem_WR_FIFO_Empty(void);
void Check_Mem_RD_FIFO_not_Full(void);
void Check_Mem_RD_FIFO_not_Empty(void);
void Check_2D_Busy(void);
void Check_SDRAM_Ready(void);
unsigned char Power_Saving_Status(void);
void Check_Power_is_Normal(void);           //Normal mode
void Check_Power_is_Saving(void);           //Saving mode
void Check_NO_Interrupt(void);              //Interrupt
void Check_Interrupt_Occur(void);           //Interrupt


// ===========================================
// BUSY
// ===========================================
void Check_Busy_Draw(void);
void Check_Busy_SFI_DMA(void);


// ===========================================
// RGB24 DISPLAY CONFIG
// ===========================================
//**[01h]**//
void Enable_PLL(void);
void RA8876_Sleep(void);
void RA8876_WakeUp(void);
void TFT_24bit(void);
void TFT_18bit(void);
void TFT_16bit(void);
void TFT_LVDS(void);
void Key_Scan_Enable(void);
void Key_Scan_Disable(void);
void RA8876_I2CM_Enable(void);
void RA8876_I2CM_Disable(void);
void Enable_SFlash_SPI(void);
void Disable_SFlash_SPI(void);
void Host_Bus_8bit(void);
void Host_Bus_16bit(void);

//**[02h]**//
void RGB_8b_8bpp(void);
void RGB_8b_16bpp(void);
void RGB_8b_24bpp(void);

void RGB_16b_8bpp(void);
void RGB_16b_16bpp(void);
void RGB_16b_24bpp_mode1(void);
void RGB_16b_24bpp_mode2(void);

void MemRead_Left_Right_Top_Down(void);
void MemRead_Right_Left_Top_Down(void);
void MemRead_Top_Down_Left_Right(void);
void MemRead_Down_Top_Left_Right(void);

void MemWrite_Left_Right_Top_Down(void);
void MemWrite_Right_Left_Top_Down(void);
void MemWrite_Top_Down_Left_Right(void);
void MemWrite_Down_Top_Left_Right(void);

//**[03h]**//
void Interrupt_Active_Low(void);
void Interrupt_Active_High(void);
void ExtInterrupt_Debounce(void);
void ExtInterrupt_Nodebounce(void);
void ExtInterrupt_Input_Low_Level_Trigger(void);
void ExtInterrupt_Input_High_Level_Trigger(void);
void ExtInterrupt_Input_Falling_Edge_Trigger(void);
void ExtInterrupt_Input_Rising_Edge_Trigger(void);
void LVDS_Format1(void);
void LVDS_Format2(void);
void Graphic_Mode(void);
void Text_Mode(void);
void Memory_Select_SDRAM(void);
void Memory_Select_Graphic_Cursor_RAM(void);
void Memory_Select_Color_Palette_RAM(void);


// ===========================================
// INTERRUPTS
// ===========================================
//**[05h]**//
//**[06h]**//
//**[07h]**//
//**[09h]**//
//**[0Ah]**//
//**[0Bh]**//
void Enable_Resume_Interrupt(void);
void Disable_Resume_Interrupt(void);
void Enable_ExtInterrupt_Input(void);
void Disable_ExtInterrupt_Input(void);
void Enable_I2CM_Interrupt(void);
void Disable_I2CM_Interrupt(void);
void Enable_Vsync_Interrupt(void);
void Disable_Vsync_Interrupt(void);
void Enable_KeyScan_Interrupt(void);
void Disable_KeyScan_Interrupt(void);
void Enable_DMA_Draw_BTE_Interrupt(void);
void Disable_DMA_Draw_BTE_Interrupt(void);
void Enable_PWM1_Interrupt(void);
void Disable_PWM1_Interrupt(void);
void Enable_PWM0_Interrupt(void);
void Disable_PWM0_Interrupt(void);

//**[0Ch]**//
unsigned char Read_Interrupt_status(void);
void Clear_Resume_Interrupt_Flag(void);
void Clear_ExtInterrupt_Input_Flag(void);
void Clear_I2CM_Interrupt_Flag(void);
void Clear_Vsync_Interrupt_Flag(void);
void Clear_KeyScan_Interrupt_Flag(void);
void Clear_DMA_Draw_BTE_Interrupt_Flag(void);
void Clear_PWM1_Interrupt_Flag(void);
void Clear_PWM0_Interrupt_Flag(void);

//**[0Dh]**//
void Mask_Resume_Interrupt_Flag(void);
void Mask_ExtInterrupt_Input_Flag(void);
void Mask_I2CM_Interrupt_Flag(void);
void Mask_Vsync_Interrupt_Flag(void);
void Mask_KeyScan_Interrupt_Flag(void);
void Mask_DMA_Draw_BTE_Interrupt_Flag(void);
void Mask_PWM1_Interrupt_Flag(void);
void Mask_PWM0_Interrupt_Flag(void);

//
void Enable_Resume_Interrupt_Flag(void);
void Enable_ExtInterrupt_Input_Flag(void);
void Enable_I2CM_Interrupt_Flag(void);
void Enable_Vsync_Interrupt_Flag(void);
void Enable_KeyScan_Interrupt_Flag(void);
void Enable_DMA_Draw_BTE_Interrupt_Flag(void);
void Enable_PWM1_Interrupt_Flag(void);
void Enable_PWM0_Interrupt_Flag(void);


//**[0Eh]**//
void Enable_GPIOF_PullUp(void);
void Enable_GPIOE_PullUp(void);
void Enable_GPIOD_PullUp(void);
void Enable_GPIOC_PullUp(void);
void Enable_XDB15_8_PullUp(void);
void Enable_XDB7_0_PullUp(void);
void Disable_GPIOF_PullUp(void);
void Disable_GPIOE_PullUp(void);
void Disable_GPIOD_PullUp(void);
void Disable_GPIOC_PullUp(void);
void Disable_XDB15_8_PullUp(void);
void Disable_XDB7_0_PullUp(void);


//**[0Fh]**//
void XPDAT18_Set_GPIO_D7(void);
void XPDAT18_Set_KOUT4(void);
void XPDAT17_Set_GPIO_D5(void);
void XPDAT17_Set_KOUT2(void);
void XPDAT16_Set_GPIO_D4(void);
void XPDAT16_Set_KOUT1(void);
void XPDAT9_Set_GPIO_D3(void);
void XPDAT9_Set_KOUT3(void);
void XPDAT8_Set_GPIO_D2(void);
void XPDAT8_Set_KIN3(void);
void XPDAT2_Set_GPIO_D6(void);
void XPDAT2_Set_KIN4(void);
void XPDAT1_Set_GPIO_D1(void);
void XPDAT1_Set_KIN2(void);
void XPDAT0_Set_GPIO_D0(void);
void XPDAT0_Set_KIN1(void);


void Select_Main_Window_8bpp(void);
void Select_Main_Window_16bpp(void);
void Select_Main_Window_24bpp(void);


// ======================================================
// Display Configuration Register (DPCR)
// ======================================================
//**[12h]**//
void PCLK_Rising(void);
void PCLK_Falling(void);
void Display_ON(void);
void Display_OFF(void);
void Color_Bar_ON(void);
void Color_Bar_OFF(void);
void HSCAN_L_to_R(void);
void HSCAN_R_to_L(void);
void VSCAN_T_to_B(void);
void VSCAN_B_to_T(void);
void PDATA_Set_RGB(void);
void PDATA_Set_RBG(void);
void PDATA_Set_GRB(void);
void PDATA_Set_GBR(void);
void PDATA_Set_BRG(void);
void PDATA_Set_BGR(void);
void PDATA_IDLE_STATE(void);


// ======================================================
// RGB24 IF CONFIG
// ======================================================
//**[13h]**//
void HSYNC_Low_Active(void);
void HSYNC_High_Active(void);
void VSYNC_Low_Active(void);
void VSYNC_High_Active(void);
void DE_Low_Active(void);
void DE_High_Active(void);
void Idle_DE_Low(void);
void Idle_DE_High(void);
void Idle_PCLK_Low(void);
void Idle_PCLK_High(void);
void Idle_PDAT_Low(void);
void Idle_PDAT_High(void);
void Idle_HSYNC_Low(void);
void Idle_HSYNC_High(void);
void Idle_VSYNC_Low(void);
void Idle_VSYNC_High(void);


//**[14h][15h][1Ah][1Bh]**//
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY);
//**[16h][17h]**//
void LCD_Horizontal_Non_Display(unsigned short WX);
//**[18h]**//
void LCD_HSYNC_Start_Position(unsigned short WX);
//**[19h]**//
void LCD_HSYNC_Pulse_Width(unsigned short WX);
//**[1Ch][1Dh]**//
void LCD_Vertical_Non_Display(unsigned short HY);
//**[1Eh]**//
void LCD_VSYNC_Start_Position(unsigned short HY);
//**[1Fh]**//
void LCD_VSYNC_Pulse_Width(unsigned short HY);


// ===========================================
// FRAME BUFFER
// ===========================================
//**[20h][21h][22h][23h]**//
void Main_Image_Start_Address(unsigned long Addr);
//**[24h][25h]**//
void Main_Image_Width(unsigned short WX);
//**[26h][27h][28h][29h]**//
void Main_Window_Start_XY(unsigned short WX,unsigned short HY);


// ===========================================
// CANVAS
// ===========================================
//**[50h][51h][52h][53h]**//
void Canvas_Image_Start_address(unsigned long Addr);
//**[54h][55h]**//
void Canvas_image_width(unsigned short WX);
//**[56h][57h][58h][59h]**//
void Active_Window_XY(unsigned short WX,unsigned short HY);
//**[5Ah][5Bh][5Ch][5Dh]**//
void Active_Window_WH(unsigned short WX,unsigned short HY);
//**[5E]**//
void Select_Write_Data_Position(void);
void Select_Read_Data_Position(void);
void Memory_XY_Mode(void);
void Memory_Linear_Mode(void);
void Memory_8bpp_Mode(void);
void Memory_16bpp_Mode(void);
void Memory_24bpp_Mode(void);
//**[5Fh][60h][61h][62h]**//
void Goto_Pixel_XY(unsigned short WX,unsigned short HY);
void Goto_Linear_Addr(unsigned long Addr);
//**[63h][64h][65h][66h]**//
void Goto_Text_XY(unsigned short WX,unsigned short HY);


// ===========================================
// PICTURE IN PICTURE CONFIG 
// ===========================================
//**[10h]**//
void Enable_PIP1(void);
void Disable_PIP1(void);
void Enable_PIP2(void);
void Disable_PIP2(void);
void Select_PIP1_Parameter(void);
void Select_PIP2_Parameter(void);

//**[11h]**//
void Select_PIP1_Window_8bpp(void);
void Select_PIP1_Window_16bpp(void);
void Select_PIP1_Window_24bpp(void);
void Select_PIP2_Window_8bpp(void);
void Select_PIP2_Window_16bpp(void);
void Select_PIP2_Window_24bpp(void);

//**[2Ah][2Bh][2Ch][2Dh]**//
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY);
//**[2Eh][2Fh][30h][31h]**//
void PIP_Image_Start_Address(unsigned long Addr);
//**[32h][33h]**//
void PIP_Image_Width(unsigned short WX);
//**[34h][35h][36h][37h]**//
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY);
//**[38h][39h][3Ah][3Bh]**//
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY);


// ===========================================
// BLOCK TRANSFER ENGINE
// ===========================================
//**[90h]**//
void BTE_Enable(void);
void BTE_Disable(void);

void Check_BTE_Busy(void);

void Pattern_Format_8X8(void);
void Pattern_Format_16X16(void);

//[91h]
void BTE_ROP_Code(unsigned char setx);
void BTE_Operation_Code(unsigned char setx);

//[92h]
void BTE_S0_Color_8bpp(void);
void BTE_S0_Color_16bpp(void);
void BTE_S0_Color_24bpp(void);

void BTE_S1_Color_8bpp(void);
void BTE_S1_Color_16bpp(void);
void BTE_S1_Color_24bpp(void);
void BTE_S1_Color_Constant(void);
void BTE_S1_Color_8bit_Alpha(void);
void BTE_S1_Color_16bit_Alpha(void);

void BTE_Destination_Color_8bpp(void);
void BTE_Destination_Color_16bpp(void);
void BTE_Destination_Color_24bpp(void);

//[93h][94h][95h][96h]
void BTE_S0_Memory_Start_Address(unsigned long Addr);	

//[97h][98h]
void BTE_S0_Image_Width(unsigned short WX);	

//[99h][9Ah][9Bh][9Ch]
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[9Dh][9Eh][9Fh][A0h]
void BTE_S1_Memory_Start_Address(unsigned long Addr);
void S1_Constant_color_256(unsigned char temp);
void S1_Constant_color_65k(unsigned short temp);
void S1_Constant_color_16M(unsigned long temp);	

//[A1h][A2h]
void BTE_S1_Image_Width(unsigned short WX);	

//[A3h][A4h][A5h][A6h]
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[A7h][A8h][A9h][AAh]
void BTE_Destination_Memory_Start_Address(unsigned long Addr);	

//[ABh][ACh]
void BTE_Destination_Image_Width(unsigned short WX);	

//[ADh][AEh][AFh][B0h]
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[B1h][B2h][B3h][B4h]
void BTE_Window_Size(unsigned short WX, unsigned short WY);

//[B5h]
void BTE_Alpha_Blending_Effect(unsigned char temp);


// ===========================================
// SERIAL FLASH
// ===========================================
//REG[B6h] Serial flash DMA Controller REG (DMA_CTRL) 
void Start_SFI_DMA(void);
void Check_Busy_SFI_DMA(void);

//REG[B7h] Serial Flash/ROM Controller Register (SFL_CTRL) 
void Select_SFI_0(void);
void Select_SFI_1(void);
void Select_SFI_Font_Mode(void);
void Select_SFI_DMA_Mode(void);
void Select_SFI_24bit_Address(void);
void Select_SFI_32bit_Address(void);
void Select_SFI_Waveform_Mode_0(void);
void Select_SFI_Waveform_Mode_3(void);
void Select_SFI_0_DummyRead(void);
void Select_SFI_8_DummyRead(void);
void Select_SFI_16_DummyRead(void);
void Select_SFI_24_DummyRead(void);
void Select_SFI_Single_Mode(void);
void Select_SFI_Dual_Mode0(void);
void Select_SFI_Dual_Mode1(void);

//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data);
unsigned char SPI_Master_FIFO_Data_Get(void);

//REG[B9h] SPI master Control Register (SPIMCR2) 
void Mask_SPI_Master_Interrupt_Flag(void);
void Select_nSS_drive_on_xnsfcs0(void);
void Select_nSS_drive_on_xnsfcs1(void);
void nSS_Inactive(void);
void nSS_Active(void);
void OVFIRQEN_Enable(void);
void EMTIRQEN_Enable(void);
void Reset_CPOL(void);
void Set_CPOL(void);
void Reset_CPHA(void);
void Set_CPHA(void);

//REG[BAh] SPI master Status Register (SPIMSR)
unsigned char Tx_FIFO_Empty_Flag(void);
unsigned char Tx_FIFO_Full_Flag(void);
unsigned char Rx_FIFO_Empty_Flag(void);
unsigned char Rx_FIFO_full_flag(void);
unsigned char OVFI_Flag(void);
void Clear_OVFI_Flag(void);
unsigned char EMTI_Flag(void);
void Clear_EMTI_Flag(void);

//REG[BB] SPI Clock period (SPIDIV) 
void SPI_Clock_Period(unsigned char temp);

//**[BCh][BDh][BEh][BFh]**//
void SFI_DMA_Source_Start_Address(unsigned long Addr);
//**[C0h][C1h][C2h][C3h]**//
void SFI_DMA_Destination_Start_Address(unsigned long Addr);
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY);
//**[C4h][C5h]**//
void SFI_DMA_Destination_Width(unsigned short WX);
//**[C6h][C7h][C8h][C9h]**//
void SFI_DMA_Transfer_Number(unsigned long Addr);
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY);
//**[CAh][CBh]**//
void SFI_DMA_Source_Width(unsigned short WX);













////////////////////////////////////////////////////////////////////////
////**** [ Function : SDRAM ] ****////
//**[E0h]~[E4h]**//
// �����g�b RA8876_Select_SDRAM();


////////////////////////////////////////////////////////////////////////
////**** [ Function : I2C ] ****////
/*�t�~�إߧ���I2C In/Out ���n��*/
//**[E5h]~[EAh]**//
void RA8876_I2CM_Clock_Prescale(unsigned short WX);
//**[E7h]**//
void RA8876_I2CM_Transmit_Data(unsigned char temp);
//**[E8h]**//
unsigned char RA8876_I2CM_Receiver_Data(void);
//**[E9h]**//
 
void RA8876_I2CM_Read_With_Ack(void);
void RA8876_I2CM_Read_With_Nack(void);
void RA8876_I2CM_Write_With_Start(void);
void RA8876_I2CM_Write(void);
void RA8876_I2CM_Stop(void);
 

//**[EAh]**//
unsigned char RA8876_I2CM_Check_Slave_ACK(void);
unsigned char RA8876_I2CM_Bus_Busy(void);
unsigned char RA8876_I2CM_transmit_Progress(void);
unsigned char RA8876_I2CM_Arbitration(void);



////////////////////////////////////////////////////////////////////////
////**** [ Function : GPIO ] ****////
//[F0h][F1h]
void Set_GPIO_A_In_Out(unsigned char temp);
void Write_GPIO_A_7_0(unsigned char temp);
unsigned char Read_GPIO_A_7_0(void);
//[F2h]
void Write_GPIO_B_7_4(unsigned char temp);
unsigned char Read_GPIO_B_7_0(void);
//[F3h][F4h]
void Set_GPIO_C_In_Out(unsigned char temp);
void Write_GPIO_C_7_0(unsigned char temp);
unsigned char Read_GPIO_C_7_0(void);
//[F5h][F6h]
void Set_GPIO_D_In_Out(unsigned char temp);
void Write_GPIO_D_7_0(unsigned char temp);
unsigned char Read_GPIO_D_7_0(void);
//[F7h][F8h]
void Set_GPIO_E_In_Out(unsigned char temp);
void Write_GPIO_E_7_0(unsigned char temp);
unsigned char Read_GPIO_E_7_0(void);
//[F9h][FAh]
void Set_GPIO_F_In_Out(unsigned char temp);
void Write_GPIO_F_7_0(unsigned char temp);
unsigned char Read_GPIO_F_7_0(void);


////////////////////////////////////////////////////////////////////////
////**** [ Function : Key ] ****////
//**[FBh]~[FFh]**//
//[FBh]
void Long_Key_enable(void);
void Key_Scan_Freg(unsigned char temp);	//set bit2~0 

//[FCh]
void Key_Scan_Wakeup_Function_Enable(void);
void Long_Key_Timing_Adjustment(unsigned char setx);//set bit5~3
unsigned char Numbers_of_Key_Hit(void);

//[FDh][FEh][FFh]
unsigned char Read_Key_Strobe_Data_0(void);
unsigned char Read_Key_Strobe_Data_1(void);
unsigned char Read_Key_Strobe_Data_2(void);




// ======================================================================================
// ---------------------------------- GRAPHICS ------------------------------------------
// ======================================================================================
// ===========================================
// CURSOR CONFIG
// ===========================================
//**[3C]**//
void Enable_Graphic_Cursor(void);
void Disable_Graphic_Cursor(void);
void Select_Graphic_Cursor_1(void);
void Select_Graphic_Cursor_2(void);
void Select_Graphic_Cursor_3(void);
void Select_Graphic_Cursor_4(void);
void Enable_Text_Cursor(void);
void Disable_Text_Cursor(void);
void Enable_Text_Cursor_Blinking(void);
void Disable_Text_Cursor_Blinking(void);

//**[3D]**//
void Blinking_Time_Frames(unsigned char temp);
//**[3E][3Fh]**//
void Text_Cursor_H_V(unsigned short WX,unsigned short HY);
//**[40h][41h][42h][43h]**//
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY);
//**[44]**//
void Set_Graphic_Cursor_Color_1(unsigned char temp);
//**[45]**//
void Set_Graphic_Cursor_Color_2(unsigned char temp);

void Graphic_cursor_initial(void);

// ===========================================
// GEOMETRY PRIMITIVES
// ===========================================
//**[67h]**//
void Start_Line(void);
void Start_Triangle(void);
void Start_Triangle_Fill(void);

//**[68h]~[73h]**//
void Line_Start_XY(unsigned short WX,unsigned short HY);
void Line_End_XY(unsigned short WX,unsigned short HY);
void Triangle_Point1_XY(unsigned short WX,unsigned short HY);
void Triangle_Point2_XY(unsigned short WX,unsigned short HY);
void Triangle_Point3_XY (unsigned short WX,unsigned short HY);
void Square_Start_XY(unsigned short WX,unsigned short HY);
void Square_End_XY(unsigned short WX,unsigned short HY);

//**[76h]**//
void Start_Circle_or_Ellipse(void);
void Start_Circle_or_Ellipse_Fill(void);
void Start_Left_Down_Curve(void);
void Start_Left_Up_Curve(void);
void Start_Right_Up_Curve(void);
void Start_Right_Down_Curve(void);
void Start_Left_Down_Curve_Fill(void);
void Start_Left_Up_Curve_Fill(void);
void Start_Right_Up_Curve_Fill(void);
void Start_Right_Down_Curve_Fill(void);
void Start_Square(void);
void Start_Square_Fill(void);
void Start_Circle_Square(void);
void Start_Circle_Square_Fill(void);

//**[77h]~[7Eh]**//
void Circle_Center_XY(unsigned short WX,unsigned short HY);
void Ellipse_Center_XY(unsigned short WX,unsigned short HY);
void Circle_Radius_R(unsigned short WX);
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY);
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY);


// ===========================================
// FONT
// ===========================================
//**[CCh]**//
void Font_Select_UserDefine_Mode(void);
void CGROM_Select_Internal_CGROM(void);
void CGROM_Select_Genitop_FontROM(void);
void Font_Select_8x16_16x16(void);
void Font_Select_12x24_24x24(void);
void Font_Select_16x32_32x32(void);
void Internal_CGROM_Select_ISOIEC8859_1(void);
void Internal_CGROM_Select_ISOIEC8859_2(void);
void Internal_CGROM_Select_ISOIEC8859_3(void);
void Internal_CGROM_Select_ISOIEC8859_4(void);

//**[CDh]**//
void Enable_Font_Alignment(void);
void Disable_Font_Alignment(void);
void Font_Background_select_Transparency(void);
void Font_Background_select_Color(void);
void Font_0_degree(void);
void Font_90_degree(void);
void Font_Width_X1(void);
void Font_Width_X2(void);
void Font_Width_X3(void);
void Font_Width_X4(void);
void Font_Height_X1(void);
void Font_Height_X2(void);
void Font_Height_X3(void);
void Font_Height_X4(void);

//**[CEh]**//
void GTFont_Select_GT21L16TW_GT21H16T1W(void);
void GTFont_Select_GT23L16U2W(void);
void GTFont_Select_GT23L24T3Y_GT23H24T3Y(void);
void GTFont_Select_GT23L24M1Z(void);
void GTFont_Select_GT23L32S4W_GT23H32S4W(void);
void GTFont_Select_GT20L24F6Y(void);
void GTFont_Select_GT21L24S1W(void);
void GTFont_Select_GT22L16A1Y(void);

//**[CFh]**//
void Set_GTFont_Decoder(unsigned char temp);
//**[D0h]**//
void Font_Line_Distance(unsigned char temp);
//**[D1h]**//
void Set_Font_to_Font_Width(unsigned char temp);

//**[D2h]~[D4h]**//
void Foreground_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Foreground_color_256(unsigned char temp);
void Foreground_color_65k(unsigned short temp);
void Foreground_color_16M(unsigned long temp);

//**[D5h]~[D7h]**//
void Background_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Background_color_256(unsigned char temp);
void Background_color_65k(unsigned short temp);
void Background_color_16M(unsigned long temp);

//**[DBh]~[DEh]**//
void CGRAM_Start_address(unsigned long Addr);

void Show_String(char *str);
void LCD_PutChar(uint x,uint y,uchar c,uint charColor,uint bkColor);
void LCD_DisplayString(uint X,uint Y, char *ptr, uint charColor, uint bkColor);
// ======================================================================================
// ---------------------------------- GRAPHICS ------------------------------------------
// ======================================================================================


void Show_picture(unsigned long numbers,const unsigned short *datap);
void Show_picture1(unsigned long numbers,const unsigned short *datap);

void LCD_Clear(uint Color);
void LCD_SetCursor(uint Xpos, uint Ypos);

void LCD_SetPoint(uint x,uint y,uint point);
void LCD_WriteRAM_Prepare(void);

void Show_picture(unsigned long numbers,const unsigned short *datap);


#endif
