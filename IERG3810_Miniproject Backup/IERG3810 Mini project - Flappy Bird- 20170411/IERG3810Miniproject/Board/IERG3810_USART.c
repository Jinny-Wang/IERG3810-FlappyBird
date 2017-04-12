#include "stm32f10x.h"
#include "IERG3810_USART.h"

// put your procedure and code here

void IERG3810_USART2_init(u32 pclk1, u32 bound)
{
		//USATR2
		float temp;
		u16 mantissa;
		u16 fraction;
	  	temp = (float)(pclk1 * 1000000)/(bound*16);
		mantissa = temp;
		fraction = (temp-mantissa) * 16;
		mantissa <<=4;
		mantissa += fraction;
		RCC->APB2ENR |= 1<<2; // Enable Port A (RM0008 page-142)
		RCC->APB1ENR |= 1<<17; //Enable USART 2(RM0008 page-144)
		GPIOA->CRL &= 0xffff00ff; // set PA2, PA3 Alternate Function
		GPIOA->CRL |= 0x00008B00; //set PA2, PA3 Alternate Function
		RCC->APB1RSTR |= 1<<17; // Reset USART2(RM0008 page-152)
		RCC->APB1RSTR &= ~(1<<17); //Set USART2(RM0008 page-152) ~means inverted
		USART2->BRR=mantissa; //Configure Baud rate register of USART2(RM0008 page-792)
	  	USART2->CR1 |= 0x2008; //Configure Control register1 , set USART2 to 8N1(RM0008 page-793)
	
}

void IERG3810_USART1_init(u32 pclk2, u32 bound)
{
		//USATR1
		float temp;
		u16 mantissa;
		u16 fraction;
	  	temp = (float)(pclk2 * 1000000)/(bound*16);
		mantissa = temp;
		fraction = (temp-mantissa) * 16;
		mantissa <<=4;
		mantissa += fraction;
	  	RCC->APB2ENR |= 1<<2; //IOPAENR - Enable PortA(RM0008 page-142)
	  	RCC->APB2ENR |= 1<<14; // USART1EN - Enable USART1 (RM0008 page-142)
		GPIOA->CRH &= 0xfffff00f; // set PA9, PA10 Alternate Function
		GPIOA->CRH |= 0x000008B0; //set PA9, PA10 Alternate Function
		RCC->APB2RSTR |= 1<<14; // Reset USART1(RM0008 page-103)
		RCC->APB2RSTR &= ~(1<<14); //Set USART1(RM0008 page-103) ~means inverted
		USART1->BRR=mantissa; //Configure Baud rate register of USART1(RM0008 page-792)
	  	USART1->CR1 |= 0x2008; //Configure Control register1 , set USART1 to 8N1(RM0008 page-793)
	
}

void USART_print(u8 USARTport, char *st)
{
	u8 i=0;
	while(st[i] != 0x00) //While the string hasn't ended
	{
		if(USARTport == 1)
		{
			USART1->DR = st[i]; //send out the i-th character
			while((USART1->SR &0x00000080) >>7 != 1); //When transmission hasn't ended,wait
		}
		if(USARTport == 2)
    	{
			USART2->DR = st[i];//send out the i-th character
			while((USART2->SR &0x00000080) >>7 != 1);//When transmission hasn't ended,wait
    	}			
		if(i == 255) break;
		i++;
	}	
}
