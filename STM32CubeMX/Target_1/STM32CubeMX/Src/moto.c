#include "moto.h"
#include "main.h"
#include "stdbool.h"

#define LeftMotoCounter htim2
#define RightMotoCounter htim9
#define RightEncoder htim3
#define LeftEncoder htim4

#define MaxPercent 1.0f
#define MinPercent 0.0f
#define PWM_ARR 100.0f
#define COUNTER_PER_ROUTE 13*30*4
#define PI 3.14159265f

extern TIM_HandleTypeDef LeftMotoCounter;
extern TIM_HandleTypeDef RightMotoCounter;
extern TIM_HandleTypeDef LeftEncoder;
extern TIM_HandleTypeDef RightEncoder;

void LeftEncoderCB();
void RightEncoderCB();



void MotoFor(uint16_t counter,enum moto whichMoto)
{
	if(whichMoto == motoLeft) __HAL_TIM_SetCompare(&LeftMotoCounter, TIM_CHANNEL_1, counter);
    else 					  __HAL_TIM_SetCompare(&RightMotoCounter,TIM_CHANNEL_1, counter);
}

void MotoBack(uint16_t counter,enum moto whichMoto)
{
	if(whichMoto == motoLeft) __HAL_TIM_SetCompare(&LeftMotoCounter, TIM_CHANNEL_4, counter);
    else 					  __HAL_TIM_SetCompare(&RightMotoCounter,TIM_CHANNEL_2, counter);
}

void motoInit()
{
	HAL_TIM_PWM_Start(&LeftMotoCounter, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&LeftMotoCounter, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&RightMotoCounter, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&RightMotoCounter, TIM_CHANNEL_2);

	MotoFor (0,motoLeft);
	MotoBack(0,motoLeft);
	
	MotoFor (0,motoRight);
	MotoBack(0,motoRight);
	
	HAL_TIM_RegisterCallback(&LeftEncoder, HAL_TIM_PERIOD_ELAPSED_CB_ID,LeftEncoderCB);
	HAL_TIM_RegisterCallback(&RightEncoder,HAL_TIM_PERIOD_ELAPSED_CB_ID,RightEncoderCB);
	HAL_TIM_Encoder_Start(&LeftEncoder, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&RightEncoder, TIM_CHANNEL_ALL);
}

bool leftEncoderElapsed = 0;
bool rightEncoderElapsed = 0;

void LeftEncoderCB()
{
	leftEncoderElapsed = 1;
}
void RightEncoderCB()
{
	rightEncoderElapsed = 1;
}
bool ifEncoderElapsed(enum moto whichMoto)
{
	if(whichMoto == motoLeft) return leftEncoderElapsed;
	else                      return rightEncoderElapsed;
}
void eraseEncoderElapsed(enum moto whichMoto)
{
	leftEncoderElapsed  = !(whichMoto == motoLeft)  & leftEncoderElapsed;
	rightEncoderElapsed = !(whichMoto == motoRight) & rightEncoderElapsed;
}
uint16_t counterCal(float per)
{
	if(per > MaxPercent) per =  MaxPercent;
	if(per < MinPercent)per =   MinPercent;
    return (uint16_t)(per * (float)PWM_ARR);
}
void MotoActivate(float per,enum moto whichMoto)
{
    if (per >= 0)
	{
		MotoFor(counterCal(per),whichMoto);
		MotoBack(0,whichMoto);
	}
	else
	{
		MotoFor(0,whichMoto);
		MotoBack(counterCal(per * -1.0f),whichMoto);
	}
}

uint16_t Moto_GetCounter(enum moto whichMoto)
{
	if(whichMoto == motoLeft) return __HAL_TIM_GetCounter(&LeftEncoder);	
	else return						 __HAL_TIM_GetCounter(&RightEncoder);	
}

float counterToAngular(uint16_t counterDelt)
{
	return ((float)counterDelt) / ((float)COUNTER_PER_ROUTE) * (2.0f*PI);
}
float speedToAngularSpeed(float speed)
{
	return ((float)speed) / ((float)COUNTER_PER_ROUTE) * (2.0f*PI);
}

float Moto_GetAngular(enum moto whichMoto)
{
	return counterToAngular(Moto_GetCounter(whichMoto));
}