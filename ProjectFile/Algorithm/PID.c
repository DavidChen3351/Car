#include "main.h"

#include "PID.h"
#include <stdbool.h>
#include "moto.h"

#define DEFAULT_Kp 0.01f
#define DEFAULT_Ki 0.01f
#define DEFAULT_Kd 0.01f
#define DEFAULT_I_Size 0.5f
#define DEFAULT_PID_SIZE 1.0f

/*
*@pram PID *pid store target ,Intergral,current and last value
*calculate PID
*Intergral is limit by I_Size
*PID_Strength is limit by PID_Size
*/
void PID_Cal(PID *pid)
{
	float P_Strength = (pid->target  - pid->currentValue) * (pid->Kp);

	float IntergralGain = (pid->target  - pid->currentValue) * (pid->Ki);
	if(IntergralGain >0 && pid->I_Strength + IntergralGain < pid->I_Size)
	{
		pid->I_Strength += IntergralGain;
	}else if(IntergralGain <0 && pid->I_Strength + IntergralGain > -pid->I_Size)
	{
		pid->I_Strength += IntergralGain;
	}

	float D_Strength  = (pid->currentValue - pid->lastValue) * (pid->Kd);
	pid->lastValue = pid->currentValue;

	float PID_Strength = P_Strength + pid->I_Strength + D_Strength;

	if(PID_Strength > pid->PID_Size)
	{
		PID_Strength = pid->PID_Size;
	}else if(PID_Strength < -pid->PID_Size)
	{
		PID_Strength = -pid->PID_Size;
	}

	pid->PID_Output = PID_Strength;
}

/*
*set the target value of PID
*/
inline void PID_SetTarget(PID *pid,float target)
{
	pid->target = target;
}

/*
*set the current wanted control value for PID calculation
*/
inline void PID_SetValue(PID *pid,float currentValue)
{
	pid->currentValue = currentValue;
}

void PID_SetParam(PID* pid,float Kp,float Ki,float Kd,float I_Size,float PID_Size)
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;
	pid->I_Size = I_Size;
	pid->PID_Size = PID_Size;
}

void PID_SetDefaultParam(PID* pid)
{
	pid->Kp = DEFAULT_Kp;
	pid->Ki = DEFAULT_Ki;
	pid->Kd = DEFAULT_Kd;
	pid->I_Size = DEFAULT_I_Size;
	pid->PID_Size = DEFAULT_PID_SIZE;
}