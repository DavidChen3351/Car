#include "moto.h"
#include "main.h"
#include "stdbool.h"

#define LeftMotoCounter htim2
#define RightMotoCounter htim9
#define RIGHT_ENCODER htim3
#define LEFT_ENCODER htim4

#define MaxPercent 1.0f
#define MinPercent 0.0f
#define PWM_ARR 100
#define COUNTER_PER_ROUTE 13 * 30 * 4
#define PI 3.14159265f

#define countPerCircle 52
#define timerInterval 10 // 10 ms
#define COUNTER_ARR 65536

#define MaxAccumCal 6
#define speedAlpha 0.2f
#define accAlpha 0.2f


extern TIM_HandleTypeDef LeftMotoCounter;
extern TIM_HandleTypeDef RightMotoCounter;
extern TIM_HandleTypeDef LEFT_ENCODER;
extern TIM_HandleTypeDef RIGHT_ENCODER;

void encoderOverFlowCB(TIM_HandleTypeDef *htim);
void encoderCB_Ini(uint8_t motoIndex,TIM_HandleTypeDef *htim);

struct encoderCBs
{
	struct speeds *pSpeed;
	TIM_HandleTypeDef *htim;
};

struct encoderCBs encoderCB[2];

void MotoFor(uint16_t counter, enum moto whichMoto)
{
	if (whichMoto == motoLeft)
		__HAL_TIM_SetCompare(&LeftMotoCounter, TIM_CHANNEL_1, counter);
	else
		__HAL_TIM_SetCompare(&RightMotoCounter, TIM_CHANNEL_1, counter);
}

void MotoBack(uint16_t counter, enum moto whichMoto)
{
	if (whichMoto == motoLeft)
		__HAL_TIM_SetCompare(&LeftMotoCounter, TIM_CHANNEL_4, counter);
	else
		__HAL_TIM_SetCompare(&RightMotoCounter, TIM_CHANNEL_2, counter);
}

void motoInit()
{
	HAL_TIM_PWM_Start(&LeftMotoCounter, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&LeftMotoCounter, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&RightMotoCounter, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&RightMotoCounter, TIM_CHANNEL_2);

	MotoFor(0, motoLeft);
	MotoBack(0, motoLeft);

	MotoFor(0, motoRight);
	MotoBack(0, motoRight);

	encoderCB_Ini(0,&LeftMotoCounter );
	encoderCB_Ini(1,&RightMotoCounter);
	
	HAL_TIM_RegisterCallback(&LEFT_ENCODER, HAL_TIM_PERIOD_ELAPSED_CB_ID,  encoderOverFlowCB);
	HAL_TIM_RegisterCallback(&RIGHT_ENCODER, HAL_TIM_PERIOD_ELAPSED_CB_ID, encoderOverFlowCB);
	HAL_TIM_Encoder_Start(&LEFT_ENCODER, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&RIGHT_ENCODER, TIM_CHANNEL_ALL);
}

void encoderCB_Ini(uint8_t motoIndex,TIM_HandleTypeDef *htim)
{
	encoderCB[motoIndex].htim = htim;
}

void encoderCB_SpeedIni(uint8_t motoIndex,struct speeds *pSpeed)
{
	encoderCB[motoIndex].pSpeed = pSpeed;
}

void encoderOverFlowCB(TIM_HandleTypeDef *htim)
{
	for(uint8_t i = 0;i<2;i++)
	{
		if(encoderCB[i].htim == htim)
		{
			if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&LEFT_ENCODER))
			{
				encoderCB[i].pSpeed->totalCounter  -= COUNTER_ARR;
			}
			else
			{
				encoderCB[i].pSpeed->totalCounter  += COUNTER_ARR;
			}
		}
	}
}

uint16_t counterCal(float per)
{
	if (per > MaxPercent)
		per = MaxPercent;
	if (per < MinPercent)
		per = MinPercent;
	return (uint16_t)(per * (float)PWM_ARR);
}

void MotoActivate(float per, enum moto whichMoto)
{
	if (per >= 0)
	{
		MotoFor(counterCal(per), whichMoto);
		MotoBack(0, whichMoto);
	}
	else
	{
		MotoFor(0, whichMoto);
		MotoBack(counterCal(per * -1.0f), whichMoto);
	}
}

uint16_t Moto_GetCounter(enum moto whichMoto)
{
	if (whichMoto == motoLeft)
		return __HAL_TIM_GetCounter(&LEFT_ENCODER);
	else
		return __HAL_TIM_GetCounter(&RIGHT_ENCODER);
}

float counterToAngular(uint16_t counterDelt)
{
	return ((float)counterDelt) / ((float)COUNTER_PER_ROUTE) * (2.0f * PI);
}

float speedToAngularSpeed(float speed)
{
	return ((float)speed) / ((float)COUNTER_PER_ROUTE) * (2.0f * PI);
}

float Moto_GetAngular(enum moto whichMoto)
{
	return counterToAngular(Moto_GetCounter(whichMoto));
}

void speedCal(struct speeds *pSpeed)
{
	
	int32_t counter = Moto_GetCounter(pSpeed->whichMoto) + pSpeed->totalCounter;
	int32_t diff = counter - pSpeed->previousCounter;
	
	if (pSpeed->accumCal < MaxAccumCal && diff == 0)
	{
		pSpeed->accumCal++;
		pSpeed->ifNewSpeedCal = false;
		return;
	}
	
	uint16_t time = ((float)timerInterval) * (float)(pSpeed->accumCal + 1);

	float newspeed = ((float)diff) / ((float)time);
	float lastSpeed = pSpeed->currentSpeed;
	pSpeed->lastSpeed = lastSpeed;

	pSpeed->currentSpeed = speedAlpha * newspeed + (1.0f - speedAlpha) * lastSpeed;

	float newAcc = ((float)pSpeed->currentSpeed - (float)pSpeed->lastSpeed) / ((float)time);
	float lastAcc = pSpeed->currentAcc;

	pSpeed->currentAcc = accAlpha * newAcc + (1.0f - accAlpha) * lastAcc;

	pSpeed->previousCounter = counter;
	pSpeed->accumCal = 0;
	pSpeed->ifNewSpeedCal = true;
}