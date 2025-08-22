#pragma once
#include "main.h"
#include "moto.h"
struct PIDs{
    float PID_Strength;
	float I_Strength;
	float targetSpeed;
};

struct speeds{
	int16_t  accumCounter;
	uint16_t accumCal ;
	uint16_t previousCounter;
	float    currentSpeed;          
	float    currentAcc;
    
	enum moto whichMoto;
	bool      ifNewSpeedCal;
	float     lastSpeed;
};

void PID_Cal(struct PIDs *pid,float currentSpeed,float currentAcc);
void speedCal(struct speeds *speed);