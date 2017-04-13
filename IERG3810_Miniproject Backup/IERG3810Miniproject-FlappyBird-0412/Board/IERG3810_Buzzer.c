#include "stm32f10x.h"
#include "IERG3810_Buzzer.h"

// put your procedure and code here
void IERG3810_Buzzer_Init(void){
	
	/*Enable Buzzer PB8*/
	RCC -> APB2ENR |= 1<<3; 
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000003;
	GPIOB->BRR = 1 << 8; //reset bit for buzzer (PB8) // set buzzer to low, no sound.
}
