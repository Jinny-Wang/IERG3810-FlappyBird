#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"
#include "global.h"

u8 task1HeartBeat = 0;
u8 task2HeartBeat = 0;
#define DS0_ON GPIOB->BRR |= 1<<5;
#define DS0_OFF GPIOB->BSRR |= 1<<5;
#define DS1_ON GPIOE->BRR |= 1<<5;
#define DS1_OFF GPIOE->BSRR |= 1<<5;

int ps2count=0;
u8 ps2key;
u8 ps2key_previous=0x00;
void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
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


void DS0_turnOff(){
	//level one subroutine
	GPIOB->BSRR = 1<<5; //turn off DS0 directly
}

void DS0_turnOff2(){
	//level two subroutine
	DS0_turnOff();//call level oen subroutine
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

void IERG3810_TIM4_Init(u16 arr,u16 psc){
	RCC->APB1ENR |= 1<<2; //enable TIM4
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM4->DIER = 1<<0;
	TIM4->CR1 |= 0x01;
	NVIC->IP[30] = 0x45; //refer to lab-4
	NVIC->ISER[0] |= (1<<30); //refer to lab-4
	
}

void TIM3_IRQHandler(void){
	
	
	//code for 5-1
	if(TIM3->SR & 1<<0)//RM0008 page-395 When update interrupt pending occur
	{
		 GPIOB->ODR ^= 1<<5;//toggle DS0 with read-modify-write
	}
	TIM3->SR &= ~(1<<0);//RM0008 page-395 clear the interrupt pending bit
	/* code for 5-3
	GPIOB->BRR = 1 << 5;//direct modify register
	GPIOB->BSRR = 1 << 5;//direct modify register
	GPIOB->BRR = 1 << 5;//direct modify register
	GPIOB->BSRR = 1 << 5;//direct modify register
	GPIOB->ODR ^= 1<<5; //toggle DS0 with read-modify-write
	GPIOB->ODR ^= 1<<5; //toggle DS0 with read-modify-write
	GPIOB->ODR ^= 1<<5; //toggle DS0 with read-modify-write
	GPIOB->ODR ^= 1<<5; //toggle DS0 with read-modify-write
	GPIOB->ODR &= ~(1<<5); //use &= operator
	GPIOB->ODR |= 1<<5 ;//use |= operator
	GPIOB->ODR &= ~(1<<5); //use &= operator
	GPIOB->ODR |= 1<<5 ;//use |= operator
	TIM3->SR &= ~(1<<0); //RM0008 page-395
	TIM3->SR &= ~(1<<0); //Do one more time clear the interrupt pending bit*/
}

void TIM4_IRQHandler(void){
	if(TIM4->SR & 1<<0)//RM0008 page-395 When update interrupt pending occur
	{
		 GPIOE->ODR ^= 1<<5;//toggle DS1 with read-modify-write
	}
	TIM4->SR &= ~(1<<0);//RM0008 page-395 clear the interrupt pending bit
}

void IERG3810_SYSTICK_Init10ms(void){
	//SYSTICK
	SysTick->CTRL = 0; //Clear Systick->CTRL setting
	SysTick->LOAD = 89999; //what should be filled? refer to 0337E page 8-10
	                       //72M / (8*100) - 1  
	                       //10ms -> 100 Hz
	
	//CLKSOURCE=0 : STCLK (FCLK/8)
	//CLKSOURCE = 1: FCLK
	//CLKSOURCE=0 is synchronized and better than CLKSROUCE = 1
	//Refer to Clock tree on page-93 of RM0008
	SysTick->CTRL |= 0x03; //what should be filled?
	                      //refer to 033tE page 8-8
}

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc){
	RCC->APB2ENR |= 1<<3;
	GPIOB->CRL &= 0xFF0FFFFF; 
	GPIOB->CRL |= 0x00B00000;
	RCC->APB2ENR |= 1<<0 ; //lab4
	AFIO->MAPR &= 0xFFFFF3FF; // RM0008 page -179
	AFIO->MAPR |= 1<<11; //RM0008 page -179
	RCC->APB1ENR |= 1<<1; //RM0008 page 112
	TIM3->ARR = arr; //RM0008 page 403
	TIM3->PSC = psc; //RM0008 page 402
	TIM3->CCMR1 |= 7 << 12; // page 397
	TIM3->CCMR1 |= 1<<11; //page 397
	TIM3->CCER |= 1<<4; //page 401
	TIM3->CR1 = 0x0080; //RM0008 page 388
	TIM3->CR1 |= 0x01; //RM0008 page 388
}



void lab5_4(void){
	IERG3810_clock_tree_init();
	IERG3810_LED_Init();
	IERG3810_NVIC_SetPriorityGroup(5);//set PRIGROUP
	IERG3810_TIM3_Init(4999,7199); //set timer-3 
	IERG3810_TIM4_Init(4999,1799); //set timer-4 
	GPIOB->BRR = 1<<5;
	while(1){	
		DS0_turnOff(); 
		GPIOB->BRR = 1<<5;
		DS0_turnOff2(); 
		//DS0_turnOff();
		//DS0_turnOff2();
	}
}

void lab5_5(void){
	IERG3810_clock_tree_init();
	IERG3810_LED_Init();
	//IERG3810_NVIC_SetPriorityGroup(5);//set PRIGROUP
	//IERG3810_TIM3_Init(4999,7199); //set timer-3 
	//IERG3810_TIM4_Init(4999,1799); //set timer-4 
	IERG3810_SYSTICK_Init10ms();
	GPIOB->BRR = 1<<5;
	GPIOE->BRR = 1<<5;
	while(1){	
		if(task1HeartBeat >= 20)
		{
			task1HeartBeat=0;
			GPIOB->ODR ^= 1<<5;
			//write task1 here		
		}
		if(task2HeartBeat >= 33)
		{
			task2HeartBeat=0;
			GPIOE->ODR ^= 1<<5;
		}
	}
	
}
int main(void){
	u16 led0pwmval = 0;
	u8 dir = 1;
	IERG3810_clock_tree_init();
	IERG3810_LED_Init();
	IERG3810_TIM3_PwmInit(9999,47);//arr= , psc=0
	IERG3810_TIM4_Init(4999,1799); //set timer-4 
	IERG3810_NVIC_SetPriorityGroup(5);//set PRIGROUP
	while(1){	
		Delay(1500);
		if(dir)
		{
			led0pwmval++;
		}
		else
    {			
			led0pwmval--;
		}
		if(led0pwmval > 5000){ 
			dir=0;
		}
		if(led0pwmval == 0){ 
			dir = 1;
		}
		TIM3->CCR2 = led0pwmval;
	}

}

