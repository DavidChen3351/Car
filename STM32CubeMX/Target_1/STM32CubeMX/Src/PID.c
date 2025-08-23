#include "main.h"
#include "PID.h"
#include <stdbool.h>
#include "moto.h"

#define ConstP 0.6f
#define ConstI 0.08f
#define ConstD 0.9f
#define MAX_I  0.2f
#define MIN_I  -0.2f

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