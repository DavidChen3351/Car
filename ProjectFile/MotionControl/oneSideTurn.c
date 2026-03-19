#include "turn.h"

#define MAX_OVERFLOW 1.0f
#define MIN_OVERFLOW -1.0f
float overFlow(float *value)
{
	float overflow = 0.0f;
	if (*value > MAX_OVERFLOW)
	{
		overflow = *value - MAX_OVERFLOW;
		*value = MAX_OVERFLOW;
	}
	else if (*value < MIN_OVERFLOW)
	{
		overflow = MIN_OVERFLOW - *value;
		*value = MIN_OVERFLOW;
	}
	return overflow;
}

void oneSideTurn(turnHandle *t)
{
	t->targetPer[0] = t->forwardPer + t->turnPer;
	t->targetPer[1] = t->forwardPer - t->turnPer;
	float leftOverFlow = overFlow(&t->targetPer[0]);
	float rightOverFlow = overFlow(&t->targetPer[1]);
	t->targetPer[0] -= rightOverFlow;
	t->targetPer[1] -= leftOverFlow;
}