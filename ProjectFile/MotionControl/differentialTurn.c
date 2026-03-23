#include <stdint.h>
#include "motoConfig.h"

void differentialTurn(float vel, float omega, float motoTarget[2])
{
    // float R = vel / omega;//radius
    float d = WHEEL_DISTANCE / 2.0f; // half of two wheel distance

    motoTarget[0] = vel + d * omega;
    motoTarget[1] = vel - d * omega;
}

/*
    if (t->turnPer != 0)
    {
        if (t->turnPer > 0)
        {
            t->targetPer[0] = t->forwardPer;
            t->targetPer[1] = t->forwardPer * (1.0f - t->turnPer);
        }
        else
        {
            t->turnPer = -t->turnPer;
            t->targetPer[0] = (1.0f - t->turnPer) * t->forwardPer;
            t->targetPer[1] = t->forwardPer;
        }
    }
    else
    {
        t->targetPer[0] = t->forwardPer;
        t->targetPer[1] = t->forwardPer;
    }
*/