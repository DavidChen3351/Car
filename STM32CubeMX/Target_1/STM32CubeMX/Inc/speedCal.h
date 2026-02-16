#pragma once
#include "main.h"
#include <stdbool.h>
#include "kalFilter.h"

typedef struct
{
	uint16_t accumCal;
	uint16_t currentCounter;
	uint16_t previousCounter;

	float currentSpeed;
	float currentAcc;
	float lastSpeed;
	float distanceDiff; // distance gain between two calculations

	Kal kal;
	bool validSpeed;
} speedStruct;

void speedIni(speedStruct *speed);
void speedCal(speedStruct *speed,uint16_t counter);