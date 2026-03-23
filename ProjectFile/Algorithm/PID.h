#pragma once

#include "main.h"


typedef struct {
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
}PID_Handle;

void PID_SetParam(PID_Handle* pid,float Kp,float Ki,float Kd,float I_Size,float PID_Size);
void PID_SetDefaultParam(PID_Handle* pid);

void PID_SetTarget(PID_Handle *pid,float target);
float PID_Cal(PID_Handle *pid,float currentValue);
