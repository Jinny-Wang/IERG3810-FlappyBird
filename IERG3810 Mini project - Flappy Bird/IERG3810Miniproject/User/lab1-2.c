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
	
	//Enable KEY2  Port E Pin 2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //Enable the clock for port B
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //Set output mode to internel pull-up
	GPIO_Init(GPIOE, & GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_5); //Turn OFF the LED
	
	while(1){		
	  uint8_t keyin = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);	 //Store the output from KEY2
		if(keyin > 0){ // If key2 is not pressed
			GPIO_SetBits(GPIOB, GPIO_Pin_5);  //Turn off the LED
		}
		else{
			GPIO_ResetBits(GPIOB, GPIO_Pin_5); //Turn on the LED
		}
	}
}
