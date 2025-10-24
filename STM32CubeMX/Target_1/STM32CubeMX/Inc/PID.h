#pragma once

#include "main.h"


typedef struct PID_Struct{
	float I_Strength;
	float I_Size;
	float PID_Size;
		
	float Kp;
	float Ki;
	float Kd;
	
	float target;
	float currentValue;
	float lastValue;

	float PID_Output;
}PID;

void PID_SetParam(PID* pid,float Kp,float Ki,float Kd,float I_Size,float PID_Size);
void PID_SetDefaultParam(PID* pid);

void PID_SetTarget(PID *pid,float target);
void PID_SetValue(PID *pid,float currentValue);
float PID_Cal(PID *pid);
