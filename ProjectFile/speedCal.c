#include "speedCal.h"
#include "motoConfig.h"
#include "kalFilter.h"

#include <stdbool.h>
#include "math.h"
#define SPEED_CAL_PERIOD MOTO_CONTROL_PERIOD_S //speed calculation period in seconds

/*
 *calculate speed and acceleration for moto
 */
inline void speedCal(speedStruct *speed,float gain,float input)
{
	#if MOTO_DISABLE_KAL == false
	// kal predict
	u controlInput = {{input}};
	z measure = {};
	R cov = {{0.05f}};
	kalPredict(&(speed->kal), controlInput);

	// calculate total distance and measurement
	speed->totalDistance += gain;
	measure[0][0] = speed->totalDistance;

	// kal update
	kalUpdate(&(speed->kal), measure, cov);
	speed->currentSpeed = speed->kal.x[1][0];
	speed->distanceDiff = speed->kal.x[0][0] - speed->totalDistance;
	speed->totalDistance = speed->kal.x[0][0];
	#else
		(void)input;
		speed->distanceDiff = gain;
		speed->totalDistance += gain;
		speed->currentSpeed = speed->distanceDiff / SPEED_CAL_PERIOD;
	#endif
}
//2026.2.21
//when put percise 0.25 input to my current moto with my car (disabled PID and Kal)at 
//full weight(blue battery,pc,lazer radar and a long wire connecting my jlink to my laptop),
//one moto speed stable at about 0.24,and another stable at about 0.20,so I set k = 0.22/0.25 = 0.88
//k = speed / motoInput

//(by the way it is indeed a stupid way to test.I wrote a test code to give percise 0.25 input,
//and I just staring at the speed ,which is changing around certain value.)

//(well those "certain value" is due to moto encoder,which give up and down pulse when moto spin
//and they should also be Linear too.)

//when moto drive only gear ,I got speed 0.30 0.67 0.99 at input 0.25 0.5 and 0.75,basicaly linear I guess
//my initial guess about k is 0.5,well I think it is not far from the true value
void speedIni(speedStruct *speed)
{
	speed->totalDistance = 0;
	speed->currentSpeed = 0;
	iniX x = {{0},{0}};
	iniP P = {{0,0},{0,0}};
	Qv var = 0.1f; 
	float k = 0.88f;//what a mysterious tiny number
	kalInit(&(speed->kal),x,P,var,k);
}