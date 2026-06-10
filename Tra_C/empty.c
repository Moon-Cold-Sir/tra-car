/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "board.h"

int32_t PWMA,PWMB;
float speedA = 50.0,speedB = 50.0;
u8 Flag_Stop = 1;
int quaTurn_Tim = -1;

int tem2 = 0,tem4 = 0;
float actuall_error = 0;

//蓝牙通信配置了串口1(bound:9600)+DMA，用于防止编码器中断+定时中断过多导致接收的蓝牙数据丢失
int main(void)
{
	int i=0;
    SYSCFG_DL_init();

	MPU6050_Init();
	Interrupt_Init();

	//Start the timer.
	//If you check the "start timer" option in the configuration
	//it will repeatedly loop without needing to be started.
	DL_Timer_startCounter(PWM_0_INST);

	NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_ClearPendingIRQ(ENCODERB_INT_IRQN);
	NVIC_ClearPendingIRQ(UART_BT_INST_INT_IRQN);
	NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(ENCODERB_INT_IRQN);
	NVIC_EnableIRQ(UART_BT_INST_INT_IRQN);

	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);;
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);

	BT_DAMConfig();
	OLED_Init();
	
    while (1) 
    {
		// Save the last deviation value
		//printf("%d  %d\n\r",encoderA_cnt,encoderB_cnt);
		sensortrack();
		OLED_ShowString(2,1,(uint8_t *)"EA:",8);
		OLED_ShowNum(20,1,MotorA.Current_Encoder,5,8);
		OLED_ShowString(2,2,(uint8_t *)"EB:",8);
		OLED_ShowNum(20,2,MotorB.Current_Encoder,5,8);
		OLED_ShowString(2,3,(uint8_t *)"sen:",8);
		OLED_ShowNum(25,3,sensordata[0],2,8);
		OLED_ShowNum(38,3,sensordata[1],2,8);
		OLED_ShowNum(51,3,sensordata[2],2,8);
		OLED_ShowNum(64,3,sensordata[3],2,8);
		OLED_ShowNum(77,3,sensordata[4],2,8);
		OLED_ShowNum(90,3,sensordata[5],2,8);
		OLED_ShowNum(103,3,sensordata[6],2,8);
		if(Key2_GetNum() == 1)
		{
			DL_TimerG_startCounter(TIMER_0_INST);
			
			while(1)
			{
				OLED_ShowString(2,1,(uint8_t *)"EA:",8);
				OLED_ShowNum(20,1,MotorA.Current_Encoder,5,8);
				OLED_ShowString(2,2,(uint8_t *)"EB:",8);
				OLED_ShowNum(20,2,MotorB.Current_Encoder,5,8);
				OLED_ShowString(2,3,(uint8_t *)"sen:",8);
				OLED_ShowNum(25,3,sensordata[0],2,8);
				OLED_ShowNum(38,3,sensordata[1],2,8);
				OLED_ShowNum(51,3,sensordata[2],2,8);
				OLED_ShowNum(64,3,sensordata[3],2,8);
				OLED_ShowNum(77,3,sensordata[4],2,8);
				OLED_ShowNum(90,3,sensordata[5],2,8);
				OLED_ShowNum(103,3,sensordata[6],2,8);
				if(Key2_GetNum() == 1)
				{
					DL_TimerG_stopCounter(TIMER_0_INST);
					Stop_Car();
					while(1);
				}
			}
		}
    }
}

// 10ms timer interrupt
void TIMER_0_INST_IRQHandler(void)
{
    if(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
    {
        if(DL_TIMER_IIDX_ZERO)
        {
			sensortrack();
			actuall_error = LineTrackingError();
			Get_Velocity_From_Encoder(Get_Encoder_countA,Get_Encoder_countB);
			if(CheckIsAllWhite())
			{
			}
			else
			{
				MotorA.Target_Encoder = speedA-actuall_error;
				MotorB.Target_Encoder = speedB+actuall_error;
			}
			PWMA = Incremental_PI_Right(MotorA.Current_Encoder,MotorA.Target_Encoder);// PID Control
			PWMB = Incremental_PI_Left(MotorB.Current_Encoder,MotorB.Target_Encoder);// PID Control
			PWMA = limit_PWM(PWMA,-4000,4000);
			PWMB = limit_PWM(PWMB,-4000,4000);
			Set_PWM(PWMA, PWMB);
		}
    }
}

void TIMER_1_INST_IRQHandler(void)
{
	if(DL_TimerA_getPendingInterrupt(TIMER_1_INST))
    {
		BTBufferHandler();//处理接收到的蓝牙数据
		APP_Show();
	}
}
