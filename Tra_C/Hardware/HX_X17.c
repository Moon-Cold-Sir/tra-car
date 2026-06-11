#include "HX_X17.h"
#include "ti_msp_dl_config.h"

#define SevenTrack 7

int Err_weight[SevenTrack] = {-30, -7, -3, 0, 3, 7, 30};

uint8_t sensordata[SevenTrack] = {0};
//黑1白0

//Read HX_X17 data
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

//Check is all white
uint8_t CheckIsAllWhite(void)
{
    int i;
    for(i = 0; i<SevenTrack; i++)
    {
        if(sensordata[i])
        {
            return 0;
        }
    }
    return 1;
}

//Calculate track deviation
float LineTrackingError(void)
{
    int i;
    int sum_error = 0;
    float actuall_error = 0;
    for(i = 0; i<SevenTrack; i++)
    {
        actuall_error += Err_weight[i]*sensordata[i];
        // black_num += sensordata[i];
    }
    // if(black_num)
    // {
    //     actuall_error = (float)sum_error/black_num;
    // }
    return actuall_error;
}

//Right angle detect
uint8_t TrackQua(void)
{
    if(myabs((int)LineTrackingError())>(Err_weight[6]+Err_weight[5]))
    {return 1;}
    else {return 0;}
}