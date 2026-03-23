#include "pos.h"
#include "motoConfig.h"
#include "moto.h"

#include <math.h>
#include <stdbool.h>
#define POS_COMPUTE_PERIOD MOTO_CONTROL_PERIOD_S //position compute period in seconds

/*
 *this function compute position xy based on distance gain drived from 2 motos
 *new position is accumlated on previous position 
 */
inline void posCompute(positionHandle *pos)
{
	float wheelDistanceGain[2];
	wheelDistanceGain[0] = pos->distanceGain[0];
	wheelDistanceGain[1] = pos->distanceGain[1];
	float carDistanceGain;
	float newAngle;
	float angleChange;
	float lastAngle;

	pos->speed = (pos->motoSpeed[0] + pos->motoSpeed[1]) / 2.0f;
	pos->theta = (pos->motoSpeed[1] - pos->motoSpeed[0]) / (2.0f * WHEEL_DISTANCE);
	carDistanceGain = (wheelDistanceGain[0] + wheelDistanceGain[1]) / 2.0f;

	angleChange = (wheelDistanceGain[1] - wheelDistanceGain[0]) / WHEEL_DISTANCE;
	lastAngle = pos->angle;

	pos->x += carDistanceGain * cosf(lastAngle + angleChange / 2.0f);
	pos->y += carDistanceGain * sinf(lastAngle + angleChange / 2.0f);

	newAngle = fmodf(lastAngle + angleChange, 2.0f * PI);
	if (newAngle < 0.0f)
	{
		newAngle += 2.0 * PI;
	}
	pos->angle = newAngle;
	pos->totalDistance += carDistanceGain;

	pos->distanceGain[0] = 0.0f;
	pos->distanceGain[1] = 0.0f;
}

inline void posInit(positionHandle *pos)
{
	pos->x = 0.0f;
	pos->y = 0.0f;
	pos->angle = PI / 2.0f;
	pos->totalDistance = 0.0f;
	for (uint8_t i = 0; i < MOTO_NUM; i++)
	{
		pos->distanceGain[i] = 0.0f;
	}
}

void posSetDistance(positionHandle *pos,float speed[2],float distanceGain[2])
{
	for(int i=0;i<2;i++)
	{
		pos->motoSpeed[i] = speed[i];
		pos->distanceGain[i] += distanceGain[i];
	}
}