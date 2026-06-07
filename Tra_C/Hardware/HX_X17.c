#include "HX_X17.h"
#include "ti_msp_dl_config.h"
uint8_t sensordata[7] = {0};
uint8_t All_White = 0;
//黑1白0
void sensortrack(void)
{
    sensordata[0] = (DL_GPIO_readPins(HX_X17_L3_PORT, HX_X17_L3_PIN) != 0) ? 1 : 0;
    sensordata[1] = (DL_GPIO_readPins(HX_X17_L2_PORT, HX_X17_L2_PIN) != 0) ? 1 : 0;
    sensordata[2] = (DL_GPIO_readPins(HX_X17_L1_PORT, HX_X17_L1_PIN) != 0) ? 1 : 0;
    sensordata[3] = (DL_GPIO_readPins(HX_X17_M0_PORT, HX_X17_M0_PIN) != 0) ? 1 : 0;
    sensordata[4] = (DL_GPIO_readPins(HX_X17_R1_PORT, HX_X17_R1_PIN) != 0) ? 1 : 0;
    sensordata[5] = (DL_GPIO_readPins(HX_X17_R2_PORT, HX_X17_R2_PIN) != 0) ? 1 : 0;
    sensordata[6] = (DL_GPIO_readPins(HX_X17_R3_PORT, HX_X17_R3_PIN) != 0) ? 1 : 0;
}