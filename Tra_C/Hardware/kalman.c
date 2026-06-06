#include "HX_X17.h"
#include "ti_msp_dl_config.h"
float Q = 0.03,R = 3;
float estEA = 0, estEB = 0;//estmate encoderA_cnt, now the most believably predict A speed
float error_estEA = 1, error_estEB = 1;//now predict error
float Kalman_Filter(float measurement, float *estimate, float *error_estimate)
{
    *error_estimate = *error_estimate + Q;// 更新误差
    float kalman_gain = *error_estimate / (*error_estimate + R);// 卡尔曼增益
    *estimate = *estimate + kalman_gain * (measurement - *estimate);// 更新估计值
    *error_estimate = (1.0f - kalman_gain) * (*error_estimate);// 更新误差
    
    return *estimate;
}

typedef struct {
    float v;        // 估计速度 (最优估计)
    float a;        // 估计加速度 (内部状态)
    float P[2][2];  // 误差协方差矩阵 (2x2)
    
    // 系统参数
    float Q_v;      // 速度过程噪声方差
    float Q_a;      // 加速度过程噪声方差
    float R;        // 测量噪声方差 (编码器量化误差)
    float dt;       // 控制周期 (秒)，例如 10ms = 0.01f
} Kalman2_Filter_t;

// 实例化左右电机的卡尔曼滤波器
Kalman2_Filter_t KF_Left;
Kalman2_Filter_t KF_Right;

// 初始化函数
void Kalman2_Init(Kalman2_Filter_t *kf, float dt)
{
    kf->v = 0.0f;
    kf->a = 0.0f;
    kf->P[0][0] = 1.0f; kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f; kf->P[1][1] = 1.0f;
    
    kf->dt = dt;
    kf->Q_v = 0.01f;   // 调参：速度受外部干扰的方差
    kf->Q_a = 0.1f;    // 调参：加速度波动的方差 (小车加减速越猛，此值需要越大)
    kf->R   = 5.0f;    // 调参：由于编码器存在 +-3 的跳变，R值适当给大一点信任预测
}

// 二阶卡尔曼核心更新函数
// 入口参数：kf(滤波器实例指针)，measurement(当前读到的速度测量值)
// 返回值：滤波后的平滑速度
float Kalman2_Update(Kalman2_Filter_t *kf, float measurement)
{
    float dt = kf->dt;
    
    // ================= 1. 预测阶段 (Predict) =================
    // 状态预测：v_pred = v + a*dt, a_pred = a
    float v_pred = kf->v + kf->a * dt;
    float a_pred = kf->a;
    
    // 误差协方差矩阵预测 P_pred = F * P * F^T + Q
    float P00_pred = kf->P[0][0] + dt * (kf->P[1][0] + kf->P[0][1]) + dt * dt * kf->P[1][1] + kf->Q_v;
    float P01_pred = kf->P[0][1] + dt * kf->P[1][1];
    float P10_pred = kf->P[1][0] + dt * kf->P[1][1];
    float P11_pred = kf->P[1][1] + kf->Q_a;
    
    // ================= 2. 更新阶段 (Update) =================
    // 计算卡尔曼增益 K = P_pred * H^T / (H * P_pred * H^T + R)
    // 因为只测量速度，观测矩阵 H = [1, 0]
    float S = P00_pred + kf->R; 
    float K0 = P00_pred / S;    // 速度增益
    float K1 = P10_pred / S;    // 加速度增益
    
    // 计算测量残差 (实际测量值 - 预测值)
    float y = measurement - v_pred;
    
    // 更新最优状态估计 X = X_pred + K * y
    kf->v = v_pred + K0 * y;
    kf->a = a_pred + K1 * y;
    
    // 更新协方差矩阵 P = (I - K * H) * P_pred
    kf->P[0][0] = (1.0f - K0) * P00_pred;
    kf->P[0][1] = (1.0f - K0) * P01_pred;
    kf->P[1][0] = -K1 * P00_pred + P10_pred;
    kf->P[1][1] = -K1 * P01_pred + P11_pred;
    
    return kf->v;
}