#include "stm32f10x.h"
#include "IERG3810_KEY.h"

// put your procedure and code here

void IERG3810_KEY_Init(void){
	/*Enable KEY2 PE2*/
	RCC->APB2ENR |= 1<<6; // enable the clock of port E
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800; // Input with pull-up pull-down
	
	/*Enable KEY1 PE3*/
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000;
	
	
	/*Enable KEY_UP PA0*/
	RCC -> APB2ENR |= 1<<2;
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	
	GPIOE->BSRR = 1 << 2; // setbit for key2 (PE2) 
						  //Set KEY2 to high, which is "not pressed" status.

	GPIOE->BSRR = 1 << 3; // setbit for key1 (PE3) 
						  // Set KEY1 to high, which is "not pressed" status.

	GPIOA->BRR = 1; //reset bit for key_up (PA0) 
					// set key_up to low, which is "not pressed" status.
}
