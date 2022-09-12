
#include "FT5316.h"
#include "stm32f4xx_gpio.h"


// сброс контроллера
static void FT5316_reset(void)
{
	FT5316_RSTN_RESET;
	delay_ms(5);
	FT5316_RSTN_SET;
	delay_ms(350);
}


void TOUCH_Init(void)
{
  FT5316_reset();
}


void TOUCH_Wr_Reg(uchar RegIndex, uchar RegValue1)
{  
  I2Cx_writeData(I2C1, FT5316_ADDRESS, RegIndex, RegValue1);
  
	delay_us(100);
}


uchar TOUCH_Read_Reg(uchar RegIndex)
{
  uchar receive=0;
 	
  receive = I2Cx_readData(I2C1, FT5316_ADDRESS, RegIndex);
	return receive;
}


void TOUCH_RdParFrPCTPFun(uchar *PCTP_Par,uchar ValFlag)
{
  uchar k;
  
  I2Cx_readDataBurst(I2C1, FT5316_ADDRESS, 0x00, ValFlag, PCTP_Par);
}


uchar ft5x0x_read_data(void)
{
  uchar buf[32] = {0}; uchar ret = 0;

	#ifdef CONFIG_FT5X0X_MULTITOUCH
		TOUCH_RdParFrPCTPFun(buf, 31);
	#else
  		TOUCH_RdParFrPCTPFun(buf, 7);
	#endif

  ts_event.touch_point = buf[2] & 0xf;

  if (ts_event.touch_point == 0) 
	{
		return 0;
  }

	#ifdef CONFIG_FT5X0X_MULTITOUCH
	switch (ts_event.touch_point) 
	{
    case 5:
      ts_event.x5 = (uint)(buf[0x1b] & 0x0F)<<8 | (uint)buf[0x1c];
      ts_event.y5 = (uint)(buf[0x1d] & 0x0F)<<8 | (uint)buf[0x1e];
    
    case 4:
      ts_event.x4 = (uint)(buf[0x15] & 0x0F)<<8 | (uint)buf[0x16];
      ts_event.y4 = (uint)(buf[0x17] & 0x0F)<<8 | (uint)buf[0x18];
    
    case 3:
      ts_event.x3 = (uint)(buf[0x0f] & 0x0F)<<8 | (uint)buf[0x10];
      ts_event.y3 = (uint)(buf[0x11] & 0x0F)<<8 | (uint)buf[0x12];
    
    case 2:
      ts_event.x2 = (uint)(buf[9] & 0x0F)<<8 | (uint)buf[10];
      ts_event.y2 = (uint)(buf[11] & 0x0F)<<8 | (uint)buf[12];
    
    case 1:
      ts_event.x1 = (uint)(buf[3] & 0x0F)<<8 | (uint)buf[4];
      ts_event.y1 = (uint)(buf[5] & 0x0F)<<8 | (uint)buf[6];
      break;
    
    default:
      return 0;
	}
	#else
	if (ts_event.touch_point == 1)
	{
	 	ts_event.x1 = (uint)(buf[3] & 0x0F)<<8 | (uint)buf[4];
	 	ts_event.y1 = (uint)(buf[5] & 0x0F)<<8 | (uint)buf[6];
	 	ret = 1;
	}
	else
	{
    s_event.x1 = 0xFFFF;
	 	ts_event.y1 = 0xFFFF;
	 	ret = 0;
	}
	#endif
  
	return ret;
}


void  counter0(void)
{
 	if(PEN==0)										//Detect the occurrence of an interrupt
 	{
		ts_event.Key_Sta=Key_Down;                              

 	}
}


struct _ts_event touchGetPoint(void)
{
  while(ts_event.Key_Sta!=Key_Down)
  {
    counter0();
  }
  ts_event.Key_Sta=Key_Up;
  ft5x0x_read_data();
  return ts_event;
}
