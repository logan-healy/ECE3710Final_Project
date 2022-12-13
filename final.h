#include "stdint.h"

void LCD_WriteCom(unsigned char com);
void LCD_Keypad_Init(void);
void LCD_WriteData(unsigned char dat);
void delay_ms(unsigned int ms);
int KeypadPoll(void);
void SysTick_Initialize (int ticks);
void SysTick_Handler(void);
void Pump_Init(void);
void Pump_Cup(unsigned int cup);
void LCD_DisplayString(unsigned int line, unsigned char *ptr);
void Water_Prompt(void);
void Light_Prompt(void);
void run(void);
int divRoundClosest(const int n, const int d);
void Light_On(void);
void Light_Off(void);
void RaspberryPiInit(void);
void CheckParam (void);
void button_init(void);
void EXTI_Init(void);
void EXTI0_IRQHandler (void);

//TEMPERATURE SENSOR
void startTempSignal(void);
void delay_us(unsigned int us);
void tempInit(void);
void DHT_GetData(void);
char DHT11_Read(void);
uint8_t Check_Response(void);
