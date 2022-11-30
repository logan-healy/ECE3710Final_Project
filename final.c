#include "stm32l476xx.h"
#include "final.h"

int test = 0;
unsigned char I = 0;
int state = 0;
int ten = 0;
int one = 0;
int count = 0;

void LCD_WriteCom(unsigned char com) {
		unsigned char com1;
		unsigned char com2;
		com1 = com & (0x0F);
		com2 = (com>>4)&(0x0F);
		GPIOC->ODR &= 0xFFFFFFC0; //clear 4 bits we are sending
		GPIOC->ODR |= com2;
	
		GPIOC->ODR |= 0x10;
		delay_ms(1);
		GPIOC->ODR &= 0xFFFFFFCF;
		
		GPIOC->ODR &= 0xFFFFFFF0; //clear 4 bits we are sending
		GPIOC->ODR |= com1;
	
		GPIOC->ODR |= 0x10;
		delay_ms(1);
		GPIOC->ODR &= 0xFFFFFFCF;

}


void LCD_WriteData(unsigned char dat) {
	unsigned char dat1;
	unsigned char dat2;
	dat1 = dat & (0x0F);
	dat2 = (dat>>4) & (0x0F);
	GPIOC->ODR &= 0xFFFFFFC0;
	GPIOC->ODR |= 0x20;
	GPIOC->ODR |= dat2;
	GPIOC->ODR |= 0x10; //Turn on enable (latch)
	delay_ms(1);
	GPIOC->ODR &= 0xFFFFFFEF; //Turn off enable and clear 4 bits
	delay_ms(1);
	GPIOC->ODR &= 0xFFFFFFF0;
	GPIOC->ODR |= dat1;
	GPIOC->ODR |= 0x10; //Turn on enable (latch)
	delay_ms(1);
	GPIOC->ODR &= 0xFFFFFFEF; //Turn off enable and clear 4 bits
	delay_ms(1);
	GPIOC->ODR &= 0xFFFFFFF0;
}


void LCD_Keypad_Init(void)
{
		RCC->CR |= RCC_CR_HSION;
		RCC->CFGR |= RCC_CFGR_SW_HSI;
		while ((RCC->CR & RCC_CR_HSIRDY)==0);
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
		GPIOC->MODER &= 0xF0000000;
		GPIOC->MODER |= GPIO_MODER_MODE0_0;
		GPIOC->MODER |= GPIO_MODER_MODE1_0;
		GPIOC->MODER |= GPIO_MODER_MODE2_0;
		GPIOC->MODER |= GPIO_MODER_MODE3_0;
		GPIOC->MODER |= GPIO_MODER_MODE4_0;
		GPIOC->MODER |= GPIO_MODER_MODE5_0;
		GPIOC->MODER |= GPIO_MODER_MODE6_0;
		GPIOC->MODER |= GPIO_MODER_MODE7_0;
		GPIOC->MODER |= GPIO_MODER_MODE8_0;
		GPIOC->MODER |= GPIO_MODER_MODE9_0;
		GPIOC->OTYPER |= GPIO_OTYPER_OT6;
		GPIOC->OTYPER |= GPIO_OTYPER_OT7;
		GPIOC->OTYPER |= GPIO_OTYPER_OT8;
		GPIOC->OTYPER |= GPIO_OTYPER_OT9;
		
	//	delay_ms(15);
		LCD_WriteCom(0x02); //return cursor to inital position 6.2
		delay_ms(1);
		LCD_WriteCom(0x28); //6.6 4bit bus, 2line display, 5x8 matrix
		delay_ms(1);
		LCD_WriteCom(0x28);
		delay_ms(1);
		LCD_WriteCom(0x28);
		delay_ms(1);
		LCD_WriteCom(0x0C); //6.4 display switch control
		delay_ms(2);
		LCD_WriteCom(0x06); //Cursor moves left to  right 6.3
		delay_ms(1);
		LCD_WriteCom(0x01);
}

//void Keypad_Init(void){
//	
//	RCC->CR |= RCC_CR_HSION;
//	RCC->CFGR |= RCC_CFGR_SW_HSI;
//	while ((RCC->CR & RCC_CR_HSIRDY)==0);
//	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
//	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
//	
//	GPIOB->MODER &= 0xF0000FFF;
//	GPIOB->MODER |= GPIO_MODER_MODE6_0;
//	GPIOB->MODER |= GPIO_MODER_MODE7_0;
//	GPIOB->MODER |= GPIO_MODER_MODE8_0;
//	GPIOB->MODER |= GPIO_MODER_MODE9_0;
//	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
//	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
//	GPIOB->OTYPER |= GPIO_OTYPER_OT8;
//	GPIOB->OTYPER |= GPIO_OTYPER_OT9;
//	GPIOB->PUPDR &= 0x00000000;
	
//	GPIOC->MODER = GPIO_MODER_MODE0_0;
//	GPIOC->MODER |= GPIO_MODER_MODE1_0;
//	GPIOC->MODER |= GPIO_MODER_MODE2_0;
//	GPIOC->MODER |= GPIO_MODER_MODE3_0;
//	GPIOC->MODER |= GPIO_MODER_MODE4_0;
	
//	GPIOC->PUPDR = 0x000002AA;
	
//}




void delay_ms(unsigned int ms) {
		volatile int x = 0;
		unsigned int counter = ms * ((16000)/6);
		for (unsigned int i=0; i < counter;i++)
		{
			x = x+1;
		}
		return;
}




void KeypadPoll(void){					//gets tens value and calls keypadpollones
	while(1){
	if(state == 0){
		GPIOB->ODR |= 0x380;
		delay_ms(1);
		if ((GPIOB->IDR & 0x2000) != 0x2000){
			//	LCD_WriteData('A');
		}
		if ((GPIOB->IDR & 0x1000) != 0x1000){
			//	LCD_WriteData('3');
		}
		if ((GPIOB->IDR & 0x800) != 0x800){
			//LCD_WriteData('2');
		}
		if ((GPIOB->IDR & 0x400) != 0x400){
			//LCD_WriteData('1');
		}
		
		
		GPIOB->ODR ^= 0xC0;
		delay_ms(1);
		if ((GPIOB->IDR & 0x2000) != 0x2000){
			//LCD_WriteData('B');
		}
		if ((GPIOB->IDR & 0x1000) != 0x1000){
			//LCD_WriteData('6');
		}
		if ((GPIOB->IDR & 0x800) != 0x800){
			//LCD_WriteData('5');
		}
		if ((GPIOB->IDR & 0x400) != 0x400){
			//LCD_WriteData('4');
		}
		
		
		
		GPIOB->ODR ^= 0x180;
		delay_ms(1);
		if ((GPIOB->IDR & 0x2000) != 0x2000){
			//LCD_WriteData('C');
		}
		if ((GPIOB->IDR & 0x1000) != 0x1000){
			//LCD_WriteData('9');
		}
		if ((GPIOB->IDR & 0x800) != 0x800){
			//LCD_WriteData('8');
		}
		if ((GPIOB->IDR & 0x400) != 0x400){
			//LCD_WriteData('7');
		}
		
		
		
		GPIOB->ODR ^= 0x300;
		delay_ms(1);
		if ((GPIOB->IDR & 0x2000) != 0x2000){
			//LCD_WriteData('D');
		}
		if ((GPIOB->IDR & 0x1000) != 0x1000){
			//LCD_WriteData('#');
		}
		if ((GPIOB->IDR & 0x800) != 0x800){
			//LCD_WriteData('0');
		}
		if ((GPIOB->IDR & 0x400) != 0x400){
			//LCD_WriteData('*');
		}
		
		GPIOB->ODR &= 0xFFFFFC3F;
	}
	}
}




void SysTick_Initialize(int ticks){
	SysTick->CTRL = 0;
	SysTick->LOAD = ticks-1;
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000007;
	
}



void SysTick_Handler(void){


}


	


