#include "stm32f10x.h"
#include "bird.h"
#include "IERG3810_LCD.h"

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

