#include "stm32f10x.h"

void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}

int main(void){

	/*Initialization*/
	
	/*Enable LED DS0 - PB5*/
	RCC -> APB2ENR |= 1<<3;   // enable the clock of port B
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000; // set max clock rate  to 50 MHz, output push-pull mode
	
	/*Enable KEY2 PE2*/
	RCC->APB2ENR |= 1<<6;     // enable the clock of port E
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800; // set max clock rate  to 50 MHz, Input with pull-up pull-down
	
  	/*Enable LED DS1 - PE5*/
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000; // set max clock rate  to 50 MHz, output push-pull mode
	
	/*Enable KEY1 PE3*/
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000; // set max clock rate  to 50 MHz, Input with pull-up pull-down
	
	/*Enable KEY_UP PA0*/
	RCC -> APB2ENR |= 1<<2;
	GPIOA->CRL &= 0xFFFFFFF0; // enable the clock of port A
	GPIOA->CRL |= 0x00000008; // set max clock rate  to 50 MHz, Input with pull-up pull-down
	
	/*Enable Buzzer PB8*/
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000003; // set max clock rate  to 50 MHz, output push-pull mode
	
	GPIOB->BSRR = 1 << 5; // setbit for LED0 (PB5) Set LED0 to high, which is "not lit" status.
	GPIOE->BSRR = 1 << 2; // setbit for key2 (PE2) // Set KEY2 to high, which is "not pressed" status.
	GPIOE->BSRR = 1 << 5; // setbit for LED1 (PE5) Set LED1 to high, which is "not lit" status.
	GPIOE->BSRR = 1 << 3; // setbit for key1 (PE3) // Set KEY1 to high, which is "not pressed" status.
	GPIOA->BRR = 1; //reset bit for key_up (PA0) // set key_up to low, which is "not pressed" status.
	GPIOB->BRR = 1 << 8; //reset bit for buzzer (PB8) // set buzzer to low, no sound.
	
	
	while(1){

		uint32_t key2_input = (GPIOE->IDR  & 0x00000004) >> 2; // read input from key2
		uint32_t key1_input = (GPIOE->IDR  & 0x00000008) >> 3; // read input from key1
		uint32_t keyup_input = GPIOA->IDR & 0x00000001;        // read input from keyup
		
		/*Key2 Controls LED0 : press-lit*/
		if(key2_input == 1) //not pressed
		{
			GPIOB->BSRR = 1 << 5; //set->high->turn off the LED 
		}
		else{ //pressed
			GPIOB->BRR = 1 << 5; // reset-> low-> light the LED 
		}
		
		/*Key1 toggles LED1*/
		if(key1_input == 0){
				if( (GPIOE->ODR & 0x00000020) >> 5 == 0) //check whether LED1 is lit
				{
					GPIOE->BSRR = 1 << 5;             //turn off the LED if it was lit
				}
				else{
					GPIOE->BRR = 1 << 5;               //turn on the LED if it was not lit
				}
				
				while(key1_input ==0){                 // when key_1 is still pressed
					key1_input = (GPIOE->IDR  & 0x00000008) >> 3; //stay in the loop to prevent changes

				}
		}		
		/*Key_up(PA0) toggles Buzzer(PB8)*/
		if(keyup_input == 1){
				if((GPIOB->ODR & 0x00000100) >> 8 == 1)//check whether the buzzer is sounding
				{
						GPIOB->BRR = 1 << 8; //turn off the buzzer if it was sounding
					  
				}
				else{
					GPIOB->BSRR = 1 << 8; //turn on the buzzer if it was not sounding
				
				}
			    
			    /*stay in the loop when key_up is still pressed*/	
				while(keyup_input == 1){
					keyup_input = GPIOA->IDR & 0x00000001;
				}
		}
		Delay(100000);	//Delay to slow down the while loop, prevent errors when toggling.	
 	}
}
