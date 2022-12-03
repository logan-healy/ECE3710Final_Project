void LCD_WriteCom(unsigned char com);
void LCD_Keypad_Init(void);
void LCD_WriteData(unsigned char dat);
void delay_ms(unsigned int ms);
void KeypadPoll(int *value);
void SysTick_Initialize (int ticks);
void SysTick_Handler(void);
void Pump_Init(void);
void Pump_Cup(unsigned int cup);
void LCD_DisplayString(unsigned int line, unsigned char *ptr);
void Water_Prompt(void);
void Light_Prompt(void);
void run(void);
int divRoundClosest(const int n, const int d);

