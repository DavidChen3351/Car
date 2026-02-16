#include "speedCal.h"
#include "motoConfig.h"
#include "kalFilter.h"

#include <stdbool.h>
#include "math.h"
#define SPEED_CAL_PERIOD MOTO_CONTROL_PERIOD_S //speed calculation period in seconds

int32_t speedCalRawDiff(uint16_t current, uint16_t prev);
float counterDiffToMeter(int32_t counter);
/*
 *this function calculate the raw difference between two counters, considering overflow
 */
inline int32_t speedCalRawDiff(uint16_t current, uint16_t prev)
{
	int32_t delta = (int32_t)current - (int32_t)prev;
	if (delta > (COUNTER_ARR / 2))
	{
		delta -= (COUNTER_ARR + 1); // positive overflow
	}
	else if (delta < -(COUNTER_ARR / 2))
	{
		delta += (COUNTER_ARR + 1); // negative overflow
	}
	return delta;
}

/*
 *calculate speed and acceleration for moto
 */
inline void speedCal(speedStruct *speed,uint16_t counter)
{
	// kal predict
	kalPredict(&(speed->kal), speed->currentAcc);

	// calculate difference of counter change
	int32_t counterDiff = speedCalRawDiff(counter, speed->previousCounter);

	// if difference is zero,accumlate times util reach max accumlate times
	if (speed->accumCal < MAX_ACCUM_CAL && counterDiff == 0)
	{
		speed->accumCal++;
		speed->validSpeed = false;
		return;
	}

	// program reach here if
	// 1.accumulateCal is zero and nonzero difference
	// 2.accumulateCal reach max,even if difference is zero.

	// calculate iteration times based on accumlate calculation
	uint8_t iterationCount = speed->accumCal + 1;
	float iterationTime = ((float)iterationCount) * SPEED_CAL_PERIOD;

	// calculate speed
	float diff = counterDiffToMeter(counterDiff);
	float newSpeed = diff / iterationTime ;

	// lastSpeed comes from kal,while currentSpeed does not
	// speed->currentAcc = (speed->currentSpeed - speed->lastSpeed) / (time);

	// kal update,get filtered speed
	kalUpdate(&(speed->kal), newSpeed);
	speed->currentSpeed = speed->kal.vEstimate;
	speed->currentAcc = (speed->currentSpeed - speed->lastSpeed) / ((float)iterationCount);
	speed->distanceDiff = speed->currentSpeed * iterationTime;
	// prepare for next calculation
	// 1.update last speed
	// 2.update previous counter
	// 3.reset accumlate cal
	speed->lastSpeed = speed->currentSpeed;
	speed->previousCounter = counter;
	speed->accumCal = 0;
	speed->validSpeed = true;
}

inline float counterDiffToMeter(int32_t counter)
{
    return((float)counter / (float)COUNTER_PER_ROUTE * WHEEL_CIRCUMFERENCE);
}

void speedIni(speedStruct *speed)
{
	speed->currentAcc = 0.0f;
	speed->currentSpeed = 0.0f;
	speed->previousCounter = 0;
	speed->accumCal = 0;
	speed->validSpeed = false;
	kalDefaultParams(&(speed->kal));
}