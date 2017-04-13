#include "stm32f10x.h"
#include "IERG3810_DrawColumn.h"
#include "IERG3810_LCD.h"

void draw_bottom_column(int x, u16 color, int bottom_level){
	/*draw bottom column
	  x: location
	  bottom_level: specify the height of the column (0~9)*/
	
	  int i1, i2, i3;
	  u16 width = 30;
	  u16 height = 40 + bottom_level * 10; //minimum height: 40
	
  // draw the green column	
	  IERG3810_TFTLCD_FillRectangle(color,x, width, 0, height); // the main part of the column 
		IERG3810_TFTLCD_FillRectangle(color,x - 5, width+10, height+2, 10); // the "wider part of the column" -> for asthetic purpose
	
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

void clear_column(int x, int bottom_level, int top_level){
	
	draw_bottom_column(x,0x0000, bottom_level);
	draw_top_column(x,0x0000, top_level);
	
}

int level_to_y(int level , int is_top) //transform level to "y- coordinate value"
{
	
	//top_or_bottom = 0 : bottom
	//top_or_bottom = 1 : top
	
	int height = 40 + level * 10;
	if(is_top == 1) // for top column
	{
		return 320-height;
		
	}
  else{ //for bottom column
		
		return height;
		
	}
	
}

/*
int coordinate_to_all_corners(int x, int y, int is_top,int is_main){ //0: bottom 1:top , 1: main body of the column 0: wider part of the column
	
	if(is_top&&is_main){
		
		
	}
	else if(!is_top && is_main){
		
		
	}
	else if(is_top && !is_main){
		
		
	}
	else{
		
		
	}
	
}*/

void clear_column_part(int old_column_x, int column_move_speed, int bottom_level, int top_level){
	
	 int temp_old_x = old_column_x;
	
	 int new_x = temp_old_x - column_move_speed;
	

	
	 int top_y = level_to_y(top_level,1) - 10;
	 int bottom_y = level_to_y(bottom_level,0)+ 10;
		 
	 int length_x = (temp_old_x + 35) - (new_x + 30);
	 int length_y_top = 320 - top_y;
	 int length_y_bottom = bottom_y;
	
	 int start_x = new_x + 30;
	
	  
	 if(length_x > 0){
		 IERG3810_TFTLCD_FillRectangle(0x0000,start_x, length_x+1, top_y-1, length_y_top+1);
		 IERG3810_TFTLCD_FillRectangle(0x0000,start_x, length_x+1, 0, length_y_bottom+2);
		 
	 }
}


