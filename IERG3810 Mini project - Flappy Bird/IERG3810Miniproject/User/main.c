#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"
#include "global.h"
#include "IERG3810_LCD.h"
#include "bird.h"
#include "stdlib.h"
#define DS0_ON GPIOB->BRR |= 1<<5;
#define DS0_OFF GPIOB->BSRR |= 1<<5;
#define DS1_ON GPIOE->BRR |= 1<<5;
#define DS1_OFF GPIOE->BSRR |= 1<<5;
u8 task1HeartBeat = 0;
u8 task2HeartBeat = 0;
void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}
void draw_bottom_column(int x, u16 color, int bottom_level){
	/*draw bottom column
	  x: location
	  bottom_level: specify the height of the column (0~9)*/
	
	  int i1, i2, i3;
	  u16 width = 30;
	  u16 height = 40 + bottom_level * 10; //minimum height: 40
	
  // draw the green column	
	  IERG3810_TFTLCD_FillRectangle(color,x, width, 0, height);
		IERG3810_TFTLCD_FillRectangle(color,x - 5, width+10, height+2, 10);
	
	// draw the black boundary  
	  for (i1 = 0; i1 < height; i1++)
		{
			IERG3810_TFTLCD_DrawDot(x-2, i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-1, i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width, i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+1, i1, 0x0000);
		}
		for (i2 = 0; i2 < 11; i2++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-1-5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+6, height+i2+1, 0x0000);			
		}
		for (i3 = 0; i3 < 44; i3++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, height, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, height+12, 0x0000);	
		}
}

void draw_top_column(int x, u16 color, int top_level){
	/*draw bottom column
	  x: location
	  top_level: specify the height of the column (0~9)*/
	
		int i1, i2, i3;
	  u16 width = 30;
	  u16 height = 40 + top_level * 10;
	
    // draw the green column	
	  IERG3810_TFTLCD_FillRectangle(color,x, width, 320-height, height);
		IERG3810_TFTLCD_FillRectangle(color,x - 5, width+10, 309-height, 10);
	
		// draw the black boundary  
	  for (i1 = 0; i1 < height; i1++)
		{
			IERG3810_TFTLCD_DrawDot(x-2, 319-i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-1, 319-i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width, 319-i1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+1, 319-i1, 0x0000);
		}
		for (i2 = 0; i2 < 11; i2++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5, 319-(height+i2+1), 0x0000);
			IERG3810_TFTLCD_DrawDot(x-1-5, 319-(height+i2+1), 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+5, 319-(height+i2+1), 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+6, 319-(height+i2+1), 0x0000);			
		}
		for (i3 = 0; i3 < 44; i3++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, 319-height, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, 319-(height+12), 0x0000);	
		}
}

void draw_column(int x, u16 color, int bottom_level, int top_level){
    //draw the whole column
	  //bottom_level + top_level should less than 18 to let the bird fly through
	  //green:  0x07E0
		draw_bottom_column(x,color, bottom_level);
		draw_top_column(x,color, top_level);
}

/*J: This function draws a bird with the bottom left corner position specified by (start_x,start_y)
     start_x and start_y are decimal numbers
mode specify the mode of bird, 0: flying, 1: falling down*/
void draw_bird_dot(int start_x, int start_y,u8 mode){
	u16 i,j;
	u16 position_x, position_y;
	IERG3810_TFTLCD_Init();
	
	for(i=0; i<12; i++){
		for(j=0; j < 17; j++){
			if(mode == 0){
				position_x = start_x + j;
				position_y = start_y + i;
			}
			if(mode == 1){
				position_x = start_x + i;
				position_y = start_y + 16- j;
			}
			switch(bird_basic[i][j]){
				case 0x00:
				{
					break;
				}
				case 0x01:
				{
					IERG3810_TFTLCD_DrawDot(position_x, position_y,0x0000);
					break;
				}
				case 0x02:
				{
					IERG3810_TFTLCD_DrawDot(position_x, position_y,0xFFFF);
					break;
				}
				case 0x03:
				{
					IERG3810_TFTLCD_DrawDot(position_x, position_y,0xFF20);
					break;
				}
				case 0x04:
				{
					IERG3810_TFTLCD_DrawDot(position_x, position_y,0xED61);
					break;
				}
				case 0x05:
				{
					IERG3810_TFTLCD_DrawDot(position_x, position_y,0xFB40);
					break;
				}
			}
		}
	}
}

void draw_bird_rectangle(int start_x, int start_y,u16 length,u8 mode){
	int i,j;
	int position_x, position_y;
	IERG3810_TFTLCD_Init();
	
	for(i=0; i<12; i++){
		for(j=0; j < 17; j++){
			if(mode == 0){
				position_x = start_x + j*length;
				position_y = start_y + i*length;
			}
			if(mode == 1){
				position_x = start_x + i*length;
				position_y = start_y + (16- j)*length;
			}
			switch(bird_basic[i][j]){
				case 0x00:
				{
					break;
				}
				case 0x01:
				{
					IERG3810_TFTLCD_FillRectangle(0x0000,position_x,length, position_y,length);
					break;
				}
				case 0x02:
				{
					IERG3810_TFTLCD_FillRectangle(0xFFFF,position_x,length, position_y,length);
					break;
				}
				case 0x03:
				{
					IERG3810_TFTLCD_FillRectangle(0xFF20,position_x,length, position_y,length);
					break;
				}
				case 0x04:
				{
					IERG3810_TFTLCD_FillRectangle(0xED61,position_x,length, position_y,length);
					break;
				}
				case 0x05:
				{
					IERG3810_TFTLCD_FillRectangle(0xFB40,position_x,length, position_y,length);
					break;
				}
			}
		}
	}
	
}

void IERG3810_SYSTICK_Init_10ms(void){
	//SYSTICK
	SysTick->CTRL = 0; //Clear Systick->CTRL setting
	SysTick->LOAD = 89999; //what should be filled? refer to 0337E page 8-10
	                       //72M / (8*10000) - 1  
	                       //0.1ms -> 10000 Hz
	
	//CLKSOURCE=0 : STCLK (FCLK/8)
	//CLKSOURCE = 1: FCLK
	//CLKSOURCE=0 is synchronized and better than CLKSROUCE = 1
	//Refer to Clock tree on page-93 of RM0008
	SysTick->CTRL |= 0x03; //what should be filled?
	                      //refer to 033tE page 8-8
}

int main(void){
	  int bird_x = 40;
	  int bird_y = 138;
	  int column1_x = 240;
    int column2_x = column1_x+120;
	  int column3_x = column1_x+240;
		int levels[6]= {0,0,0,0,0,0};
		int i;
		for (i = 0; i < 5; i++){
			levels[i] = rand()%9;
		}	  			
    IERG3810_TFTLCD_Init();
		IERG3810_TFTLCD_SetParameter();
	  IERG3810_clock_tree_init();
		IERG3810_LED_Init();
  	IERG3810_SYSTICK_Init_10ms();

	  Delay(1000000);
	  //IERG3810_TFTLCD_FillRectangle(u16 color,u16 start_x, u16 length_x, u16 start_y, u16 length_y);
	  IERG3810_TFTLCD_FillRectangle(0x0000,0, 240, 0, 320);
	  while (1) {

			if(task1HeartBeat > 1){
				IERG3810_TFTLCD_FillRectangle(0x0000,bird_x, 34, bird_y, 28);			
				GPIOB->ODR ^= 1<<5;
				bird_y -= 2; 
				
				if(bird_y+24 < 0)
					bird_y = 320;
				//IERG3810_TFTLCD_FillRectangle(0x0000,bird_x, 34, bird_y, 24);			
				draw_bird_rectangle(bird_x,bird_y,2,0);
				task1HeartBeat = 0;
			}
			if(task2HeartBeat > 2){
				//IERG3810_TFTLCD_FillRectangle(0x07FF,column1, 40, 0, 320);

				column1_x -= 2;
				column2_x -= 2;
				column3_x -= 2;
				if(column1_x+44 < 0)
					column1_x = 240;
				if(column2_x+44 < 0)
					column2_x = column1_x+120;
				if(column3_x+44 < 0)
					column3_x = column1_x+240;			  
				draw_column(column1_x,0x07E0,levels[0],levels[1]);
				draw_column(column2_x,0x07E0,levels[2],levels[3]);
				draw_column(column3_x,0x07E0,levels[4],levels[5]);
				task2HeartBeat = 0;

			}		  
		
    }

}

