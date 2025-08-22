#pragma once
#include "main.h"
#include "stdbool.h"

enum moto{
	motoLeft = 0,
	motoRight 
};

void motoInit();
bool ifEncoderElapsed(enum moto whichMoto);
void MotoActivate(float per,enum moto whichMoto);
uint16_t Moto_GetCounter(enum moto whichMoto);
void eraseEncoderElapsed(enum moto whichMoto);

float counterToAngular(uint16_t counterDelt);
float speedToAngularSpeed(float speed);
float Moto_GetAngular(enum moto whichMoto);