#include "control.h"

u8 PID_Send;            //延时和调参相关变量
u16 test_num,show_cnt;

u8 CCD_count,ELE_count;
int Sensor_Left,Sensor_Middle,Sensor_Right,Sensor;

Encoder OriginalEncoder; 					//编码器原始数据   
Motor_parameter MotorA,MotorB;				//左右电机相关变量
float Velocity_KPb=40,Velocity_KIb=1.0,Velocity_KPa=40,Velocity_KIa=1.0;	//速度环pi,a/b reflect motorA/motorB

/**************************************************************************
Function: Get_Velocity_From_Encoder
Input   : none
Output  : none
函数功能：读取编码器和转换成速度
入口参数: 无 
返回  值：无
**************************************************************************/	
//Take four-point averaging for more stable speed readings.
void Get_Velocity_From_Encoder(int Encoder1, int Encoder2)
{
    static int LastEncoder1 = 0, LastEncoder2 = 0;
    // 使用数组记录最近4次的脉冲增量
    static int DeltaA_Buffer[4] = {0};
    static int DeltaB_Buffer[4] = {0};
    static u8 filter_index = 0;
    
    int DeltaEncoder1 = Encoder1 - LastEncoder1;
    int DeltaEncoder2 = Encoder2 - LastEncoder2;
    LastEncoder1 = Encoder1;
    LastEncoder2 = Encoder2;
    
    // 存入环形缓冲区
    DeltaA_Buffer[filter_index] = DeltaEncoder1;
    DeltaB_Buffer[filter_index] = DeltaEncoder2;
    filter_index = (filter_index + 1) % 4; // 窗口大小为4
    
    // 计算滑动总和 (相当于放大了检测窗口)
    int Sum_DeltaA = 0, Sum_DeltaB = 0;
    for(int i = 0; i < 4; i++) {
        Sum_DeltaA += DeltaA_Buffer[i];
        Sum_DeltaB += DeltaB_Buffer[i];
    }
    
    // 因为总和是4个周期的，所以计算公式中要除以4.0f
    float Encoder_A_pr = Sum_DeltaA / 4.0f;
    float Encoder_B_pr = Sum_DeltaB / 4.0f;
    
	//编码器脉冲数转换为车轮速度，单位cm/s
    MotorA.Current_Encoder = Encoder_A_pr * Frequency * Perimeter * 100 / 1560.0f;
    MotorB.Current_Encoder = Encoder_B_pr * Frequency * Perimeter * 100 / 1560.0f;
}



//运动学逆解，由x和y的速度得到编码器的速度,Vx是m/s,Vz单位是度/s(角度制)
void Get_Target_Encoder(float Vx,float Vz)
{
	float amplitude=3.5f; //Wheel target speed limit //车轮目标速度限幅
	if(Vx<0) Vz=-Vz;
	else     Vz=Vz;
	//Inverse kinematics //运动学逆解
	 MotorA.Target_Encoder = Vx - Vz * Wheelspacing / 2.0f; //计算出左轮的目标速度
	 MotorB.Target_Encoder = Vx + Vz * Wheelspacing / 2.0f; //计算出右轮的目标速度
	//Wheel (motor) target speed limit //车轮(电机)目标速度限幅
//	 MotorA.Target_Encoder=target_limit_float( MotorA.Target_Encoder,-amplitude,amplitude); 
//	 MotorB.Target_Encoder=target_limit_float( MotorB.Target_Encoder,-amplitude,amplitude); 
}


/**************************************************************************
Function: Absolute value function
Input   : a：Number to be converted
Output  : unsigned int
函数功能：绝对值函数
入口参数：a：需要计算绝对值的数
返回  值：无符号整型
**************************************************************************/
int myabs(int a)
{
	int temp;
	if(a<0)  temp=-a;
	else temp=a;
	return temp;
}


/**************************************************************************
Function: PWM_Limit
Input   : IN;max;min
Output  : OUT
函数功能：限制PWM赋值
入口参数: IN：输入参数  max：限幅最大值  min：限幅最小值 
返回  值：限幅后的值
**************************************************************************/	 	
float PWM_Limit(float IN,float max,float min)
{
	float OUT = IN;
	if(OUT>max) OUT = max;
	if(OUT<min) OUT = min;
	return OUT;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_Left (float Encoder,float Target)
{ 	
	 static float Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                					//计算偏差
	 Pwm+=Velocity_KPb*(Bias-Last_bias)+Velocity_KIb*Bias;   	//增量式PI控制器
//	 if(Pwm>7200)Pwm=7200;
//	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   					//保存上一次偏差 
	 return Pwm;                         					//增量输出
}


int Incremental_PI_Right (float Encoder,float Target)
{ 	
	 static float Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                					//计算偏差
	 Pwm+=Velocity_KPa*(Bias-Last_bias)+Velocity_KIa*Bias;   	//增量式PI控制器
//	 if(Pwm>7200)Pwm=7200;
//	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   					//保存上一次偏差 
	 return Pwm;                         					//增量输出
}

//In-place turn
void InPlaceTurn(float StartYaw,float TurnAngle)
{
	Get_Velocity_From_Encoder(Get_Encoder_countA,Get_Encoder_countB);
    float KP = 0.5f;
    float KD = 0.4f;
	
	int Yaw_PWMA = 0, Yaw_PWMB = 0;

    static float LastError = 0;

    float TargetYaw = NormalizeAngle(StartYaw + TurnAngle);

    float Error = NormalizeAngle(TargetYaw - yaw);
    float Turn_PWM = KP * Error + KD * (Error - LastError);
	LastError = Error;
	
	Turn_PWM = limit_PWM(Turn_PWM, -40, 40);
	MotorA.Target_Encoder = Turn_PWM;
	MotorB.Target_Encoder = -Turn_PWM;
	Yaw_PWMA = Incremental_PI_Right(MotorA.Current_Encoder,MotorA.Target_Encoder);// PID Control
	Yaw_PWMB = Incremental_PI_Left(MotorB.Current_Encoder,MotorB.Target_Encoder);// PID Control
	Yaw_PWMA = limit_PWM((int)Yaw_PWMA,-4000,4000);
 	Yaw_PWMB = limit_PWM((int)Yaw_PWMB,-4000,4000);
    Set_PWM(Yaw_PWMA,Yaw_PWMB);
}

//后续优化代码时，可以考虑给传感器增加一个防抖动
void CarMode1(void)
{
	static int32_t PWMA,PWMB;
	static float speedA = 40.0,speedB = 40.0;
	static int Turn_StartGEA = 0, Turn_StartGEB = 0, Delta_GEA = 0, Delta_GEB = 0;
	static uint8_t Flag_Allwhite = 0, Flag_Repeatqua = 0, Flag_Finishqua = 0;
	static float actuall_error = 0;

	sensortrack();
	actuall_error = LineTrackingError();
	Get_Velocity_From_Encoder(Get_Encoder_countA,Get_Encoder_countB);
	if(TrackQua()&&Flag_Repeatqua == 0&&Flag_Finishqua == 0)
	{
		QuaTurn_Tim++;
		Flag_Finishqua = 0;
		Flag_Repeatqua = 1;
	}
	if(CheckIsAllWhite())
	{
		if(MotorA.Last_TargetEncoder - speedA >= 0)
		{
			MotorA.Target_Encoder = speedA + 10;
			MotorB.Target_Encoder = 0;
		}
		else
		{
			MotorB.Target_Encoder = speedB + 10;
			MotorA.Target_Encoder = 0;
		}
		if(!Flag_Allwhite)
		{
			Turn_StartGEA = Get_Encoder_countA;
			Turn_StartGEB = Get_Encoder_countB;
		}
		Flag_Allwhite = 1;
	}
	else
	{
		if(Flag_Allwhite)
		{
			Delta_GEA = Get_Encoder_countA - Turn_StartGEA;
			Delta_GEB = Get_Encoder_countB - Turn_StartGEB;
			if(myabs(Delta_GEA-Delta_GEB)>=800)
			{
				MotorA.Target_Encoder = speedA-actuall_error;
				MotorB.Target_Encoder = speedB+actuall_error;
				Flag_Allwhite = 0;
				Flag_Finishqua = 1;// Turn complete = 1
			}
			else {}
		}
		else
		{
			MotorA.Target_Encoder = speedA-actuall_error;
			MotorB.Target_Encoder = speedB+actuall_error;
		}
	}
	
	if(Flag_Finishqua == 1&&sensordata[3] == 1&&Flag_Repeatqua == 1) 
	{
		Flag_Repeatqua = 0;
		Flag_Finishqua = 0;
	}
	MotorA.Last_TargetEncoder = MotorA.Target_Encoder;
	MotorB.Last_TargetEncoder = MotorB.Target_Encoder;
	PWMA = Incremental_PI_Right(MotorA.Current_Encoder,MotorA.Target_Encoder);// PID Control
	PWMB = Incremental_PI_Left(MotorB.Current_Encoder,MotorB.Target_Encoder);// PID Control
	PWMA = limit_PWM(PWMA,-4000,4000);
	PWMB = limit_PWM(PWMB,-4000,4000);
	Set_PWM(PWMA, PWMB);
}
//优化时可以考虑增加角度传感器，直接转固定角度而非延时
void CarMode2(void)
{
	static int32_t PWMA,PWMB;
	static float speedA = 40.0,speedB = 40.0;
	static int Turn_StartGEA = 0, Turn_StartGEB = 0, Delta_GEA = 0, Delta_GEB = 0, Count_Time = 0;
	static uint8_t Flag_Allwhite = 0, Flag_AllwhiteAvo = 0, Flag_Repeatqua = 0, Flag_Finishqua = 0, Obs_Avoid = 0;
	static float actuall_error = 0;

	sensortrack();
	actuall_error = LineTrackingError();
	Get_Velocity_From_Encoder(Get_Encoder_countA,Get_Encoder_countB);
	if(DistVal <= 350 && DistVal > 0 || Obs_Avoid == 1)
	{
		if(DistVal <= 350 && Obs_Avoid == 0 && DistVal > 0)
		{
			Turn_StartGEA = 0;
			Turn_StartGEB = 0;
			Delta_GEA = 0;
			Delta_GEB = 0;
			Flag_Repeatqua = 0;
			Flag_Finishqua = 0;
			Flag_Allwhite = 0;
		}
		Obs_Avoid = 1;
		if(Count_Time >= 0 && Count_Time <= 40)
		{
			MotorA.Target_Encoder = speedA-25;
			MotorB.Target_Encoder = speedB+15;
			Count_Time++;
		}
		else if(Count_Time >40 && Count_Time <= 130)
		{
			MotorA.Target_Encoder = speedA+15;
			MotorB.Target_Encoder = speedB-25;
			Count_Time++;
		}
		else if(Count_Time >130)
		{
			MotorA.Target_Encoder = speedA;
			MotorB.Target_Encoder = speedB;
		}
		if(CheckIsAllWhite())
		{
			Flag_AllwhiteAvo = 1;
		}
		if(Flag_AllwhiteAvo == 1)
		{
			if(Flag_AllwhiteAvo != CheckIsAllWhite())
			{
				Obs_Avoid = 0;
				Flag_AllwhiteAvo = 0;
				Count_Time = 0;
			}
		}
	}
	else 
	{
		if(TrackQua()&&Flag_Repeatqua == 0&&Flag_Finishqua == 0)
		{
			QuaTurn_Tim++;
			Flag_Finishqua = 0;
			Flag_Repeatqua = 1;
		}
		if(CheckIsAllWhite())
		{
			if(MotorA.Last_TargetEncoder - speedA >= 0)
			{
				MotorA.Target_Encoder = speedA + 10;
				MotorB.Target_Encoder = 0;
			}
			else
			{
				MotorB.Target_Encoder = speedB + 10;
				MotorA.Target_Encoder = 0;
			}
			if(!Flag_Allwhite)
			{
				Turn_StartGEA = Get_Encoder_countA;
				Turn_StartGEB = Get_Encoder_countB;
			}
			Flag_Allwhite = 1;
		}
		else
		{
			if(Flag_Allwhite)
			{
				Delta_GEA = Get_Encoder_countA - Turn_StartGEA;
				Delta_GEB = Get_Encoder_countB - Turn_StartGEB;
				if(myabs(Delta_GEA-Delta_GEB)>=800)
				{
					MotorA.Target_Encoder = speedA-actuall_error;
					MotorB.Target_Encoder = speedB+actuall_error;
					Flag_Allwhite = 0;
					Flag_Finishqua = 1;// Turn complete = 1
				}
				else {}
			}
			else
			{
				MotorA.Target_Encoder = speedA-actuall_error;
				MotorB.Target_Encoder = speedB+actuall_error;
			}
		}
		
		if(Flag_Finishqua == 1&&sensordata[3] == 1&&Flag_Repeatqua == 1) 
		{
			Flag_Repeatqua = 0;
			Flag_Finishqua = 0;
		}
	}
	MotorA.Last_TargetEncoder = MotorA.Target_Encoder;
	MotorB.Last_TargetEncoder = MotorB.Target_Encoder;
	PWMA = Incremental_PI_Right(MotorA.Current_Encoder,MotorA.Target_Encoder);// PID Control
	PWMB = Incremental_PI_Left(MotorB.Current_Encoder,MotorB.Target_Encoder);// PID Control
	PWMA = limit_PWM(PWMA,-4000,4000);
	PWMB = limit_PWM(PWMB,-4000,4000);
	Set_PWM(PWMA, PWMB);
}

void CarMode3(void)
{
	static float StartYaw = 0.0;
	static int tem = 0;
	if(tem == 0)
	{
		StartYaw = yaw;
		tem++;
	}
	InPlaceTurn(StartYaw,-135.0);
	// static int32_t PWMA,PWMB;
	// static float speedA = 40.0,speedB = 40.0;
	// static int Turn_StartGEA = 0, Turn_StartGEB = 0, Delta_GEA = 0, Delta_GEB = 0;
	// static uint8_t Flag_Allwhite = 0, Flag_Repeatqua = 0, Flag_Finishqua = 0;
	// static float actuall_error = 0;

	// sensortrack();
	// actuall_error = LineTrackingError();
	// Get_Velocity_From_Encoder(Get_Encoder_countA,Get_Encoder_countB);
	// if(TrackQua()&&Flag_Repeatqua == 0&&Flag_Finishqua == 0)
	// {
	// 	QuaTurn_Tim++;
	// 	Flag_Finishqua = 0;
	// 	Flag_Repeatqua = 1;
	// }
	// if(CheckIsAllWhite())
	// {
	// 	if(MotorA.Last_TargetEncoder - speedA >= 0)
	// 	{
	// 		MotorA.Target_Encoder = speedA + 10;
	// 		MotorB.Target_Encoder = 0;
	// 	}
	// 	else
	// 	{
	// 		MotorB.Target_Encoder = speedB + 10;
	// 		MotorA.Target_Encoder = 0;
	// 	}
	// 	if(!Flag_Allwhite)
	// 	{
	// 		Turn_StartGEA = Get_Encoder_countA;
	// 		Turn_StartGEB = Get_Encoder_countB;
	// 	}
	// 	Flag_Allwhite = 1;
	// }
	// else
	// {
	// 	if(Flag_Allwhite)
	// 	{
	// 		Delta_GEA = Get_Encoder_countA - Turn_StartGEA;
	// 		Delta_GEB = Get_Encoder_countB - Turn_StartGEB;
	// 		if(myabs(Delta_GEA-Delta_GEB)>=800)
	// 		{
	// 			MotorA.Target_Encoder = speedA-actuall_error;
	// 			MotorB.Target_Encoder = speedB+actuall_error;
	// 			Flag_Allwhite = 0;
	// 			Flag_Finishqua = 1;// Turn complete = 1
	// 		}
	// 		else {}
	// 	}
	// 	else
	// 	{
	// 		MotorA.Target_Encoder = speedA-actuall_error;
	// 		MotorB.Target_Encoder = speedB+actuall_error;
	// 	}
	// }
	
	// if(Flag_Finishqua == 1&&sensordata[3] == 1&&Flag_Repeatqua == 1) 
	// {
	// 	Flag_Repeatqua = 0;
	// 	Flag_Finishqua = 0;
	// }
	// MotorA.Last_TargetEncoder = MotorA.Target_Encoder;
	// MotorB.Last_TargetEncoder = MotorB.Target_Encoder;
	// PWMA = Incremental_PI_Right(MotorA.Current_Encoder,MotorA.Target_Encoder);// PID Control
	// PWMB = Incremental_PI_Left(MotorB.Current_Encoder,MotorB.Target_Encoder);// PID Control
	// PWMA = limit_PWM(PWMA,-4000,4000);
	// PWMB = limit_PWM(PWMB,-4000,4000);
	// Set_PWM(PWMA, PWMB);
}
