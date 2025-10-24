#pragma once

#include "main.h"
#include "stdbool.h"
#include "kalFilter.h"
#include "GPIO.h"
#include "PID.h"

#define MOTO_NUM 2

typedef uint8_t motoIndex;

typedef struct speedStruct{
	uint16_t  accumCal ;
	uint16_t  previousCounter;

	float     currentSpeed;          
	float     currentAcc;
	float     lastSpeed;

	Kal kal;
}speed;

typedef struct motoStruct{
	GPIO_motoHandle motoGPIO;
	speed motoSpeed;
	PID motoPID;
}motoHandle;

typedef struct targetStruct{
	float targetPer[MOTO_NUM];
}target;

void motoIni();
void speedCal(motoHandle* moto);
void motoControl(target* t);
motoHandle *getMotoStruct(motoIndex index);