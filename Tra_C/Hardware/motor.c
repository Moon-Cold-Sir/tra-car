#include "motor.h"
float Velcity_Kp=10.0,  Velcity_Ki=0.4,  Velcity_Kd=0;
/***********************************************
公司名称：武汉轮迹科技有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通店铺：https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改日期：2024-07-19

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2024-07-19

All rights reserved
***********************************************/
void Motor_Init(void)
{
  Stop_Car();
}

int limit_PWM(int value,int low,int high)
{
	if(value>high) return high;
	else if(value<low) return low;
	else return value;
}


void Set_PWM(int pwmA,int pwmB)
{
	 if(pwmA>0)
    {
      DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
      DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
		  DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmA),GPIO_PWM_0_C0_IDX);
    }
    else
    {
      DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);
      DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN);
		  DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmA),GPIO_PWM_0_C0_IDX);
    }
    if(pwmB>0)
    {
		  DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
      DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
      DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmB),GPIO_PWM_0_C1_IDX);
    }
    else
    {
		  DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
      DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
		  DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmB),GPIO_PWM_0_C1_IDX);
    }
}

// Output limit (-7999 ~ 7999), frequency 10khz
void Stop_Car(void)
{
  Set_PWM(0,0);
}
