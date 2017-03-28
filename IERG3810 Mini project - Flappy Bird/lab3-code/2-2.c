#include "stm32f10x.h"

void IERG3810_clock_tree_init(void);
void IERG3810_USART2_init(u32, u32);
void IERG3810_USART1_init(u32,u32);
void Delay(u32);

void IERG3810_clock_tree_init(void)
{
	 u8 PLL=7;
	 unsigned char temp=0;
	 RCC->CFGR &= 0xF8FF0000; 
	 RCC->CR   &= 0xFEF6FFFF;
	 RCC-> CR |= 0x00010000; //HSEON=1, (RM0008 page-119)
	  
	 while(!(RCC->CR>>17));  //check HSERDY, (RM0008 page-119)

	 RCC->CFGR = 0x00000400; //PPRE1=100, (RM0008 page-119) 
	 						//Set APB1 pre scaler to "divided by 2" , 
	 						//APB2 prescaler to "not divided"

	 RCC->CFGR |= PLL<<18; //PLLMUL = 111, (RM0008 page-119) PLLMUL *9
	 RCC->CFGR |= 1<<16;  //PLLSRC=1, (RM0008 page-119)
	 FLASH->ACR |=0x32; //set FLASH with 2 wait states
	 RCC->CR |= 0x01000000; // PLLON=1, (RM0008 page-119)
	 while(!(RCC->CR>>25));//check PLLRDY, (RM0008 page-119)
     RCC->CFGR |= 0x00000002; //SW=10, (RM0008 page-119)
    while(temp != 0x02) //check SWS (RM0008 page-119)
    {
		temp=RCC->CFGR>>2;
		temp &= 0x03;
	}	
}

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
void Delay(u32 count){
	u32 i;
	for(i=0;i<count;i++);
}

int main(void){
	
	IERG3810_clock_tree_init();
	//IERG3810_USART2_init(36,9600);
	IERG3810_USART1_init(72,9600);
	Delay(500000);
	while(1){
		  //send out SID "1155076727"
		  USART1->DR = 0x31; //1
		  Delay(50000);
		  USART1->DR = 0x31; //1
		  Delay(50000);
		  USART1->DR = 0x35; //5
		  Delay(50000);
		  USART1->DR = 0x35; //5
		  Delay(50000);
		  USART1->DR = 0x30; //0
		  Delay(50000);
		  USART1->DR = 0x37; //7
		  Delay(50000);
		  USART1->DR = 0x36; //6
		  Delay(50000);
		  USART1->DR = 0x37; //7
		  Delay(50000);
		  USART1->DR = 0x32; //2
		  Delay(50000);
		  USART1->DR = 0x37; //7
		  Delay(50000);
		  Delay(1000000);
	}
}
