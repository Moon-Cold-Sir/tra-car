#include "key.h"

uint8_t Target_Cycles = 1, carmode = 1;

uint8_t Key1_GetNum(void)
{
	uint8_t KeyNum = 0;
	if ((DL_GPIO_readPins(KEY_key1_PORT, KEY_key1_PIN)&KEY_key1_PIN)!=0) 
	{
		delay_ms(20);
		while ((DL_GPIO_readPins(KEY_key1_PORT, KEY_key1_PIN)&KEY_key1_PIN) != 0);							
		KeyNum = 1;											
	}
	
	return KeyNum;
}

uint8_t Key2_GetNum(void)
{
	uint8_t KeyNum = 0;
	if ((DL_GPIO_readPins(KEY_key2_PORT, KEY_key2_PIN)&KEY_key2_PIN)==0)
	{
		delay_ms(20);
		while ((DL_GPIO_readPins(KEY_key2_PORT, KEY_key2_PIN)&KEY_key2_PIN) == 0);										
		KeyNum = 1;												
	}
	
	return KeyNum;
}

uint8_t Key3_GetNum(void)
{
	uint8_t KeyNum = 0;
	if ((DL_GPIO_readPins(KEY_key3_PORT, KEY_key3_PIN)&KEY_key3_PIN)==0)
	{
		delay_ms(20);
		while ((DL_GPIO_readPins(KEY_key3_PORT, KEY_key3_PIN)&KEY_key3_PIN) == 0);										
		KeyNum = 1;												
	}
	
	return KeyNum;
}

uint8_t Key4_GetNum(void)
{
	uint8_t KeyNum = 0;
	if ((DL_GPIO_readPins(KEY_key4_PORT, KEY_key4_PIN)&KEY_key4_PIN)==0)
	{
		delay_ms(20);
		while ((DL_GPIO_readPins(KEY_key2_PORT, KEY_key4_PIN)&KEY_key4_PIN) == 0);										
		KeyNum = 1;												
	}
	
	return KeyNum;
}