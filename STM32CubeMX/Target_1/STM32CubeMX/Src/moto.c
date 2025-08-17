#include "moto.h"
#include "main.h"
#include "stdbool.h"

#define LeftMoto htim2
#define RightMoto htim9
#define RightEncoder htim3
#define LeftEncoder htim4

extern TIM_HandleTypeDef LeftMoto;
extern TIM_HandleTypeDef RightMoto;
extern TIM_HandleTypeDef LeftEncoder;
extern TIM_HandleTypeDef RightEncoder;

void LeftEncoderCB();
void RightEncoderCB();
struct PIDs PID_Set(float targetLeft,float targetRight);
	
void LeftMotoFor(uint16_t counter)
{
    __HAL_TIM_SetCompare(&LeftMoto, TIM_CHANNEL_1, counter);
}
void LeftMotoBack(uint16_t counter)
{
    __HAL_TIM_SetCompare(&LeftMoto, TIM_CHANNEL_4, counter);
}
void RightMotoFor(uint16_t counter)
{
    __HAL_TIM_SetCompare(&RightMoto, TIM_CHANNEL_1, counter);
}
void RightMotoBack(uint16_t counter)
{
    __HAL_TIM_SetCompare(&RightMoto, TIM_CHANNEL_2, counter);
}
void motoInit()
{
	HAL_TIM_PWM_Start(&LeftMoto, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&LeftMoto, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&RightMoto, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&RightMoto, TIM_CHANNEL_2);
	LeftMotoFor(0);
	LeftMotoBack(0);
	RightMotoFor(0);
	RightMotoBack(0);
	
	HAL_TIM_RegisterCallback(&LeftEncoder,HAL_TIM_PERIOD_ELAPSED_CB_ID,LeftEncoderCB);
	HAL_TIM_RegisterCallback(&RightEncoder,HAL_TIM_PERIOD_ELAPSED_CB_ID,RightEncoderCB);
	HAL_TIM_Encoder_Start(&LeftEncoder, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&RightEncoder, TIM_CHANNEL_ALL);
	
	
}

bool LeftEncoderElapsed = 0;
bool RightEncoderElapsed = 0;
void LeftEncoderCB()
{
	LeftEncoderElapsed = 1;
}
void ifLeftEncoderElapsed()
{
	return LeftEncoderElapsed;
}
void RightEncoderCB()
{
	RightEncoderElapsed = 1;
}
const float PWM_ARR = 100.0f;
// bool isforward = 0;
#define motoLeft 0
#define motoRight 1

#define MaxSpeed 400
#define MaxPercent 0.5f
#define MinPercent 0.0f
uint16_t counterCal(float per)
{
	if(per > MaxPercent) per =  MaxPercent;
	if(per < MinPercent)per =   MinPercent;
    return (uint16_t)(per * (float)PWM_ARR);
}
void leftMoto(float per)
{
    if (per >= 0)
	{
		LeftMotoFor(counterCal(per));
		LeftMotoBack(0);
	}
	else
	{
		LeftMotoFor(0);
		LeftMotoBack(counterCal(per * -1.0f));
	}
}
void rightMoto(float per)
{
    if (per >= 0)
	{
		RightMotoFor(counterCal(per));
		RightMotoBack(0);
	}
	else
	{
		RightMotoFor(0);
		RightMotoBack(counterCal(per * -1.0f));
	}
}

struct PIDs{
    float PID_Left;
    float PID_Right;
};
void motoControlSet(float forwardPer, float turnPer,bool mode)
{
	float leftPer;
	float rightPer;
	if(mode == 0)
	{
		if (forwardPer != 0)
		{
			if (turnPer > 0)
			{
				leftPer = forwardPer;
				rightPer = forwardPer * (1.0f - turnPer);
			}
			else
			{
				turnPer = -turnPer;
				leftPer = (1.0f - turnPer) * forwardPer;
				rightPer = forwardPer;
			}
		}
		else
		{
			leftPer  = turnPer;
			rightPer = - turnPer;
		}
	}
	if(mode == 1)
	{
		if(forwardPer + turnPer > 1.0f)
		{
			leftPer = 1.0f - 2.0f * turnPer;
			rightPer = 1.0f;
		}
		else if(forwardPer - turnPer < -1.0f)
		{
			leftPer = 1.0f;
			rightPer = 2.0f * turnPer - 1.0f;
		}
		else
		{
			leftPer = forwardPer - turnPer;
			rightPer = forwardPer + turnPer;
		}
	}
	float targetLeftSpeed = leftPer   * (float)MaxSpeed;
	float targetRightSpeed = rightPer * (float)MaxSpeed;
	struct PIDs PID_Result = PID_Set(targetLeftSpeed,targetRightSpeed);
	leftMoto(PID_Result.PID_Left);
	rightMoto(PID_Result.PID_Right);
}
#define countPerCircle 52
#define timerInterval 100 // 20 ms
#define counterARR  65535

struct speeds{
	uint16_t accumCounter;
	uint16_t accumCal ;
	uint16_t previousCounter;
	float currentSpeed;          
	float currentAcc;
	TIM_HandleTypeDef *encoder;
	void (*ifCounterElapsed)();

	float lastspeed;
};

#define MaxAccumCal 4
#define speedAlpha 0.2f
#define accAlpha 0.2f
//volatile struct speeds speed;

void SpeedCal(struct speeds *speed)
{
	uint16_t counter  = __HAL_TIM_GetCounter(speed->encoder);	
	//bool endCal;
	speed->accumCounter += counter - speed->previousCounter;
	if(speed->accumCal != 0)
	{
		if(speed->accumCal < MaxAccumCal)
		{
			if(speed->accumCounter == 0)
			{
				speed->accumCal++;
				return;
			}
		}
	}		
	if(speed->ifCounterElapsed() == true)
	{
		speed->accumCounter -= counterARR;
	}
	uint16_t time = timerInterval * (speed->accumCal + 1);
	
	float newspeed = ((float)speed->accumCounter) / ((float)time);	
	float lastspeed = speed->currentSpeed;
	
	speed->currentSpeed = speedAlpha*newspeed + (1.0f-speedAlpha)*lastspeed;

	float newAcc = ((float)speed->currentSpeed - (float)speed->lastSpeed) / ((float)time);
	float lastAcc = speed->currentAcc;
	
	speed->currentAcc =    accAlpha*newAcc +     (1.0f-accAlpha)*lastAcc;

	
	speed->accumCal = 0;
}

#define ConstP 0.01f
#define ConstI 0.000001f
#define ConstD 0.01f
float I_Left_Strength = 0.0f;
float I_Right_Strength = 0.0f;
float PID_Cal(float targetSpeed,float currentSpeed,float currentAcceleration,float *I_Strength)
{
	float P_Strength = (targetSpeed - currentSpeed) * ((float)ConstP);
	
	
	///////////////////////////////
	if((*I_Strength > 1 && (targetSpeed - currentSpeed) < 0 ) || (*I_Strength < -1 && (targetSpeed - currentSpeed) > 0)  ||  (*I_Strength>=-1 && *I_Strength <= 1))
	{
		*I_Strength += (targetSpeed - currentSpeed) * ((float)timerInterval) * ((float)ConstI);
	}
	
	float D_Strength = currentAcceleration / ((float)timerInterval) * ((float)ConstD);
	float PID_Strength = P_Strength + *I_Strength + D_Strength;
	return PID_Strength;
}

struct PIDs PID_Set(float targetLeft,float targetRight,struct speeds leftSpeed,struct speeds rightSpeed)
{
  struct PIDs PID_Result;
	
	PID_Result.PID_Left  = PID_Cal(targetLeft ,leftSpeed,&I_Left_Strength );
	PID_Result.PID_Right = PID_Cal(targetRight,leftSpeed.rightSpeed ,rightSpeed.rightAcc,&I_Right_Strength);
	return PID_Result;
}