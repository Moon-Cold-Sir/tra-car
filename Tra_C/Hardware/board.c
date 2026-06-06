#include "ti_msp_dl_config.h"
#include "board.h"

volatile unsigned long tick_ms;
volatile uint32_t start_time;


void SysTick_Init(void)
{
    DL_SYSTICK_config(CPUCLK_FREQ/1000);
    NVIC_SetPriority(SysTick_IRQn, 0);
}


//获取SysTick当前计数值
uint32_t Systick_getTick(void)
{
	return (SysTick->VAL);
}


//毫秒级延时函数
void delay_ms(uint32_t ms)
{
	//简单循环方式延时
	//if( ms > SysTickMAX_COUNT/(SysTickFre/1000) ) ms = SysTickMAX_COUNT/(SysTickFre/1000);
	for(int i=0;i<1000;i++)
	{
		delay_us(ms);
	}
}


void delay_us(uint32_t us)
{
	if( us > SysTickMAX_COUNT/(SysTickFre/1000000) ) us = SysTickMAX_COUNT/(SysTickFre/1000000);
	
	us = us*(SysTickFre/1000000); //单位转换
	
	//记录定时器溢出时间
	uint32_t runningtime = 0;
	
	//获取当前定时器的计数值
	uint32_t InserTick = Systick_getTick();
	
	//更新实时时间
	uint32_t tick = 0;
	
	uint8_t countflag = 0;
	//等待延时完成
	while(1)
	{
		tick = Systick_getTick();//更新当前定时器计数值
		
		if( tick > InserTick ) countflag = 1;//发生了溢出,改变时间计算方式
		
		if( countflag ) runningtime = InserTick + SysTickMAX_COUNT - tick;
		else runningtime = InserTick - tick;
		
		if( runningtime>=us ) break;
	}

}
void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }

//如果要使用uart0也就是pa10 11进行数据传输的话，使用如下被注释代码
//重定向fputc函数，用于printf串口打印
// int fputc(int ch, FILE *stream)
// {
//     while( DL_UART_isBusy(UART_0_INST) == true );
//     DL_UART_Main_transmitData(UART_0_INST, ch);
//     return ch;
// }

// //重定向fputs函数
// int fputs(const char* restrict s, FILE* restrict stream) {

//     uint16_t char_len=0;
//     while(*s!=0)
//     {
//         while( DL_UART_isBusy(UART_0_INST) == true );
//         DL_UART_Main_transmitData(UART_0_INST, *s++);
//         char_len++;
//     }
//     return char_len;
// }


// int puts(const char* _ptr)
// {
//     return 0;
// }


int fputc(int ch, FILE *stream)
{
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART_BT_INST) == true );
	DL_UART_Main_transmitDataBlocking(UART_BT_INST, ch);
	return ch;
}

int fputs(const char* restrict s,FILE* restrict stream)
{
   uint16_t i,len;
   len = strlen(s);
   for(i=0;i<len;i++)
   {
       DL_UART_Main_transmitDataBlocking(UART_BT_INST,s[i]);
   }
   return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr,stdout);
    count += fputs("\n",stdout);
    return count;
}

//获取系统时钟毫秒数
int mspm0_get_clock_ms(unsigned long *count)
{
    if (!count)
        return 1;
    count[0] = tick_ms;
    return 0;
}