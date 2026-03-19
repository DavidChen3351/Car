#include "motoConfig.h"

typedef struct 
{
    float forwardPer;
    float turnPer;
    float targetPer[MOTO_NUM];
} turnHandle;

void differentialTurn(turnHandle *t);
#if MOTO_NUM == 2
void oneSideTurn(turnHandle *t);
#endif