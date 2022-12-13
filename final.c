#include "stm32l476xx.h"
#include "final.h"
#include "stdint.h"

static int cups = 0;
static int state;
static int light = 0;
static int cuphours;
static int seconds;
static int minutes;
static int hours;
static int pumpflag = 0;
static int breakflag = 0;
static int humidity;
static int temperature;
static uint8_t humidity2;
static uint8_t temperature2;
static uint8_t humidity1;
static uint8_t temperature1;
static uint8_t presence;
static uint16_t sum;

void delay_us(unsigned int us){    //1 us delay = 2.8
		volatile int x = 0;
		unsigned int counter = us * 1;
		for (unsigned int i=0; i < counter;i++)
		{
				x = x+1;
		}
		return;
}

void tempInit(){
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= 0xFFFFFCFF;
	GPIOB->MODER |= 0x100;
	GPIOB->ODR |= 0x10;

}

void startTempSignal(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODE4_0;
 	GPIOB->ODR &= 0xFFFFFFEF; //send 0 to PB4
	delay_us(20000);
	GPIOB->ODR  |= 0x00000010; //send 1 to PB4
	delay_us(25);
	GPIOB->ODR &= 0xFFFFFFEF;
	GPIOB->MODER &= 0xFFFFFCFF; //set PB4 to input
}

uint8_t Check_Response(){
	uint8_t response = 0;
	
	delay_us(32);

	if ((GPIOB->IDR & 0x10) == 0)
	{
		delay_us(67);
		if ((GPIOB->IDR & 0x10) == 0x10){
			response = 1;
		}
		else{ 
			response = 0;
		}
	}
	while((GPIOB->IDR & 0x10) == 0x10);
	
	return response;
}

char DHT11_Read(){
	int i, j;
	for (int i = 0; i < 8; i++){
		while((GPIOB->IDR & 0x10) == 0);
	
			delay_us(40);
			if ((GPIOB->IDR & 0x10) == 0){
				
				j &= ~(1<<(7-i));
			}
			else{
				j |= (1<<(7-i));
			}
			while((GPIOB->IDR & 0x10) == 0x10);
		}
		return j;
	}

		void DHT_GetData(){
		
			startTempSignal();
			presence = Check_Response();
			humidity1 = DHT11_Read();
			humidity2 = DHT11_Read();
			temperature1 = DHT11_Read();
			temperature2 = DHT11_Read();
			sum = DHT11_Read();
		
			temperature = temperature1;
			humidity = humidity1;
	}
	

void CheckParam (void)
{
	//if GPIOB->IDR10 is high: set GPIOB->ODR1 high to send water level low to raspberry pi
	if((GPIOB->IDR & 0x400) == 0x400)
	{
		GPIOB->ODR |= 0x2;
	}
	
	if(temperature < 10 || temperature > 50)
	{
		GPIOB->ODR |= 0x4;
	}
	
	if(humidity < 10 || humidity > 80)
	{
		GPIOB->ODR |= 0x8;
	}
	
	delay_ms(5);
	GPIOB->ODR &= 0xFFFFFFF1;
	
	//CHECK TEMP
	//IF TEMP IS OUTSIDE OF RANGE: GPIOB->ODR2 = 1
	//CHECK HUMIDITY
	//IF HUMIDITY IS OUTSIDE OF RANGE: GPIOB->ODR3 = 1
	//CHECK WATER LEVEL
	//IF WATER IS LOW: GPIOB->ODR1 = 1
	
}
void RaspberryPiInit(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	GPIOB->MODER &= 0xFFFFFF03;						//Clear gpiob 1,2,3
	GPIOB->MODER |= 0x00000057;						//Set gpiob 1,2,3 to output
}


void button_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= 0xFFFFFFFC;
	GPIOB ->PUPDR |= 0x1;
}


void EXTI_Init(void){
	NVIC_EnableIRQ(EXTI0_IRQn);
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
	EXTI->FTSR1 |= EXTI_FTSR1_FT0;
	EXTI->IMR1 |= EXTI_IMR1_IM0;
}

void EXTI0_IRQHandler(void) {
	
	if ((EXTI->PR1 & EXTI_PR1_PIF0) != 0) {
		Water_Prompt();
		delay_ms(5);
		Light_Prompt();
    EXTI->PR1 |= EXTI_PR1_PIF0;
     }
      return;
}


int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}


void run(void)
{
	while(1)
	{
	
		LCD_DisplayString(0, "Temp:");
		char T1 = (char)(temperature >> 4) +48;
		LCD_WriteData(T1);
		char T2 = (char)(temperature & 0x0F) + 48;
		LCD_WriteData(T2);
		LCD_WriteData(' ');
		LCD_WriteData('C');
		
		if(humidity == 0x1A)
		{
			humidity = 20;
		}
		if(humidity == 0x1B)
		{
			humidity = 21;
		}
		if(humidity == 0x1C)
		{
			humidity = 22;
		}
		if(humidity == 0x1D)
		{
			humidity = 23;
		}
		if(humidity == 0x1E)
		{
			humidity = 24;
		}
		
		LCD_DisplayString(1, "Humidity:");
		char H1 = (char)(humidity >> 4) + 48;
		LCD_WriteData(H1);
		char H2 = (char)(humidity & 0x0F) + 48;
		LCD_WriteData(H2);
		LCD_WriteData('%');
		
		delay_ms(1000);
		
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
		
		LCD_DisplayString(0, "Cups/Day:");
		char C = (char)cups + 48;
		LCD_WriteData(C);
		
		LCD_DisplayString(1, "HRS of Light:");
		char L = (char)light + 48;
		LCD_WriteData(L);
		
		CheckParam();
		
		delay_ms(1000);
		
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
	}
}

void Light_Prompt(void)
{
	if(breakflag == 1)
	{
		breakflag = 0;
		return;
	}
	
	LCD_DisplayString(0, "Please enter hrs");
	LCD_DisplayString(1, "of light per day");
	light = KeypadPoll();
}




void Water_Prompt(void)
{
	if(breakflag == 1)
	{
		return;
	}
	LCD_DisplayString(0, "Please enter #");
	LCD_DisplayString(1, "of cups per day");
	cups = KeypadPoll();
	cuphours = divRoundClosest(24,cups);
	LCD_WriteCom(0x1);
}

void Pump_Init(void)				//function to intialize gpioa1 to be pump control
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;			//ENABLE GPIOA CLOCK
	GPIOA->MODER &= 0xFFFFFFC0;								//CLEAR BOTTOM 6 BITS
	GPIOA->MODER |= GPIO_MODER_MODE0_0;				//SET PIN 0 TO OUTPUT;PIN 1 TO OUTPUT; LEAVE PIN 2 AS INPUT
	GPIOA->MODER |= GPIO_MODER_MODE1_0;
	GPIOA->ODR &= 0xFFFFFFFC;									//SET BOTTOM TWO BITS OF ODR TO 0	

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= 0xFFCFFFFF;
	GPIOB->PUPDR |= 0x00100000;
}

void Pump_Cup(unsigned int cup)		//pass number of cups of water desired to pump
{
	unsigned int delay = cup * 3000;
	
	GPIOA->ODR |= 0x1;					//TURN PUMP ON
	
	delay_ms(delay);						//WAIT
	
	GPIOA->ODR &= 0xFFFFFFFE;		//TURN PUMP OFF
	
	
}

void Light_On(void)
{
	GPIOA->ODR |= 0x2;
}

void Light_Off(void)
{
	GPIOA->ODR &= 0xFFFFFFFD;
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
}




void delay_ms(unsigned int ms) {
	
	if(breakflag == 1)
	{
		return;
	}
	
		volatile int x = 0;
		unsigned int counter = ms * ((16000)/6);
		for (unsigned int i=0; i < counter;i++)
		{
			x = x+1;
		}
		return;
}



int KeypadPoll(void){
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
			state = 0;
			return temp;
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
	seconds++;
	if(seconds == 60)
	{
		minutes++;
		seconds = 0;
	}
	if(minutes == 60)
	{
		hours++;
		pumpflag = 0;
		minutes = 0;
	}
	if(hours == 24)
	{
		hours = 0;
	}
	
	if((hours%divRoundClosest(24,cups)) == 0 && pumpflag == 0)
	{
		Pump_Cup(1);
		pumpflag = 1;
		breakflag = 1;
		
	}
	
	if(hours < light)
	{
		Light_On();
		
	}
	else
	{
		Light_Off();
	}
	
}