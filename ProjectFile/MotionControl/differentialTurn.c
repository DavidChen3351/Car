#include "turn.h"

void differentialTurn(turnHandle *t)
{
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
}