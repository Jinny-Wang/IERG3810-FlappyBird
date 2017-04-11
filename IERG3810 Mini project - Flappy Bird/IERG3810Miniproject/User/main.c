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

/*Global Variable -> Store the position of the bird*/
int bird_x = 0; 
int bird_y = 0; 

/*Global variable -> for PS2 keyboard*/
int ps2count=0;
u8 ps2key;
u8 ps2key_previous=0x00;

/*global variable -> indicate difficulty level*/
int difficulty_level = 1;

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
	                       //72M / (8*100) - 1  
	                       //10 ms -> 100 Hz
	
	//CLKSOURCE=0 : STCLK (FCLK/8)
	//CLKSOURCE = 1: FCLK
	//CLKSOURCE=0 is synchronized and better than CLKSROUCE = 1
	//Refer to Clock tree on page-93 of RM0008
	SysTick->CTRL |= 0x03; //what should be filled?
	                      //refer to 033tE page 8-8
}

/*PS2 keyboard initialization*/
void IERG3810_PS2key_ExtiInit(){
	
	RCC-> APB2ENR |= 1<<4; //enable port C
	GPIOC->CRH &= 0xFFFF00FF; // enable the clock of port C , pin 10 and 11
	GPIOC->CRH |= 0x00008800; // set max clock rate of PC10 and PC11 to 50 MHz, Input with pull-up pull-down
	
	//GPIOA->ODR |= 0; //unpressed
	
	/*Enable AFIO*/
	RCC->APB2ENR |=0X01;
	AFIO->EXTICR[2] &= 0XFFFF0FFF; // Set the 4th byte (count from LSB)of AFIO_EXTICR3 to 0 :EXTI-11 
	AFIO->EXTICR[2] |= 0x00002000; // select port C 
 	EXTI->IMR |= 1<<11; //Interrupt request from line11 is not masked 
	EXTI->FTSR |= 1<<11;
	
	NVIC->IP[40] = 0X35; //SET PRIORITY OF THIS INTERRUPT.
	NVIC->ISER[1] |= (1<<8);//Enable  NVIC IRQ#40  ISER[1] : IERG 32 to 63
}

//EXTI-11 handler for PS2keyboard CLK.
void EXTI15_10_IRQHandler(void){ //handler for EXTI10 to EXTI15
	//students design program here.
	if(ps2count>0 && ps2count<9)
	{
		ps2key |= ((GPIOC->IDR & 0x00000400) >> 10) << (ps2count-1); 
	}
	ps2count++;
	Delay(10);
	//A short delay caneliminate the error.
	EXTI->PR = 1<<11; //Clear this exception pending bit.
}

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup){
	//set prigroup AIRCR[10:8]
	u32 temp, temp1;
	temp1 = prigroup & 0x00000007; // only concern 3 bits
	temp1 <<= 8; //why?
	temp = SCB->AIRCR;
	temp &= 0X0000F8FF;
  temp = 0x05FA0000;
	temp |= temp1;
	SCB->AIRCR = temp;
}

void bird_fall(int step){
	
	if(task1HeartBeat > 1){
				IERG3810_TFTLCD_FillRectangle(0x0000,bird_x, 34, bird_y, 28);			
				
				bird_y -= step; 
				
				if(bird_y+24 < 0) // if the bird touches the bottom of the screen 
					bird_y = 320;  // move the bird to the top again
				//IERG3810_TFTLCD_FillRectangle(0x0000,bird_x, 34, bird_y, 24);			
				draw_bird_rectangle(bird_x,bird_y,2,0);
				task1HeartBeat = 0;
			}
}

void keyboard_control_bird(int timeout , int up_speed){
	/*use keyboard to control the bird*/
			if(ps2count >= 11)
			{
				//EXTI->IMR &= ~(1<<11); optional, suspend interrupt
				//--student design program here
				if(ps2key_previous != 0xF0 && ps2key == 0x79){ //0x79  : '+'
					IERG3810_TFTLCD_FillRectangle(0x0000,bird_x, 34, bird_y, 28);	
					bird_y += up_speed;
					draw_bird_rectangle(bird_x,bird_y,2,0);
					
				}
				else if(ps2key_previous == 0xF0 && ps2key==0x79)
					DS0_OFF;
				
			
				ps2key_previous = ps2key;
				ps2count=0;
				ps2key=0;
				//EXTI->PR = 1 << 11; //Clear this exception pending bit
				//EXTI->IMR |=(1<11) ; optional, resume interrupt
			}//end of "if PS2 keyboard received data correctly"
			timeout--;
		
			if(timeout == 0)//clear PS2 keyboard data when timeout
			{
				timeout = 20000;
				ps2key=0;
				ps2count=0;
	
			}//end of "clear PS2 keyboard data when timeout"
	
}

void Initial_ui(u16 bgcolor,int timeout){
	
	char sid1[10] = "1155076727";
		char sid2[10] = "1155076730";
		char game_name[11] = "Flappy Bird";
		char difficulty_level1[17] = "Press 1 2 or 3 to";
		char difficulty_level2[26] = "choose a difficulty level.";
		char instruction1[20] = "Press Enter to Start";
		char instruction2[27] = "Press + to Let the Bird Fly";
		
		IERG3810_TFTLCD_FillRectangle(bgcolor,0x0000,0x00F0,0,0x0140); //Blue Background for the welcome page
		
		//Show "Flappy Bird"
		IERG3810_TFTLCD_ShowCharInLine(70,280,game_name,11,0xFC00,bgcolor);
		
		//Show instructions for difficulty level
		IERG3810_TFTLCD_ShowCharInLine(10,240,difficulty_level1,17,0x0000,bgcolor);
		IERG3810_TFTLCD_ShowCharInLine(10,220,difficulty_level2,26,0x0000,bgcolor);
		
		//Show Difficulty Level blocks
		IERG3810_TFTLCD_FillRectangle(0xFFFF,20,60,140,60);
		IERG3810_TFTLCD_FillRectangle(0xFFFF,90,60,140,60);
		IERG3810_TFTLCD_FillRectangle(0xFFFF,160,60,140,60);
		IERG3810_TFTLCD_ShowChar(44,164,0x31,0x0000,0xFFFF);
		IERG3810_TFTLCD_ShowChar(114,164,0x32,0x0000,0xFFFF);
		IERG3810_TFTLCD_ShowChar(184,164,0x33,0x0000,0xFFFF);
		
		//Show control instruction
		IERG3810_TFTLCD_ShowCharInLine(10,100,instruction1,20,0x0000,bgcolor);
		IERG3810_TFTLCD_ShowCharInLine(10,80,instruction2,27,0x0000,bgcolor);
		
		//Show names and SIDs
		IERG3810_TFTLCD_ShowCharInLine(0x0050,0x0010,sid1,0x000A,0x0000,bgcolor);
		IERG3810_TFTLCD_ShowCharInLine(0x0050,0x0020,sid2,0x000A,0x0000,bgcolor); 
		IERG3810_TFTLCD_ShowChinChar(0x40,0x10,0x00,0x0000,bgcolor);
		IERG3810_TFTLCD_ShowChinChar(0x40,0x20,0x01,0x0000,bgcolor);
		
	while(1){	
		
			if(ps2count >= 11)
			{
				//EXTI->IMR &= ~(1<<11); optional, suspend interrupt
				//--student design program here
				if(ps2key_previous != 0xF0 && ps2key == 0x69){ //0x69  : '1'
					
					difficulty_level = 1;
					IERG3810_TFTLCD_FillRectangle(bgcolor,0, 240, 139, 62);
					IERG3810_TFTLCD_FillRectangle(0x0410,20,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,90,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,160,60,140,60);
					IERG3810_TFTLCD_ShowChar(44,164,0x31,0xFFFF,0x0410);
					IERG3810_TFTLCD_ShowChar(114,164,0x32,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(184,164,0x33,0x0000,0xFFFF);
					
				}
				
				if(ps2key_previous != 0xF0 && ps2key == 0x72){ //0x72  : '2'
					
					difficulty_level = 2;
					IERG3810_TFTLCD_FillRectangle(bgcolor,0, 240, 139, 62);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,20,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0x0410,90,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,160,60,140,60);
					IERG3810_TFTLCD_ShowChar(44,164,0x31,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(114,164,0x32,0xFFFF,0x0410);
					IERG3810_TFTLCD_ShowChar(184,164,0x33,0x0000,0xFFFF);
					
				}
				if(ps2key_previous != 0xF0 && ps2key == 0x7A){ //0x7A  : '3'
					
					difficulty_level = 3;
					IERG3810_TFTLCD_FillRectangle(bgcolor,0, 240, 139, 62);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,20,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,90,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0x0410,160,60,140,60);
					IERG3810_TFTLCD_ShowChar(44,164,0x31,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(114,164,0x32,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(184,164,0x33,0xFFFF,0x0410);
					
				}
				
				if(ps2key_previous != 0xF0 && ps2key == 0x5A){ //0x5A  : 'Enter'
					
					IERG3810_TFTLCD_FillRectangle(0x0000,0, 240, 0, 320);
					DS1_ON;
					break;
					
				}
				
				
			
				ps2key_previous = ps2key;
				ps2count=0;
				ps2key=0;
				//EXTI->PR = 1 << 11; //Clear this exception pending bit
				//EXTI->IMR |=(1<11) ; optional, resume interrupt
			}//end of "if PS2 keyboard received data correctly"
			timeout--;
		
			if(timeout == 0)//clear PS2 keyboard data when timeout
			{
				timeout = 20000;
				ps2key=0;
				ps2count=0;
			}//end of "clear PS2 keyboard data when timeout"
			
			
		Delay(1000000);
	}
	return;
	
}

void count_down(int start){
			u8 i;
		  for(i=start+1;i > 0; i--){
				 IERG3810_TFTLCD_DrawDigit(0x0000, 0x50, 0x50, i);
				 Delay(5000000);
				 IERG3810_TFTLCD_FillRectangle(0xFFFF,0x0000,0x00F0,0,0x0140);
			}
			IERG3810_TFTLCD_FillRectangle(0x0000,0, 240, 0, 320);
	
	
}
int main(void){
	  
	  int bird_fall_speed,column_move_speed, bird_fly_speed;
	  int timeout = 20000;
	  int column1_x = 240;
    int column2_x = column1_x+120;
	  int column3_x = column1_x+240;
		int levels[6]= {0,0,0,0,0,0};
		int i;
		bird_x = 40;
	  bird_y = 138;
		
		for (i = 0; i < 5; i++){
			levels[i] = rand()%9;
		}	  			
    IERG3810_TFTLCD_Init();
		IERG3810_TFTLCD_SetParameter();
	  IERG3810_clock_tree_init();
		IERG3810_LED_Init();
  	IERG3810_SYSTICK_Init_10ms();
    IERG3810_PS2key_ExtiInit();
		IERG3810_NVIC_SetPriorityGroup(5); // 5: four preemty priority levels
																		// 6: two preemty priority levels
		
	  Delay(1000000); //delay to enable some time for the initialization process above 
	  //IERG3810_TFTLCD_FillRectangle(u16 color,u16 start_x, u16 length_x, u16 start_y, u16 length_y);
	  
	 switch(difficulty_level){
		 case(1):
			 bird_fall_speed = 2;
		   column_move_speed = 2;
  		 bird_fly_speed = 18;
		   break;
		 case(2):
			 bird_fall_speed = 4;
		   column_move_speed = 4;
  		 bird_fly_speed = 28;
       break;		 
		 case(3):
			 bird_fall_speed = 8;
		   column_move_speed = 8;
  		 bird_fly_speed = 38;
		   break;
		 
	 }
		while(1){

			//Initialization -> Show name, SID, choose level, show instruction
			Initial_ui(0x87FF,timeout); //Light blue background
			
			switch(difficulty_level){
			 case(1):
				 bird_fall_speed = 1;
				 column_move_speed = 1;
				 bird_fly_speed = 16;
				 break;
			 case(2):
				 bird_fall_speed = 2;
				 column_move_speed = 2;
				 bird_fly_speed = 18;
				 break;		 
			 case(3):
				 bird_fall_speed = 3;
				 column_move_speed = 3;
				 bird_fly_speed = 20;
				 break;
		 
			}
			count_down(3);
			
			//The game body
			while (1) {
				 
				keyboard_control_bird(timeout,bird_fly_speed);
				bird_fall(bird_fall_speed);
				if(task2HeartBeat > 2){
					//IERG3810_TFTLCD_FillRectangle(0x07FF,column1, 40, 0, 320);

					column1_x -= column_move_speed;
					column2_x -= column_move_speed;
					column3_x -= column_move_speed;
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

}

