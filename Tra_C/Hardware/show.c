/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：5.7
修改时间：2021-04-29


Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update：2021-04-29

All rights reserved
***********************************************/
#include "show.h"
/**************************************************************************
Function: OLED display
Input   : none
Output  : none
函数功能：OLED显示
入口参数：无
返回  值：无
// **************************************************************************/
// void oled_show(void)
// {
//      memset(OLED_GRAM,0, 128*8*sizeof(u8)); //GRAM清零但不立即刷新，防止花屏
//         //=============第一行显示小车模式=======================//
//         OLED_ShowString(0,0,"Diff");
// 	    OLED_ShowString(0,10,"Bluetooth_RC");
//         //=============第四行显示左编码器PWM与读数=======================//
//                               OLED_ShowString(00,30,"L");
//         if((MotorA.Target_Encoder*1000)<0)          OLED_ShowString(16,30,"-"),
//                                                   OLED_ShowNumber(26,30,myabs((int)(MotorA.Target_Encoder*1000)),4,12);
//         if((MotorA.Target_Encoder*1000)>=0)       OLED_ShowString(16,30,"+"),
//                               OLED_ShowNumber(26,30,myabs((int)(MotorA.Target_Encoder*1000)),4,12);

//         if(MotorA.Current_Encoder<0)   OLED_ShowString(60,30,"-");
//         if(MotorA.Current_Encoder>=0)    OLED_ShowString(60,30,"+");
//                               OLED_ShowNumber(68,30,myabs((int)(MotorA.Current_Encoder*1000)),4,12);
//                                                     OLED_ShowString(96,30,"mm/s");

//         //=============第五行显示右编码器PWM与读数=======================//
//                               OLED_ShowString(00,40,"R");
//         if((MotorB.Target_Encoder*1000)<0)         OLED_ShowString(16,40,"-"),
//                                                     OLED_ShowNumber(26,40,myabs((int)(MotorB.Target_Encoder*1000)),4,12);
//         if((MotorB.Target_Encoder*1000)>=0)    		OLED_ShowString(16,40,"+"),
// 													OLED_ShowNumber(26,40,myabs((int)(MotorB.Target_Encoder*1000)),4,12);

//         if(MotorB.Current_Encoder<0)    OLED_ShowString(60,40,"-");
//         if(MotorB.Current_Encoder>=0)   OLED_ShowString(60,40,"+");
//                               OLED_ShowNumber(68,40,myabs((int)(MotorB.Current_Encoder*1000)),4,12);
//                                                     OLED_ShowString(96,40,"mm/s");

//         //=============刷新=======================//
//         OLED_Refresh_Gram();
		
// }

/**************************************************************************
Function: Send data to APP
Input   : none
Output  : none

Function:
1. Send PID parameters when requested
2. Send encoder speed data for waveform display

Waveform channels:
CH1 -> Left wheel speed
CH2 -> Right wheel speed
CH3 -> Target speed
**************************************************************************/
void APP_Show(void)
{
    int Encoder_Left_Show;
    int Encoder_Right_Show;
    int Target_Show;
    /* Convert m/s to mm/s */
    Encoder_Left_Show  = (int)(MotorA.Current_Encoder * 1000);
    Encoder_Right_Show = (int)(MotorB.Current_Encoder * 1000);
    Target_Show        = (int)(MotorA.Target_Encoder * 1000);
    /* APP requests PID parameters */
    if(PID_Send == 1)
    {
        printf("{C%d:%d:%d:%d:%d:%d:%d:%d:%d}$",(int)Velocity_KPa,(int)Velocity_KIa,
            (int)Velocity_KPb,(int)Velocity_KIb,0,0,0,0,0);
        PID_Send = 0;
    }
    else
    {
        /* Send waveform data */
        printf("{B%d:%d:%d}$",Encoder_Left_Show,Encoder_Right_Show,Target_Show);
    }
}


