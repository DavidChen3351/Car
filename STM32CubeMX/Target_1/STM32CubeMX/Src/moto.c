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

void LeftEncoderCB(TIM_HandleTypeDef *htim);
void RightEncoderCB(TIM_HandleTypeDef *htim);

struct encoderCBs
{
	struct speeds *pSpeed;
};

struct encoderCBs leftCB;
struct encoderCBs rightCB;

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

	HAL_TIM_RegisterCallback(&LEFT_ENCODER, HAL_TIM_PERIOD_ELAPSED_CB_ID, LeftEncoderCB);
	HAL_TIM_RegisterCallback(&RIGHT_ENCODER, HAL_TIM_PERIOD_ELAPSED_CB_ID, RightEncoderCB);
	HAL_TIM_Encoder_Start(&LEFT_ENCODER, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&RIGHT_ENCODER, TIM_CHANNEL_ALL);
}

void leftEncoderCB_Ini(struct speeds *pSpeed)
{
	leftCB.pSpeed = pSpeed;
}

void rightEncoderCB_Ini(struct speeds *pSpeed)
{
	rightCB.pSpeed = pSpeed;
}

void LeftEncoderCB(TIM_HandleTypeDef *htim)
{
	if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&LEFT_ENCODER))
	{
		leftCB.pSpeed->encoderOverFlow  -= COUNTER_ARR;
	}
	else
	{
		leftCB.pSpeed->encoderOverFlow  += COUNTER_ARR;
	}
}

void RightEncoderCB(TIM_HandleTypeDef *htim)
{
	if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&RIGHT_ENCODER))
	{
		rightCB.pSpeed->encoderOverFlow  -= COUNTER_ARR;
	}
	else
	{
		rightCB.pSpeed->encoderOverFlow  += COUNTER_ARR;
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
	
	int64_t counter = Moto_GetCounter(pSpeed->whichMoto);
	pSpeed->diffCounter = (int32_t)counter - (int32_t)pSpeed->previousCounter;
	
	pSpeed->diffCounter += pSpeed->encoderOverFlow;
	pSpeed->encoderOverFlow = 0;
	
	if (pSpeed->accumCal < MaxAccumCal && pSpeed->diffCounter == 0)
	{
		pSpeed->accumCal++;
		pSpeed->ifNewSpeedCal = false;
		return;
	}
	//	if(ifEncoderElapsed(speed->whichMoto) == true)
	//	{
	//		speed->accumCounter -= counterARR;
	//    eraseEncoderElapsed(speed->whichMoto);
	//	}
	uint16_t time = ((float)timerInterval) * (float)(pSpeed->accumCal + 1);

	float newspeed = ((float)pSpeed->diffCounter) / ((float)time);
	float lastSpeed = pSpeed->currentSpeed;
	pSpeed->lastSpeed = lastSpeed;

	pSpeed->currentSpeed = speedAlpha * newspeed + (1.0f - speedAlpha) * lastSpeed;

	float newAcc = ((float)pSpeed->currentSpeed - (float)pSpeed->lastSpeed) / ((float)time);
	float lastAcc = pSpeed->currentAcc;

	pSpeed->currentAcc = accAlpha * newAcc + (1.0f - accAlpha) * lastAcc;

	pSpeed->previousCounter = counter;
	// speed->accumCounter = 0;
	pSpeed->accumCal = 0;
	pSpeed->ifNewSpeedCal = true;
}