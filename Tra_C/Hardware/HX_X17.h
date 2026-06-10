#ifndef _HX_X17_H
#define _HX_X17_H
#include "ti_msp_dl_config.h"
#include "board.h"
extern uint8_t sensordata[7];
void sensortrack(void);
uint8_t CheckIsAllWhite(void);
float LineTrackingError(void);
#endif