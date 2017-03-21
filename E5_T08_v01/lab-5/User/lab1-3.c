#include "stm32f10x.h"

void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}

int main(void){
	
	RCC -> APB2ENR |= 1<<3; // equivalent to RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	                        // Enable the clock of port B

	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000; // equivalent to  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
							  //and  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 						  // set the Maximum clock rate to 50 MHz, the output mode to push-pull

	GPIOB->BSRR = 1 << 5; // equivalent to GPIO_SetBits(GPIOB,GPIO_Pin_5)
	                      // setbit for LED0 (PB5). Set LED0 to high, which is "not lit" status.
	
	
	while(1){
		GPIOB->BRR = 1 << 5;   //Turn ON the LED
	  	Delay(5000000);
		GPIOB->BSRR = 1 << 5;  //Turn OFF the LED
		Delay(5000000);
 	}
}
