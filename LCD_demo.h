
#include "RA8876.h"
#include "FT5316.h"


#define layer1_start_addr 0 		 
#define layer2_start_addr 1228800   //1024*600*2 
#define layer3_start_addr 2457600   //1024*600*2*2
#define layer4_start_addr 3686400   //1024*600*2*3
#define layer5_start_addr 4915200   //1024*600*2*4
#define layer6_start_addr 6144000   //1024*600*2*5


void Text_Demo(void);
void App_Demo_Alpha_Blending(void);
void PIP_Demo(void);
void App_Demo_Scrolling_Text(void);
void App_Demo_Waveform(void);
void App_Demo_slide_frame_buffer(void);
void BTE_Compare(void);
void BTE_Color_Expansion(void);
void BTE_Pattern_Fill(void);
void gray(void);
void Geometric(void);
void TPTEST(void);
void Gradient_Demo(void);
