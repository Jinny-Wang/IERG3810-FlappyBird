#include "stm32f10x.h"
void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}
int main(void){

	//Initialization for LED-DS0 PortB Pin 5
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //Enable the clock for port B
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // Set the output mode to push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set GPIOB peripheral maximum speed to 50 MHz
	GPIO_Init(GPIOB, & GPIO_InitStructure);
		
	GPIO_SetBits(GPIOB,GPIO_Pin_5); //Light the LED first 
		
	while(1){	 
		GPIO_SetBits(GPIOB, GPIO_Pin_5);  //Light the LED
		Delay(1000000);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_5); //Turn off the LED
		Delay(1000000);	
	}
}
