#include "stm32f10x.h"
#include "IERG3810_LED.h"

// put your procedure and code here

void IERG3810_LED_Init(void){
	/*Enable LED DS0 - PB5*/
	RCC -> APB2ENR |= 1<<3;// enable the clock of port B
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000; // output pushpull mode , 50 MHz.
	
	/*Enable LED DS1 - PE5*/
	RCC->APB2ENR |= 1<<6; // enable the clock of port E
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000; //output pushpull mode , 50 MHz.
	
	GPIOB->BSRR = 1 << 5; // setbit for LED0(PB5) //Set LED1 to high, which is "not lit" status.
	GPIOE->BSRR = 1 << 5; // setbit for LED1 (PE5) //Set LED1 to high, which is "not lit" status.
}
