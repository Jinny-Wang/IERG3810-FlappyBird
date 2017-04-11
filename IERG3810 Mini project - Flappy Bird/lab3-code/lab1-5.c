#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"

void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}

int main(void){
	
	
	IERG3810_LED_Init();
	IERG3810_KEY_Init();
	IERG3810_Buzzer_Init();
	
	while(1){
		
		//uint32_t keyin = GPIOE->IDR;
		uint32_t key2_input = (GPIOE->IDR  & 0x00000004) >> 2;
		uint32_t key1_input = (GPIOE->IDR  & 0x00000008) >> 3;
		uint32_t keyup_input = GPIOA->IDR & 0x00000001;
		
		/*Key2 Controls LED0 : press-lit*/
		if(key2_input == 1) //not pressed
		{
			GPIOB->BSRR = 1 << 5; //set->high->turn off the LED 
		}
		else{ //pressed
			GPIOB->BRR = 1 << 5; // reset-> low-> lit the LED 
		}
		
		/*Key1 toggles LED1*/
		if(key1_input == 0){
				if( (GPIOE->ODR & 0x00000020) >> 5 == 0)//check whether LED1 is lit
				{
						GPIOE->BSRR = 1 << 5; //turn off the LED
					  //Delay(1000000);
				}
				else{
					GPIOE->BRR = 1 << 5; //turn on the LED
					//Delay(1000000);
				}
				
				while(key1_input ==0){
					key1_input = (GPIOE->IDR  & 0x00000008) >> 3;
				}
		}
		
		/*Key_up(PA0) toggles Buzzer(PB8)*/
		if(keyup_input == 1){
				if((GPIOB->ODR & 0x00000100) >> 8 == 1)//check whether the buzzer is sounding
				{
						GPIOB->BRR = 1 << 8; //turn off the buzzer
					  //Delay(1000000);
				}
				else{
					GPIOB->BSRR = 1 << 8; //turn on the buzzer
					//Delay(1000000);
				}
				
				while(keyup_input == 1){
					keyup_input = GPIOA->IDR & 0x00000001;
				}
		}
		Delay(100000);
			
			
 }
}
