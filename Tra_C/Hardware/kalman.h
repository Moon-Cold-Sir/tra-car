#ifndef _kalman_H
#define _kalman_H
#include "ti_msp_dl_config.h"
#include "board.h"
extern float estEA, estEB;//estmate encoderA_cnt, now the most believably predict A speed
extern float error_estEA, error_estEB;//now predict error

float Kalman_Filter(float measurement, float *estimate, float *error_estimate);
#endif