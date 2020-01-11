#ifndef _KALMAN_H_
#define _KALMAN_H_

#include "stdint.h"
#include "math.h"
#include "stdlib.h"

void SimpleKalmanFilter(float mea_e, float est_e, float q);
float updateEstimate(float mea);

void setMeasurementError(float mea_e);
void setEstimateError(float est_e);
void setProcessNoise(float q);

float getKalmanGain(void);
float getEstimateError(void);

#endif
