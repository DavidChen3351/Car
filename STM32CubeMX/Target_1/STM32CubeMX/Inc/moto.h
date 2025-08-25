#pragma once
#include "main.h"
#include "stdbool.h"

enum moto{
	motoLeft = 0,
	motoRight 
};

struct speeds{
	int32_t  totalCounter;
	uint16_t accumCal ;
	int32_t previousCounter;
	float    currentSpeed;          
	float    currentAcc;
    
	enum moto whichMoto;
	bool      ifNewSpeedCal;
	float     lastSpeed;
};

void motoInit();
void encoderCB_SpeedIni(uint8_t motoIndex,struct speeds *pSpeed);
void MotoActivate(float per,enum moto whichMoto);
uint16_t Moto_GetCounter(enum moto whichMoto);

float counterToAngular(uint16_t counterDelt);
float speedToAngularSpeed(float speed);
float Moto_GetAngular(enum moto whichMoto);
void speedCal(struct speeds *speed);