#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_USART.h"
#include "IERG3810_Clock.h"

#define DS0_ON GPIOB->BRR |= 1<<5;
#define DS0_OFF GPIOB->BSRR |= 1<<5;
#define DS1_ON GPIOE->BRR |= 1<<5;
#define DS1_OFF GPIOE->BSRR |= 1<<5;
void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}

void IERG3810_key2_ExtiInit(){
	//KEY2 at PE2, EXTI-2,IRQ#8
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &=0XFFFFF0FF;
	GPIOE->CRL |= 0X00000800;
	GPIOE->ODR |= 1 << 2;
	RCC->APB2ENR |=0X01;
	AFIO->EXTICR[0] &= 0XFFFFF0FF;
	AFIO->EXTICR[0] |= 0X00000400;
	EXTI->IMR |= 1<<2;
	EXTI->FTSR |= 1<<2;
	//EXTI->RTSR |= 1<<2;
	NVIC->IP[8] = 0X65; //SET PRIORITY OF THIS INTERRUPT.
	NVIC->ISER[0] |= (1<<8);
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


void EXTI2_IRQHandler(void){
	//EXTI2 handler for KEY2
	u8 i;
	for (i=0; i<10; i++){
		DS0_ON;
		Delay(1000000);
		DS0_OFF;
		Delay(1000000);
	}
	EXTI->PR = 1<<2; //CLEAR THIS EXCEPTION PENDING BIT
}
u32 sheep = 0;

int main(void){
	
	IERG3810_clock_tree_init();
	IERG3810_USART2_init(36,9600);
	IERG3810_LED_Init();
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_key2_ExtiInit();
	USART_print(2, "1234567890");
	DS0_OFF;

	
	while(1){
		USART_print(2, " ---- ABCDEF ");
		Delay(5000000);
		DS1_ON;
		Delay(5000000);
		DS1_OFF;
		sheep++;
	}
}
