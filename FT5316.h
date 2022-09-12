//FT5316
/* touch panel interface define */

#ifndef FT5316_H
#define FT5316_H

#define PEN FT5316_INT_READ

#include "RA8876.h"
#include "I2C.h"


// порты
#define	FT5316_RSTN_PORT	GPIOD
#define	FT5316_RSTN_PIN		GPIO_Pin_4

#define	FT5316_INT_PORT		GPIOD
#define	FT5316_INT_PIN		GPIO_Pin_5

#define	FT5316_RSTN_RESET		GPIO_ResetBits(FT5316_RSTN_PORT,	FT5316_RSTN_PIN)
#define	FT5316_RSTN_SET			GPIO_SetBits(FT5316_RSTN_PORT,		FT5316_RSTN_PIN)

#define	FT5316_INT_READ			GPIO_ReadInputDataBit(FT5316_INT_PORT,	FT5316_INT_PIN)


#define CONFIG_FT5X0X_MULTITOUCH    //Define the multi-touch
//Touch Status	 
#define Key_Down 0x01
#define Key_Up   0x00 

struct _ts_event
{
    uint    x1;
    uint    y1;
    uint    x2;
    uint    y2;
    uint    x3;
    uint    y3;
    uint    x4;
    uint    y4;
    uint    x5;
    uint    y5;
    uchar   touch_point;
	uchar     Key_Sta;	
};

extern struct _ts_event ts_event; 


#define FT5316_ADDRESS 0x38

#define WRITE_ADD	0x70
#define READ_ADD	0x71


void TOUCH_Init(void);
void TOUCH_Wr_Reg(uchar RegIndex, uchar RegValue1);
uchar   TOUCH_Read_Reg(uchar RegIndex);
void TOUCH_RdParFrPCTPFun(uchar *PCTP_Par,uchar ValFlag);
uchar ft5x0x_read_data(void);
struct _ts_event touchGetPoint(void);


#endif
