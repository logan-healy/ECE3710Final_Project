#include "stm32l476xx.h"
#include "final.h"

int cups = 0;
int state = 0;
int light = 0;
int cuphours;

int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}


void run(void)
{
	
	
	
	
}

void Light_Prompt(void)
{
	LCD_DisplayString(0, "Please enter hrs");
	LCD_DisplayString(1, "of light per day");
	KeypadPoll(&light);
}




void Water_Prompt(void)
{
	LCD_DisplayString(0, "Please enter #");
	LCD_DisplayString(1, "of cups per day");
	KeypadPoll(&cups);
	cuphours = divRoundClosest(24,cups);
}

void Pump_Init(void)				//function to intialize gpioa1 to be pump control
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;			//ENABLE GPIOA CLOCK
	GPIOA->MODER &= 0xFFFFFFFD;								//SET GPIOA1 TO OUTPUT
	GPIOA->MODER |= GPIO_MODER_MODE0_0;			
	GPIOA->ODR &= 0xFFFFFFFE;									//SET BOTTOM BIT OF ODR TO 0								
}

void Pump_Cup(unsigned int cup)		//pass number of cups of water desired to pump
{
	unsigned int delay = cup * 1000;
	
	GPIOA->ODR |= 0x1;					//TURN PUMP ON
	
	delay_ms(delay);						//WAIT
	
	GPIOA->ODR &= 0xFFFFFFFE;		//TURN PUMP OFF
	
	
}

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

void LCD_DisplayString(unsigned int line, unsigned char *ptr) {
	if (line == 0){
		LCD_WriteCom(0x80);
}
	if (line == 1){
		LCD_WriteCom(0x80+0x40);
	}
	for (int i = 0; ptr[i]!='\0'; i++)
	{
		LCD_WriteData(ptr[i]);
	}
	
}

void LCD_Keypad_Init(void)
{
		RCC->CR |= RCC_CR_HSION;
		RCC->CFGR |= RCC_CFGR_SW_HSI;
		while ((RCC->CR & RCC_CR_HSIRDY)==0);
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
		GPIOC->MODER &= 0xF0000000;
		GPIOC->MODER |= GPIO_MODER_MODE0_0;		//GPIOC0 set to output for D4
		GPIOC->MODER |= GPIO_MODER_MODE1_0;		//GPIOC1 set to output for D5
		GPIOC->MODER |= GPIO_MODER_MODE2_0;		//GPIOC2 set to output for D6
		GPIOC->MODER |= GPIO_MODER_MODE3_0;   //GPIOC3 set to output`for D7
		GPIOC->MODER |= GPIO_MODER_MODE4_0;		//GPIOC4 set to output for Enable
		GPIOC->MODER |= GPIO_MODER_MODE5_0;		//GPIOC5 set to output for RS
		GPIOC->MODER |= GPIO_MODER_MODE6_0;		//GPIOC6 set to output for R1
		GPIOC->MODER |= GPIO_MODER_MODE7_0;		//GPIOC7 set to output for R2
		GPIOC->MODER |= GPIO_MODER_MODE8_0;		//GPIOC8 set to output for R3
		GPIOC->MODER |= GPIO_MODER_MODE9_0;		//GPIOC9 set to output for R4
		GPIOC->OTYPER |= GPIO_OTYPER_OT6;			//Set GPIOC6 to open drain
		GPIOC->OTYPER |= GPIO_OTYPER_OT7;			//Set GPIOC7 to open drain
		GPIOC->OTYPER |= GPIO_OTYPER_OT8;			//Set GPIOC8 to open drain
		GPIOC->OTYPER |= GPIO_OTYPER_OT9;			//Set GPIOC9 to open drain
		

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
		delay_ms(5);
		LCD_WriteCom(0x80);			//Write Command to set cursor to first position of first line
		//LCD_WriteCom(0x80+0x40)   //Write command to set cursot to first position of second line
}


void delay_ms(unsigned int ms) {
		volatile int x = 0;
		unsigned int counter = ms * ((16000)/6);
		for (unsigned int i=0; i < counter;i++)
		{
			x = x+1;
		}
		return;
}



void KeypadPoll(int *value){
	int temp;
	while(1){
		GPIOC->ODR |= 0x380;
		delay_ms(1);
		if ((GPIOC->IDR & 0x2000) != 0x2000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_DisplayString(0,"Invalid");
				//LCD_WriteData('A');
				state = 0;
		}
		if ((GPIOC->IDR & 0x1000) != 0x1000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('3');
				state = 1;
				temp = 3;
		}
		if ((GPIOC->IDR & 0x800) != 0x800){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('2');
				state = 1;
				temp = 2;
		}
		if ((GPIOC->IDR & 0x400) != 0x400){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('1');
				state = 1;
				temp = 1;
		}
		
		
		GPIOC->ODR ^= 0xC0;
		delay_ms(1);
		if ((GPIOC->IDR & 0x2000) != 0x2000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				//LCD_WriteData('B');
				LCD_DisplayString(0,"Invalid");
				state = 0;
		}
		if ((GPIOC->IDR & 0x1000) != 0x1000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('6');
				state = 1;
				temp = 6;
		}
		if ((GPIOC->IDR & 0x800) != 0x800){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('5');
				state = 1;
				temp = 5;
		}
		if ((GPIOC->IDR & 0x400) != 0x400){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('4');
				state = 1;
				temp = 4;
		}
		
		
		
		GPIOC->ODR ^= 0x180;
		delay_ms(1);
		if ((GPIOC->IDR & 0x2000) != 0x2000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				//LCD_WriteData('C');
				LCD_DisplayString(0,"Invalid");
				state = 0;
		}
		if ((GPIOC->IDR & 0x1000) != 0x1000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('9');
				state = 1;
				temp = 9;
		}
		if ((GPIOC->IDR & 0x800) != 0x800){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('8');
				state = 1;
				temp = 8;
		}
		if ((GPIOC->IDR & 0x400) != 0x400){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('7');
				state = 1;
				temp = 7;
		}
		
		
		
		GPIOC->ODR ^= 0x300;
		delay_ms(1);
		if ((GPIOC->IDR & 0x2000) != 0x2000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				//LCD_WriteData('D');
				LCD_DisplayString(0,"Invalid");
				state = 0;
		}
		if ((GPIOC->IDR & 0x1000) != 0x1000){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				//LCD_WriteData('#');
				state = 2;
		}
		if ((GPIOC->IDR & 0x800) != 0x800){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				LCD_WriteData('0');
				state = 1;
				temp = 0;
		}
		if ((GPIOC->IDR & 0x400) != 0x400){
				LCD_WriteCom(0x1);
				delay_ms(5);
				LCD_WriteCom(0x80);
				delay_ms(5);
				//LCD_WriteData('*');
				LCD_DisplayString(0,"Invalid");
				state = 0;
		}
		
		if(state == 1)
		{
			LCD_DisplayString(1, "# to confirm");
		}
		
		if(state == 2)
		{
			*value = temp;
			return;
		}
		
		GPIOC->ODR &= 0xFFFFFC3F;
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


	


