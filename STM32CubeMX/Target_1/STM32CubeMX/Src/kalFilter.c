#include "main.h"
#include "kalFilter.h"

#define calInterval 10.0f
void kalInit(struct kals *kal,float initV,float initVar,float r,float qAcc)
{   
     kal->vEstimate   = initV;
     kal->vVariance   = initVar;
     kal->r           = r;
     kal->accVariance = qAcc;
}

void kalPredict(struct kals* kal,float acc)
{
    kal->vPredict  = kal->vEstimate + calInterval*acc;
    kal->vVariance = kal->vVariance + calInterval*calInterval*kal->accVariance;
}

void kalUpdate(struct kals *kal,float vMeasure)
{
    kal->kalGain   = kal->vVariance / (kal->r + kal->vVariance);
    kal->vEstimate = kal->vPredict + kal->kalGain * (vMeasure - kal->vPredict);
    kal->vVariance = (1.0f-kal->kalGain) * (kal->vVariance);
}

