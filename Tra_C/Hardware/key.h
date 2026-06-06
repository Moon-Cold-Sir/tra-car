#ifndef _KEY_H
#define _KEY_H
#include "ti_msp_dl_config.h"
#include "board.h"
#define KEY  DL_GPIO_readPins(KEY_PORT,KEY_key1_PIN)
uint8_t click_N_Double (uint8_t time);  //按键状态扫描，双击状态扫描
uint8_t click(void);               //按键状态扫描
uint8_t Long_Press(void);           //状态扫描
void Key(void);
#endif
