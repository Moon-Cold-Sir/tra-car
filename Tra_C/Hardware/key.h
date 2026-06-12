#ifndef _KEY_H
#define _KEY_H
#include "ti_msp_dl_config.h"
#include "board.h"

extern uint8_t Target_Cycles;
extern uint8_t carmode;

uint8_t Key1_GetNum(void);
uint8_t Key2_GetNum(void);
uint8_t Key3_GetNum(void);
uint8_t Key4_GetNum(void);
#endif
