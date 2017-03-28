#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"
#include "global.h"
#include "IERG3810_LCD.h"
#include "bird.h"

#define DS0_ON GPIOB->BRR |= 1<<5;
#define DS0_OFF GPIOB->BSRR |= 1<<5;
#define DS1_ON GPIOE->BRR |= 1<<5;
#define DS1_OFF GPIOE->BSRR |= 1<<5;
u8 task1HeartBeat = 0;
u8 task2HeartBeat = 0;

void draw_bottom_column(u16 x, u16 color, int bottom_level){
	/*draw bottom column
	  x: location
	  bottom_level: specify the height of the column (0~9)*/
	
	  int i1, i2, i3;
	  u16 width = 20;
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
		for (i2 = 0; i2 < 10; i2++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-1-5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+5, height+i2+1, 0x0000);
			IERG3810_TFTLCD_DrawDot(x+width+6, height+i2+1, 0x0000);			
		}
		for (i3 = 0; i3 < 34; i3++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, height, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, height+12, 0x0000);	
		}
}

void draw_top_column(u16 x, u16 color, int top_level){
	/*draw bottom column
	  x: location
	  top_level: specify the height of the column (0~9)*/
	
		int i1, i2, i3;
	  u16 width = 20;
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
		for (i3 = 0; i3 < 34; i3++)
		{
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, 319-height, 0x0000);
			IERG3810_TFTLCD_DrawDot(x-2-5+i3, 319-(height+12), 0x0000);	
		}
}

void draw_column(u16 x, u16 color, int bottom_level, int top_level){
    //draw the whole column
	  //bottom_level + top_level should less than 18 to let the bird fly through
	  //green:  0x07E0
		draw_bottom_column(x,color, bottom_level);
		draw_top_column(x,color, top_level);
}

/*J: This function draws a bird with the bottom left corner position specified by (start_x,start_y)
     start_x and start_y are decimal numbers
mode specify the mode of bird, 0: flying, 1: falling down*/
void draw_bird_dot(u16 start_x, u16 start_y,u8 mode){
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

void draw_bird_rectangle(u16 start_x, u16 start_y,u16 length,u8 mode){
	u16 i,j;
	u16 position_x, position_y;
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
int main(void){
    IERG3810_TFTLCD_Init();
		IERG3810_TFTLCD_SetParameter();
	  while (1) {
			draw_column(20,0x07E0,8,9);
		  draw_bird_rectangle(20,148,2,0);
    }

}

