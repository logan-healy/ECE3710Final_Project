void LCD_WriteCom(unsigned char com);
void LCD_Keypad_Init(void);
void LCD_WriteData(unsigned char dat);
void delay_ms(unsigned int ms);
void KeypadPoll(void);
void SysTick_Initialize (int ticks);
void SysTick_Handler(void);

