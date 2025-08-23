#pragma once
#include "main.h"
#include "moto.h"
struct PIDs{
   float PID_Strength;
	float I_Strength;
	float targetSpeed;
};



void PID_Cal(struct PIDs *pid,float currentSpeed,float currentAcc);
