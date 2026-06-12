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

u8 Flag_Stop = 0;

uint8_t oled_buffer[32];

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
		DistVal = Read_Ultrasonic();
		OLED_ShowString(2,1,(uint8_t *)"EA:",8);
		OLED_ShowNum(5*4,1,MotorA.Current_Encoder,5,8);
		OLED_ShowString(60,1,(uint8_t *)"EB:",8);
		OLED_ShowNum(78,1,MotorB.Current_Encoder,5,8);
		OLED_ShowString(2,2,(uint8_t *)"sen:",8);
		OLED_ShowNum(25,2,sensordata[0],2,8);
		OLED_ShowNum(38,2,sensordata[1],2,8);
		OLED_ShowNum(51,2,sensordata[2],2,8);
		OLED_ShowNum(64,2,sensordata[3],2,8);
		OLED_ShowNum(77,2,sensordata[4],2,8);
		OLED_ShowNum(90,2,sensordata[5],2,8);
		OLED_ShowNum(103,2,sensordata[6],2,8);
		OLED_ShowString(2,3,(uint8_t *)"cyc:",8);
		OLED_ShowNum(25,3,Target_Cycles,2,8);
		OLED_ShowString(60,3,(uint8_t *)"mode:",8);
		OLED_ShowNum(90,3,carmode,2,8);
		OLED_ShowString(2,4,(uint8_t *)"yaw:",8);
		sprintf((char *)oled_buffer, "%-6.1f", yaw);
        OLED_ShowString(5*5,4,oled_buffer,8);
		OLED_ShowString(2,5,(uint8_t *)"Dist:",8);
		sprintf((char *)oled_buffer, "%4u", DistVal);
		OLED_ShowString(5*6,5,oled_buffer,8);

		if(Key3_GetNum() == 1)
		{
			Target_Cycles++;
			if(Target_Cycles>5)
			{
				Target_Cycles = 1;
			}
		}
		if(Key4_GetNum() == 1)
		{
			carmode++;
			if(carmode>3)
			{
				carmode = 1;
			}
		}
		if(Key2_GetNum() == 1)
		{
			OLED_Clear();
			DL_TimerG_startCounter(TIMER_0_INST);
			while(1)
			{
				DistVal = Read_Ultrasonic();
				OLED_ShowString(2,1,(uint8_t *)"EA:",8);
				OLED_ShowNum(5*4,1,MotorA.Current_Encoder,5,8);
				OLED_ShowString(60,1,(uint8_t *)"EB:",8);
				OLED_ShowNum(78,1,MotorB.Current_Encoder,5,8);
				OLED_ShowString(2,2,(uint8_t *)"sen:",8);
				OLED_ShowNum(25,2,sensordata[0],2,8);
				OLED_ShowNum(38,2,sensordata[1],2,8);
				OLED_ShowNum(51,2,sensordata[2],2,8);
				OLED_ShowNum(64,2,sensordata[3],2,8);
				OLED_ShowNum(77,2,sensordata[4],2,8);
				OLED_ShowNum(90,2,sensordata[5],2,8);
				OLED_ShowNum(103,2,sensordata[6],2,8);
				OLED_ShowString(2,3,(uint8_t *)"cyc:",8);
				OLED_ShowNum(25,3,Target_Cycles,2,8);
				OLED_ShowString(60,3,(uint8_t *)"mode:",8);
				OLED_ShowNum(90,3,carmode,2,8);
				OLED_ShowString(2,4,(uint8_t *)"yaw:",8);
				sprintf((char *)oled_buffer, "%-6.1f", yaw);
				OLED_ShowString(5*5,4,oled_buffer,8);
				OLED_ShowString(2,5,(uint8_t *)"Dist:",8);
				sprintf((char *)oled_buffer, "%4u", DistVal);
				OLED_ShowString(5*6,5,oled_buffer,8);
				//此处如果要是想要停止的更加干脆，可以简单一点让QuaTurn_Tim/4==Target_Cycles-1&&QuaTurn_Tim%3==1时速度全部除以二
				if(Key2_GetNum() == 1 || (QuaTurn_Tim/4==Target_Cycles))
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
			switch (carmode)
			{
				case 1: CarMode1(); break;
				case 2: CarMode2(); break;
				case 3: CarMode3(); break;
			}
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
