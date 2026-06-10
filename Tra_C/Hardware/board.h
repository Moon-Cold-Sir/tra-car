#ifndef _BOARD_H_
#define _BOARD_H_
#include "stdio.h"
#include "string.h"
#include "ti_msp_dl_config.h"
#include "led.h"
#include "key.h"
#include "motor.h"
#include "encoder.h"
#include "oled_hardware_i2c.h"
//#include "wit.h"
#include "HX_X17.h"
#include "interrupt.h"
#include "kalman.h"

#include "control.h"
#include "uart_callback.h"
#include "show.h"

#include "mpu6050.h"

#define ABS(a)      (a>0 ? a:(-a))
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */

// Enumeration of car types
// Left and right wheel speed output
typedef enum 
{
	Mec_Car = 0, 
	Omni_Car, 
	Akm_Car, 
	Diff_Car, 
	FourWheel_Car, 
	Tank_Car
} CarMode;

//extern u8 Way_Angle;                                                     //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波
//extern int Motor_Left,Motor_Right;                                 //电机PWM变量 应是motor的 向moto致敬
extern u8 Flag_Stop;                                           //停止标志位和 显示标志位 默认停止 显示打开
//extern float Voltage,Angle_Balance,Gyro_Balance,Gyro_Turn;                           //平衡倾角 平衡陀螺仪 转向陀螺仪
//extern int Temperature;
//extern u32 Distance;                                                //超声波测距
extern u8 PID_Send;
extern float Velocity_Left,Velocity_Right;  //车轮速度(mm/s)
extern u16 test_num,show_cnt;
extern u8 Car_Mode;
extern volatile unsigned long tick_ms;

// Systick reload value, 24-bit
#define SysTickMAX_COUNT 0xFFFFFF

// Systick clock frequency
#define SysTickFre 80000000

// Convert systick clock value to time unit
#define SysTick_MS(x)  ((SysTickFre/1000U)*(uint32_t)(x))
#define SysTick_US(x)  ((SysTickFre/1000000U)*(uint32_t)(x))

uint32_t Systick_getTick(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);
int mspm0_get_clock_ms(unsigned long *count);
void mspm0_delay_ms(unsigned long ms);
#endif  /* #ifndef _BOARD_H_ */