#include "encoder.h"
#include "ti_msp_dl_config.h"
#include "control.h"
uint32_t gpio_interrup1,gpio_interrup2;
int Get_Encoder_countA,Get_Encoder_countB;
/*******************************************************
函数功能：端口检测模块，返回信号
入口参数：无
返 回 值：无
***********************************************************/

//该代码必须在中断配置Trigger on Falling Edge(下降沿触发)
// void GROUP1_IRQHandler(void)
// {
// 	//读取检测信号
//     gpio_interrup1 = DL_GPIO_getEnabledInterruptStatus(ENCODERA_PORT,ENCODERA_E1A_PIN|ENCODERA_E1B_PIN);
//     gpio_interrup2 = DL_GPIO_getEnabledInterruptStatus(ENCODERB_PORT,ENCODERB_E2A_PIN|ENCODERB_E2B_PIN);
    
// 	//encoderA
// 	if((gpio_interrup1 & ENCODERA_E1A_PIN)==ENCODERA_E1A_PIN)
// 	{
// 		if(!DL_GPIO_readPins(ENCODERA_PORT,ENCODERA_E1B_PIN))
// 		{
// 			Get_Encoder_countA++;
// 		}
// 		else
// 		{
// 			Get_Encoder_countA--;
// 		}
// 	}
// 	else if((gpio_interrup1 & ENCODERA_E1B_PIN)==ENCODERA_E1B_PIN)
// 	{
// 		if(!DL_GPIO_readPins(ENCODERA_PORT,ENCODERA_E1A_PIN))
// 		{
// 			Get_Encoder_countA--;
// 		}
// 		else
// 		{
// 			Get_Encoder_countA++;
// 		}
// 	}
	
// 	//encoderB
// 	if((gpio_interrup2 & ENCODERB_E2A_PIN)==ENCODERB_E2A_PIN)
// 	{
// 		if(!DL_GPIO_readPins(ENCODERB_PORT,ENCODERB_E2B_PIN))
// 		{
// 			Get_Encoder_countB++;
// 		}
// 		else
// 		{
// 			Get_Encoder_countB--;
// 		}
// 	}
// 	else if((gpio_interrup2 & ENCODERB_E2B_PIN)==ENCODERB_E2B_PIN)
// 	{
// 		if(!DL_GPIO_readPins(ENCODERB_PORT,ENCODERB_E2A_PIN))
// 		{
// 			Get_Encoder_countB--;
// 		}                 
// 		else              
// 		{                 
// 			Get_Encoder_countB++;
// 		}
// 	}
// 	DL_GPIO_clearInterruptStatus(ENCODERA_PORT,ENCODERA_E1A_PIN|ENCODERA_E1B_PIN);
// 	DL_GPIO_clearInterruptStatus(ENCODERB_PORT,ENCODERB_E2A_PIN|ENCODERB_E2B_PIN);

// }

//该代码必须配置双边沿触发
void GROUP1_IRQHandler(void)
{
    // 获取中断状态
    gpio_interrup1 = DL_GPIO_getEnabledInterruptStatus(ENCODERA_PORT, ENCODERA_E1A_PIN | ENCODERA_E1B_PIN);
	gpio_interrup2 = DL_GPIO_getEnabledInterruptStatus(ENCODERB_PORT, ENCODERB_E2A_PIN | ENCODERB_E2B_PIN);
    
    // 获取当前 A、B 引脚的实际电平 (1 为高，0 为低)
    uint8_t MA_A_State = DL_GPIO_readPins(ENCODERA_PORT, ENCODERA_E1A_PIN) ? 1 : 0;
    uint8_t MA_B_State = DL_GPIO_readPins(ENCODERA_PORT, ENCODERA_E1B_PIN) ? 1 : 0;
	uint8_t MB_A_State = DL_GPIO_readPins(ENCODERB_PORT, ENCODERB_E2A_PIN) ? 1 : 0;
	uint8_t MB_B_State = DL_GPIO_readPins(ENCODERB_PORT, ENCODERB_E2B_PIN) ? 1 : 0;

    // --- Encoder A 处理 ---
    if((gpio_interrup1 & ENCODERA_E1A_PIN) == ENCODERA_E1A_PIN)
    {
        // A相发生跳变时：如果 A 和 B 电平不一致，为正转；一致为反转
        if(MA_A_State != MA_B_State) Get_Encoder_countA++;
        else Get_Encoder_countA--;
    }
    else if((gpio_interrup1 & ENCODERA_E1B_PIN) == ENCODERA_E1B_PIN)
    {
        // B相发生跳变时：如果 A 和 B 电平一致，为正转；不一致为反转
        if(MA_A_State == MA_B_State) Get_Encoder_countA++;
        else Get_Encoder_countA--;
    }

    // --- Encoder B 同理处理 ---
	if((gpio_interrup2 & ENCODERB_E2A_PIN) == ENCODERB_E2A_PIN)
    {
        // A相发生跳变时：如果 A 和 B 电平不一致，为正转；一致为反转
        if(MB_A_State != MB_B_State) Get_Encoder_countB--;
        else Get_Encoder_countB++;
    }
    else if((gpio_interrup2 & ENCODERB_E2B_PIN) == ENCODERB_E2B_PIN)
    {
        // B相发生跳变时：如果 A 和 B 电平一致，为正转；不一致为反转
        if(MB_A_State == MB_B_State) Get_Encoder_countB--;
        else Get_Encoder_countB++;
    }

	// 清除中断标志位
    DL_GPIO_clearInterruptStatus(ENCODERA_PORT, ENCODERA_E1A_PIN | ENCODERA_E1B_PIN);
	DL_GPIO_clearInterruptStatus(ENCODERB_PORT, ENCODERB_E2A_PIN | ENCODERB_E2B_PIN);

        
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    #if defined GPIO_MULTIPLE_GPIOA_INT_IIDX
            case GPIO_MULTIPLE_GPIOA_INT_IIDX:

                switch (DL_GPIO_getPendingInterrupt(GPIOA))
                {
    #if (defined GPIO_MPU6050_PORT) && (GPIO_MPU6050_PORT == GPIOA)
                    case GPIO_MPU6050_PIN_INT_IIDX:
                        Read_Quad();
                        break;
    #endif

                    default:
                        break;
                }

                break;
    #endif

    #if defined GPIO_MULTIPLE_GPIOB_INT_IIDX
            case GPIO_MULTIPLE_GPIOB_INT_IIDX:

                switch (DL_GPIO_getPendingInterrupt(GPIOB))
                {
    #if (defined GPIO_MPU6050_PORT) && (GPIO_MPU6050_PORT == GPIOB)
                    case GPIO_MPU6050_PIN_MPU6050_INT_IIDX:
                        Read_Quad();
                        break;
    #endif

                    default:
                        break;
                }

                break;
    #endif

    #if defined GPIO_MPU6050_INT_IIDX
            case GPIO_MPU6050_INT_IIDX:
                Read_Quad();
                break;
    #endif

            default:
                break;
        }
}

//Calculate the difference between the two encoders
int CalculateEncoderDiff(void)
{
    int encoder_diff = 0;
    encoder_diff = myabs(Get_Encoder_countA-Get_Encoder_countB);
    return encoder_diff;
}
