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
inline void posCompute(pos *position)
{
	float d;
	float newAngle;
	float angleChange;
	float lastAngle;

	position->speed = (position->motoSpeed[0] + position->motoSpeed[1]) / 2.0f;
	position->theta = (position->motoSpeed[1] - position->motoSpeed[0]) / (2.0f * WHEEL_DISTANCE);
	d = (position->d[0] + position->d[1]) / 2.0f;

	angleChange = (position->d[1] - position->d[0]) / WHEEL_DISTANCE;
	lastAngle = position->angle;

	position->x += d * cosf(lastAngle + angleChange / 2.0f);
	position->y += d * sinf(lastAngle + angleChange / 2.0f);

	newAngle = fmodf(lastAngle + angleChange, 2.0f * PI);
	if (newAngle < 0.0f)
	{
		newAngle += 2.0 * PI;
	}
	position->angle = newAngle;
	position->totalDistance += d;

	//position->d[0] = 0.0f;
	//position->d[1] = 0.0f;
}

inline void posIni(pos *position)
{
	position->x = 0.0f;
	position->y = 0.0f;
	position->angle = 0.0f;
	position->totalDistance = 0.0f;
	for (motoIndex i = 0; i < MOTO_NUM; i++)
	{
		position->d[i] = 0.0f;
	}
}

inline void posSetDistance(pos *position,float diff,float speed,motoIndex i)
{
	position->d[i] = diff;
	position->motoSpeed[i] = speed;
}