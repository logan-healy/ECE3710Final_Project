#include "stm32l476xx.h"
#include "final.h"


int main()
{
	LCD_Keypad_Init();
	delay_ms(5);
	Pump_Init();
	delay_ms(5);
	RaspberryPiInit();
	delay_ms(5);
	tempInit();
	delay_ms(5);
	Water_Prompt();
	delay_ms(5);
	Light_Prompt();
	delay_ms(5);
	DHT_GetData();
	delay_ms(5);
	button_init();
	EXTI_Init();
	SysTick_Initialize(8000);
	run();
}

