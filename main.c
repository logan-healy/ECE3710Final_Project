#include "stm32l476xx.h"
#include "final.h"


int main()
{
	LCD_Keypad_Init();
	delay_ms(5);
	Water_Prompt();
	delay_ms(5);
	Light_Prompt();
	//while(1);
}

