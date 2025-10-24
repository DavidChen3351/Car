#include "main.h"
#include "kalFilter.h"

#define calInterval 10.0f
#define DEFAULT_INIT_V 0.0f
#define DEFAULT_INIT_VAR 1.0f
#define DEFAULT_R 0.5f
#define DEFAULT_Q_ACC 0.1f

void kalInit(Kal *kal,float initV,float initVar,float r,float qAcc)
{   
     kal->vEstimate   = initV;
     kal->vVariance   = initVar;
     kal->r           = r;
     kal->accVariance = qAcc;
}

void kalDefaultParams(Kal *kal)
{
    kal->vEstimate   = DEFAULT_INIT_V;
    kal->vVariance   = DEFAULT_INIT_VAR;
    kal->r           = DEFAULT_R;
    kal->accVariance = DEFAULT_Q_ACC;
}

/*
*@para kal pointer to kal structure
*@para acc acceleration input
*/
void kalPredict(Kal* kal,float acc)
{
    kal->vPredict  = kal->vEstimate + calInterval*acc;
    kal->vVariance = kal->vVariance + calInterval*calInterval*kal->accVariance;
}

void kalUpdate(Kal *kal,float vMeasure)
{
    kal->kalGain   = kal->vVariance / (kal->r + kal->vVariance);
    kal->vEstimate = kal->vPredict + kal->kalGain * (vMeasure - kal->vPredict);
    kal->vVariance = (1.0f-kal->kalGain) * (kal->vVariance);
}

