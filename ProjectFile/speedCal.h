#pragma once
#include "main.h"
#include <stdbool.h>
#include "kalFilter.h"

typedef struct
{
	float currentSpeed; // current speed in m/s	
	float totalDistance; // total distance in meter
	float distanceDiff; // distance gain between two calculations in meter
	Kal kal;
	//uint16_t currentCounter;
	//uint16_t previousCounter;

	//float currentSpeed;
	//float currentAcc;
	//float lastSpeed;
	
	//uint8_t accumCal; // accumulate calculation times, used for reset kalman filter
	
	//bool validSpeed;
} speedHandle;

void speedIni(speedHandle *speed);
void speedCal(speedHandle *speed,float gain,float input);