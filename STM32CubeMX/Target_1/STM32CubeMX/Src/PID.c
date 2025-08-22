#include "main.h"
#include "PID.h"
#include <stdbool.h>
#include "moto.h"


#define COUNTER_PER_ROUTE 13*30*4
#define MaxPercent 0.5f
#define MinPercent 0.0f

#define ConstP 0.7f
#define ConstI 0.08f
#define ConstD 0.6f
#define MAX_I  0.2f
#define MIN_I  -0.2f

#define countPerCircle 52
#define timerInterval 10 // 10 ms
#define counterARR  65535

#define MaxAccumCal 6
#define speedAlpha 0.2f
#define accAlpha 0.2f





void speedCal(struct speeds *speed)
{
	uint16_t counter  = Moto_GetCounter(speed->whichMoto);	
	speed->accumCounter = counter - speed->previousCounter;
	if(speed->accumCal < MaxAccumCal && speed->accumCounter == 0)
	{
        speed->accumCal++;
		speed->ifNewSpeedCal = false;
        return;
	}		
	if(ifEncoderElapsed(speed->whichMoto) == true)
	{
		speed->accumCounter -= counterARR;
        eraseEncoderElapsed(speed->whichMoto);
	}
	uint16_t time = ((float)timerInterval) * (float)(speed->accumCal + 1);
	
	float newspeed = ((float)speed->accumCounter) / ((float)time);	
	float lastSpeed = speed->currentSpeed;
	speed->lastSpeed = lastSpeed;
	
	speed->currentSpeed = speedAlpha*newspeed + (1.0f-speedAlpha)*lastSpeed;

	float newAcc = ((float)speed->currentSpeed - (float)speed->lastSpeed) / ((float)time);
	float lastAcc = speed->currentAcc;
	
	speed->currentAcc =    accAlpha*newAcc +     (1.0f-accAlpha)*lastAcc;
	
	speed->previousCounter = counter;
	//speed->accumCounter = 0;
	speed->accumCal = 0;
	speed->ifNewSpeedCal = true;
}
/*
@pram struct PIDs *pid store target and result of pid cal
@pram currentSpeed     speed for cal
@pram currentAcc       acc for cal
*/
void PID_Cal(struct PIDs *pid,float currentSpeed,float currentAcc)
{
	
	float P_Strength = (pid->targetSpeed  - currentSpeed) * ((float)ConstP);
	if((pid->I_Strength  > MAX_I && (pid->targetSpeed  - currentSpeed) < 0 ) || (pid->I_Strength  < MIN_I && (pid->targetSpeed - currentSpeed) > 0)  ||  (pid->I_Strength >=MIN_I && pid->I_Strength  <= MAX_I))
	{
		//pid->I_Strength  += (pid->targetSpeed  - speed->currentSpeed) * ((float)timerInterval) * ((float)ConstI);
		pid->I_Strength  += (pid->targetSpeed  - currentSpeed) * ((float)ConstI);
	}
	//float D_Strength = speed->currentAcc / ((float)timerInterval) * ((float)ConstD);
	float D_Strength = currentAcc * ((float)ConstD);
	pid->PID_Strength = P_Strength + pid->I_Strength  + D_Strength;
}

//struct PIDs PID_Set(float targetLeft,float targetRight,struct speeds *leftSpeed,struct speeds *rightSpeed)
//{
//  struct PIDs PID_Result;
//	
//	PID_Result.PID_Left  = PID_Cal(targetLeft ,leftSpeed,&I_Left_Strength );
//	PID_Result.PID_Right = PID_Cal(targetRight,rightSpeed,&I_Right_Strength);
//	return PID_Result;
//}