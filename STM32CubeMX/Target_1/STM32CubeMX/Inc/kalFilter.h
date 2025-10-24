#pragma once

#include "main.h"

typedef struct kal_Struct {
    float kalGain;
    float vEstimate;
    float vPredict;
    float vVariance;

    float accVariance;

    float r;//measure variance

    float p;//process variance
}Kal;

void kalDefaultParams(Kal *kal);
void kalInit(Kal *kal,float initV,float initVar,float r,float qAcc);
void kalPredict(Kal *kal,float acc);
void kalUpdate(Kal *kal,float newV);