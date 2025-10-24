#include "moto.h"
#include "GPIO.h"
#include "main.h"
#include "kalFilter.h"
#include <stdbool.h>

#define LeftMotoCounter htim2
#define RightMotoCounter htim9

#define RIGHT_ENCODER htim3
#define LEFT_ENCODER htim4

#define MAX_PER 1.0f
#define MIN_PER -1.0f

#define COUNTER_PER_ROUTE 13 * 30 * 4
#define PI 3.14159265f

#define countPerCircle 52
#define timerInterval 10 // 10 ms
#define COUNTER_ARR 65536

#define MaxAccumCal 6
#define speedAlpha 0.2f
#define accAlpha 0.2f

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

int32_t speedCalRawDiff(uint16_t current, uint16_t prev);

static motoHandle motoArray[MOTO_NUM];
//void encoderOverFlowCB(TIM_HandleTypeDef *htim);
//void encoderCB_Ini(uint8_t motoIndex,TIM_HandleTypeDef *htim);

//struct encoderCBs
//{
//	struct speeds *moto_>motoSpeed;
//	TIM_HandleTypeDef *htim;
//};

//struct encoderCBs encoderCB[2];

//void encoderCB_Ini(uint8_t motoIndex,TIM_HandleTypeDef *htim)
//{
//	encoderCB[motoIndex].htim = htim;
//}

//void encoderCB_SpeedIni(uint8_t motoIndex,struct speeds *moto_>motoSpeed)
//{
//	encoderCB[motoIndex].moto_>motoSpeed = moto_>motoSpeed;
//}

//void encoderOverFlowCB(TIM_HandleTypeDef *htim)
//{
//	for(uint8_t i = 0;i<2;i++)
//	{
//		if(encoderCB[i].htim == htim)
//		{
//			if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&LEFT_ENCODER))
//			{
//				encoderCB[i].moto->motoSpeed.overFlowTimes  -= 1;
//			}
//			else
//			{
//				encoderCB[i].moto->motoSpeed.overFlowTimes  += 1;
//			}
//		}
//	}
//}

/*
*set default parameters for motoGPIO,PID,KAL and speed structure
*/
void motoIni()
{
	for(motoIndex i=0;i<MOTO_NUM;i++)
	{
		motoHandle* motoPtr = &motoArray[i];
		//GPIO ini
		switch (i)
		{
		case 0:
			{
				motoPtr->motoGPIO.motoInput_htim    = &htim2;
				motoPtr->motoGPIO.motoEncoder_htim  = &htim4;
				motoPtr->motoGPIO.motoInputForCH    = TIM_CHANNEL_1;
				motoPtr->motoGPIO.motoInputBackCH   = TIM_CHANNEL_4;
			}
			break;
		case 1:
			{
				motoPtr->motoGPIO.motoInput_htim    = &htim9;
				motoPtr->motoGPIO.motoEncoder_htim  = &htim3;
				motoPtr->motoGPIO.motoInputForCH    = TIM_CHANNEL_1;
				motoPtr->motoGPIO.motoInputBackCH   = TIM_CHANNEL_2;
			}
			break;
		}
		//speed ini
		motoPtr->motoSpeed.accumCal        = 0;
		motoPtr->motoSpeed.previousCounter = GPIO_GetMotoCounter(&(motoPtr->motoGPIO));
		motoPtr->motoSpeed.currentSpeed    = 0.0f;
		motoPtr->motoSpeed.currentAcc      = 0.0f;
		motoPtr->motoSpeed.lastSpeed       = 0.0f;
		kalDefaultParams(&(motoPtr->motoSpeed.kal));
		
		//PID ini
		PID_SetDefaultParam(&(motoPtr->motoPID));
	}
}
/*
float counterToAngular(uint16_t counterDelt)
{
	return ((float)counterDelt) / ((float)COUNTER_PER_ROUTE) * (2.0f * PI);
}

float speedToAngularSpeed(float speed)
{
	return ((float)speed) / ((float)COUNTER_PER_ROUTE) * (2.0f * PI);
}
*/

/*
*this function calculate the raw difference between two counters, considering overflow
*/
inline int32_t speedCalRawDiff(uint16_t current, uint16_t prev)
{
    int32_t delta = (int32_t)current - (int32_t)prev;
    if (delta > (COUNTER_ARR / 2)) 
	{
        delta -= (COUNTER_ARR + 1); // 正向溢出
    } else if (delta < -(COUNTER_ARR / 2)) 
	{
        delta += (COUNTER_ARR + 1); // 反向溢出
    }
    return delta;
}

void speedCal(motoHandle* moto)
{
	//kal predict
	kalPredict(&(moto->motoSpeed.kal),moto->motoSpeed.currentAcc);

	//get moto current conter and previous counter,calculate difference
	uint16_t counter = GPIO_GetMotoCounter(&(moto->motoGPIO));
	int32_t diff = speedCalRawDiff(counter, moto->motoSpeed.previousCounter);
	
	//if difference is zero,accumlate times util reach max accumlate times 
	if (moto->motoSpeed.accumCal < MaxAccumCal && diff == 0)
	{
		moto->motoSpeed.accumCal++;
		return;
	}

	//program reach here if 
	//1.no accumlation and nonzero difference
	//2.accumlation reach max,even if difference is zero.

	//calculate time interval based on accumlate times
	float time = ((float)timerInterval) * (float)(moto->motoSpeed.accumCal + 1);

	//calculate speed
	float newSpeed = ((float)diff) / (time);
	
	//calculate acceleration based on speed difference and time interval
	//lastSpeed comes from kal,while currentSpeed does not
	moto->motoSpeed.currentAcc = (moto->motoSpeed.currentSpeed - moto->motoSpeed.lastSpeed) / (time);

	//kal update
	kalUpdate(&(moto->motoSpeed.kal),newSpeed);
	moto->motoSpeed.currentSpeed = moto->motoSpeed.kal.vEstimate;

	//prepare for next calculation
	//1.update last speed
	//2.update previous counter
	//3.reset accumlate cal
	moto->motoSpeed.lastSpeed = moto->motoSpeed.currentSpeed;
	moto->motoSpeed.previousCounter = counter;
	moto->motoSpeed.accumCal = 0;
	//moto->motoSpeed.ifNewSpeedCal = true;
}

/*
*@para targetPer should between -1.0 and 1.0
*control moto according to targetPer
*/
void motoControl(target* t)
{
	for(uint8_t i=0;i<MOTO_NUM;i++)
	{
		motoHandle* moto = &motoArray[i];
		float targetPer = t->targetPer[i];
		
		PID_SetTarget(&(moto->motoPID),targetPer);

		speedCal(moto);
		PID_SetValue(&(moto->motoPID),moto->motoSpeed.currentSpeed);
	
		GPIO_SetMoto(&(moto->motoGPIO),PID_Cal(&(moto->motoPID)));
	}
}

/*
*return pointer to moto structure
*used for debug 
*/
motoHandle *getMotoStruct(motoIndex index)
{
	return &motoArray[index];
}