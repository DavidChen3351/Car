#pragma once
#include "main.h"
#include <stdbool.h>
#include "kalFilter.h"

typedef struct
{
	float totalDistance; // total distance in meter
	float currentSpeed; // current speed in m/s	
	//uint16_t currentCounter;
	//uint16_t previousCounter;

	//float currentSpeed;
	//float currentAcc;
	//float lastSpeed;
	float distanceDiff; // distance gain between two calculations in meter
	//uint8_t accumCal; // accumulate calculation times, used for reset kalman filter
	Kal kal;
	bool validSpeed;
} speedStruct;

void speedIni(speedStruct *speed);
void speedCal(speedStruct *speed,float gain,float input);