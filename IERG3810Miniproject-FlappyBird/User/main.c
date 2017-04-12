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
#include "IERG3810_DrawColumn.h"

#define DS0_ON GPIOB->BRR |= 1<<5;
#define DS0_OFF GPIOB->BSRR |= 1<<5;
#define DS1_ON GPIOE->BRR |= 1<<5;
#define DS1_OFF GPIOE->BSRR |= 1<<5;
u8 task1HeartBeat = 0;
u8 task2HeartBeat = 0;

/*Global Variable -> Store the position of the bird*/
int bird_x = 0; 
int bird_y = 0; 
int column1_x = 0;
int column2_x = 0;
int column3_x = 0;
int score = 0;
int levels[6]= {0,0,0,0,0,0};
/*Global variable -> for PS2 keyboard*/
int ps2count=0;
u8 ps2key;
u8 ps2key_previous=0x00;

/*global variable -> indicate difficulty level*/
int difficulty_level = 1;

/*global variable -> record the status of the column*/
int column[320][34];



void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}


void IERG3810_TIM3_Init(u16 arr,u16 psc){
	//TIM3, IRQ#29
	RCC->APB1ENR |= 1<<1; //RM0008 page-111, refer to lab-1 enable APB1 (Clock is set to 36MHz in IERG3810_clock_tree_init(); )
	TIM3->ARR = arr;  //RM0008 page 403 set TIM3 auto-reload register to 4999 -> divide the original clock rate by 5000,since count from 0 
	TIM3->PSC=psc; //RM0008 page-402,403 set TIM3 prescaler to 7199 -> divide the original clock rate by 7200 (7199+1, since count from 0)
	TIM3->DIER |= 1<<0; //RM0008 page-393 UPDATE interrupt enabled
	TIM3->CR1 |= 0x01; //RM0008 page-388  Counter enabled 
	NVIC->IP[29] = 0x45; //refer to lab-4
	NVIC->ISER[0] |= (1<<29); //refer to lab-4

}
void TIM3_IRQHandler(void){
	
	
	//code for 5-1
	if(TIM3->SR & 1<<0)//RM0008 page-395 When update interrupt pending occur
	{
		 GPIOE->ODR ^= 1<<5;//toggle DS0 with read-modify-write
	}
	TIM3->SR &= ~(1<<0);//RM0008 page-395 clear the interrupt pending bit
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
				
				//if(bird_y+24 < 0) // if the bird touches the bottom of the screen 
					//bird_y = 320;  // move the bird to the top again
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
				//else if(ps2key_previous == 0xF0 && ps2key==0x79)
				
				
			
				ps2key_previous = ps2key;
				ps2count=0;
				ps2key=0;
				//EXTI->PR = 1 << 11; //Clear this exception pending bit
				//EXTI->IMR |=(1<11) ; optional, resume interrupt
			}//end of "if PS2 keyboard received data correctly"
			/*timeout--;
		
			if(timeout == 0)//clear PS2 keyboard data when timeout
			{
				timeout = 20000;
				ps2key=0;
				ps2count=0;
	
			}//end of "clear PS2 keyboard data when timeout"*/
	
}

void Initial_ui(u16 bgcolor,int timeout){
	 int i,m,n;
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
		
		
		/*position parameters initialization*/
		
		for (i = 0; i < 6; i++){
				levels[i] = rand()%9;
		}
		for (m=0;m<34;m++){
					  for (n=0;n<320;n++){
						  column[n][m]=0;
					  }
		}
		
	  column1_x = 240;
    column2_x = column1_x+100;
		column3_x = column1_x+200;
    bird_x = 40;
	  bird_y = 138;		
		score = 0;
		DS0_OFF;
	while(1){	
		
			if(ps2count >= 11)
			{
				//EXTI->IMR &= ~(1<<11); optional, suspend interrupt
				//--student design program here
				if(ps2key_previous == 0xF0 && ps2key == 0x69){ //0x69  : '1'
					
					difficulty_level = 1;
					IERG3810_TFTLCD_FillRectangle(bgcolor,0, 240, 139, 62);
					IERG3810_TFTLCD_FillRectangle(0x0410,20,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,90,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,160,60,140,60);
					IERG3810_TFTLCD_ShowChar(44,164,0x31,0xFFFF,0x0410);
					IERG3810_TFTLCD_ShowChar(114,164,0x32,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(184,164,0x33,0x0000,0xFFFF);
					
				}
				
				if(ps2key_previous == 0xF0 && ps2key == 0x72){ //0x72  : '2'
					
					difficulty_level = 2;
					IERG3810_TFTLCD_FillRectangle(bgcolor,0, 240, 139, 62);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,20,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0x0410,90,60,140,60);
					IERG3810_TFTLCD_FillRectangle(0xFFFF,160,60,140,60);
					IERG3810_TFTLCD_ShowChar(44,164,0x31,0x0000,0xFFFF);
					IERG3810_TFTLCD_ShowChar(114,164,0x32,0xFFFF,0x0410);
					IERG3810_TFTLCD_ShowChar(184,164,0x33,0x0000,0xFFFF);
					
				}
				if(ps2key_previous == 0xF0 && ps2key == 0x7A){ //0x7A  : '3'
					
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
					//DS1_ON;
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
			
			
		Delay(100);
	}
	return;
	
}

int num_of_digit(int num){
	int count = 0;
	if (num == 0) return 1;
	
	while(num != 0){
		num /= 10;
		count++;
	}
	return count;
}
void game_over(u16 bgcolor){
	  int i;
		char gameover[10] = "GAME OVER!";
		char score_display[16] = "Your Score:     ";
		char instruction[16] = "Press NumLock to";
		char instruction2[12] = "Start Agian.";
	
	  int score_length = num_of_digit(score);
	  int temp_score = score;
	  //score_display[15] = score_length + '0';
	
	  for(i = 0; i < score_length; i++){

				char digit = temp_score%10 + '0';
			  score_display[15-i] = digit;
			  temp_score /= 10;	
	 }
	
	  
		IERG3810_TFTLCD_FillRectangle(bgcolor,30,180,70,180);
		IERG3810_TFTLCD_ShowCharInLine(40,180,gameover,10,0xF800,bgcolor);
		
		IERG3810_TFTLCD_ShowCharInLine(35,150,score_display,16,0x0000,bgcolor);
		
		IERG3810_TFTLCD_ShowCharInLine(35,100,instruction,16,0x0000,bgcolor);
		IERG3810_TFTLCD_ShowCharInLine(35,80,instruction2,12,0x0000,bgcolor);
	
		while(1){
		
		
		if(ps2count >= 11)
			{
				
				if(ps2key_previous != 0xF0 && ps2key == 0x77){ //0x77  : 'Num'
					break;
					
			}
				
				
			
				ps2key_previous = ps2key;
				ps2count=0;
				ps2key=0;
				//EXTI->PR = 1 << 11; //Clear this exception pending bit
				//EXTI->IMR |=(1<11) ; optional, resume interrupt
			}//end of "if PS2 keyboard received data correctly"
			
		Delay(100);
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

void get_status(){
	if (column1_x<=bird_x+34 && column1_x >= bird_x){
	int m,n;
					for(n=0;n<320;n++){
						for(m =33;m > (bird_x+34) - column1_x;m--){
							if (n<50 + levels[0] * 10){
								column[n][m]=1;
							}
						 if (n>=310-(40 + levels[1] * 10) ){
								column[n][m]=1;
							}

						}
					}

				}
			else if (column1_x<bird_x && bird_x <= column1_x+30){
					int m,n;
					for(n=0;n<320;n++){
						for(m =0;m< (column1_x+30-bird_x);m++){
							if (n<50 + levels[0] * 10){
								column[n][m]=1;
							}
							if (n>=310-(40 + levels[1] * 10)) {
								column[n][m]=1;
							}

						}
					}
						
				}
			else if (column2_x<=bird_x+34 && column2_x >= bird_x){
					int m,n;
					for(n=0;n<320;n++){
						for(m =33;m > (bird_x+34) - column2_x;m--){
							if (n<50 + levels[2] * 10){
								column[n][m]=1;
							}
						if (n>=310-(40 + levels[3] * 10)) {
								column[n][m]=1;
							}

						}
					}	
				}
			else if (column2_x<bird_x && bird_x <= column2_x+30){
					int m,n;
					for(n=0;n<320;n++){
						for(m =0;m< (column2_x+30-bird_x);m++){
							if (n<50 + levels[2] * 10){
								column[n][m]=1;
							}
						 if (n>=310-(40 + levels[3] * 10) ){
								column[n][m]=1;
							}

						}
					}
					
				}
			else if (column3_x<=bird_x+34 && column3_x >= bird_x){
					int m,n;
					for(n=0;n<320;n++){
						for(m =33;m > (bird_x+34) - column3_x;m--){
							if (n<50 + levels[3] * 10){
								column[n][m]=1;
							}
						if (n>=310-(40 + levels[4] * 10) ){
								column[n][m]=1;
							}

						}
					}

				}
			else if (column3_x<bird_x && bird_x <= column3_x+30){
					int m,n;
					for(n=0;n<320;n++){
						for(m =0;m< (column3_x+30-bird_x);m++){
							if (n<40 + levels[3] * 10){
								column[n][m]=1;
							}
					if (n>=310-(40 + levels[4] * 10) ){
								column[n][m]=1;
							}

						}
					}	
				
				}		
}

int check_collide(){
	if(bird_y+20 < 320 &&bird_y > 0){
		if (column[bird_y][33]==1 || column[bird_y][0]==1 || column[bird_y][15]==1 
		||column[bird_y+10][33]==1|| column[bird_y+10][0]==1|| column[bird_y+10][15]==1
		||column[bird_y+23][33]==1|| column[bird_y+23][0]==1|| column[bird_y+23][15]==1 ){
					return 1;
			}
  }
	
	if(bird_y < 0)
		return 1;
	return 0;
}


void column_move(int column_move_speed){
	
	clear_column_part(column1_x,column_move_speed, levels[0], levels[1]);
	clear_column_part(column2_x,column_move_speed, levels[2], levels[3]);
	clear_column_part(column3_x, column_move_speed,levels[4], levels[5]);
	
	column1_x -= column_move_speed;
	column2_x -= column_move_speed;
	column3_x -= column_move_speed;
	if (column1_x+44 == bird_x) score++;
	if (column2_x+44 == bird_x) score++;
	if (column3_x+44 == bird_x) score++; 
	if(column1_x + 44 < 0){
		column1_x = 240;
		levels[0] =  rand()%9;
		levels[1] =  rand()%9;
	}
	if(column2_x+44 < 0) {
		column2_x = column1_x+100;
		levels[2] =  rand()%9;
		levels[3] =  rand()%9;
	}
	if(column3_x+44 < 0){
		column3_x = column1_x+200;	
		levels[4] =  rand()%9;
		levels[5] =  rand()%9;
	}
					
	draw_column(column1_x,0x07E0,levels[0],levels[1]);
	draw_column(column2_x,0x07E0,levels[2],levels[3]);
	draw_column(column3_x,0x07E0,levels[4],levels[5]);	
	
}

int main(void){
	  
	  int bird_fall_speed,column_move_speed, bird_fly_speed;
		int i,m,n;  
		int timeout = 20000;
		
		for (i = 0; i < 6; i++){
				levels[i] = rand()%9;
		}

		
	  column1_x = 240;
    column2_x = column1_x+100;
		column3_x = column1_x+200;
    bird_x = 40;
	  bird_y = 138;		
		
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
			IERG3810_TIM3_Init(4999,7199);
			//The game body
			while (1) {
				DS0_OFF;	
        keyboard_control_bird(timeout,bird_fly_speed);
				bird_fall(bird_fall_speed);	
				
				if(task2HeartBeat > 1){
				  
          //IERG3810_TFTLCD_FillRectangle(0x07FF,column1, 40, 0, 320);
				  for (m=0;m<34;m++){
					  for (n=0;n<320;n++){
						  column[n][m]=0;
					  }
				  }
          get_status();
          if (check_collide() == 1){
				   DS0_ON;
					 break;
			    }
          column_move(column_move_speed);
			    task2HeartBeat = 0;

			  }		  
    
			}
			game_over(0xFFFA);
			
	}

}

