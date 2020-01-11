#ifndef __PH_H
#define __PH_H

#include "stdint.h"

typedef struct{
		float _temperature ;
    float _phValue      ; // = 7.0;
    float _acidVoltage   ;// = 2032.44;    //buffer solution 4.0 at 25C
    float _neutralVoltage ;//= 1500.0;     //buffer solution 7.0 at 25C
    float _voltage       ;// = 1500.0;	
} PH_InitTypeDef;

extern PH_InitTypeDef PH_InitStructure;

float ReadPH(float voltage, PH_InitTypeDef PH_InitStructure);
void InitPH(PH_InitTypeDef *PH_InitStructure);
void phCalibration(PH_InitTypeDef PH_InitStructure, float voltage);


#endif
