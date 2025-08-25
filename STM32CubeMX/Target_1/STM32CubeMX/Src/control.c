#include "main.h"
#include "control.h"
#include "controllerData.h"
#include "PID.h"
#include "moto.h"
#include "kalFilter.h"
#include "stdio.h"

#define CH_Forward 2
#define CH_Turn 0
#define CH_ControlMode 6
#define MAX_COUNTER_SPEED 6
#define MAX_CONTROL_PER 1.0f
#define MIN_CONTROL_PER -1.0f

#define INIT_V 0.0f
#define INIT_VAR_OF_V 1.0f
#define MEASURE_VARIANCE 0.5f
#define ACC_VARIANCE 1.0f

struct controlTargets
{
	float forwardPer;
	float turnPer;
	bool mode;
	float leftMotoTarget;
	float rightMotoTarget;
};

enum computeMode
{
	add = 0,
	minus,
};

static char data[32];

void controllerToTarget(struct controlTargets *controlTarget);
float compute(float a, float b, float *result, enum computeMode);

struct controlTargets controlTarget;

struct speeds leftMotoSpeed;
struct speeds rightMotoSpeed;

struct PIDs PID_Left;
struct PIDs PID_Right;

struct kals kalLeft;
struct kals kalRight;

void controlIni()
{
	leftMotoSpeed.whichMoto = motoLeft;
	rightMotoSpeed.whichMoto = motoRight;
	
	kalInit(&kalLeft, INIT_V, INIT_VAR_OF_V, MEASURE_VARIANCE, ACC_VARIANCE);
	kalInit(&kalRight, INIT_V, INIT_VAR_OF_V, MEASURE_VARIANCE, ACC_VARIANCE);
	
	encoderCB_SpeedIni(0,&leftMotoSpeed);
	encoderCB_SpeedIni(1,&rightMotoSpeed);
}

void setControlTarget()
{
	controlTarget.forwardPer = getCH_Per(CH_Forward);
	controlTarget.turnPer = getCH_Per(CH_Turn);
	controlTarget.mode = getCH_Shift(CH_ControlMode) > 0 ? 1 : 0;
	controllerToTarget(&controlTarget);
	PID_Left.targetSpeed = controlTarget.leftMotoTarget * MAX_COUNTER_SPEED;
	PID_Right.targetSpeed = controlTarget.rightMotoTarget * MAX_COUNTER_SPEED;
}

void control()
{
	speedCal(&leftMotoSpeed);
	speedCal(&rightMotoSpeed);
	if (leftMotoSpeed.ifNewSpeedCal == true)
	{
		kalUpdate(&kalLeft, leftMotoSpeed.currentSpeed);
		leftMotoSpeed.ifNewSpeedCal = false;
		PID_Cal(&PID_Left, kalLeft.vEstimate, leftMotoSpeed.currentAcc);
	}
	else
	{
		PID_Cal(&PID_Left, kalLeft.vPredict, leftMotoSpeed.currentAcc);
	}
	if (rightMotoSpeed.ifNewSpeedCal == true)
	{
		kalUpdate(&kalRight, rightMotoSpeed.currentSpeed);
		rightMotoSpeed.ifNewSpeedCal = false;
		PID_Cal(&PID_Right, kalRight.vEstimate, rightMotoSpeed.currentAcc);
	}
	else
	{
		PID_Cal(&PID_Right, kalRight.vPredict, rightMotoSpeed.currentAcc);
	}

//  MotoActivate(PID_Left.PID_Strength , motoLeft);
//  MotoActivate(PID_Right.PID_Strength,motoRight);

	kalPredict(&kalLeft, leftMotoSpeed.currentAcc);
	kalPredict(&kalRight, rightMotoSpeed.currentAcc);

	sprintf(data, "%2.2f,%2.2f,%2.2f\n", kalLeft.vEstimate, kalLeft.vPredict, leftMotoSpeed.currentSpeed);
	sendData(data);

	MotoActivate(controlTarget.leftMotoTarget, motoLeft);
	MotoActivate(controlTarget.rightMotoTarget, motoRight);
}
void controllerToTarget(struct controlTargets *controlTarget)
{
	float leftPer;
	float rightPer;
	bool mode = controlTarget->mode;
	float forwardPer = controlTarget->forwardPer;
	float turnPer = controlTarget->turnPer;
	if (mode == 0)
	{
		if (forwardPer != 0)
		{
			if (turnPer > 0)
			{
				leftPer = forwardPer;
				rightPer = forwardPer * (MAX_CONTROL_PER - turnPer);
			}
			else
			{
				turnPer = -turnPer;
				leftPer = (MAX_CONTROL_PER - turnPer) * forwardPer;
				rightPer = forwardPer;
			}
		}
		else
		{
			leftPer = turnPer;
			rightPer = -turnPer;
		}
	}
	if (mode == 1)
	{
		//		if(forwardPer + turnPer > 1.0f)
		//		{
		//			leftPer = 1.0f;
		//			rightPer = 1.0f - 2.0f*turnPer;
		//		}
		//		else if(forwardPer - turnPer < -1.0f)
		//		{
		//			leftPer = -1.0f;
		//			rightPer = 1.0f - 2.0f * turnPer;
		//		}
		//		else
		//		{
		float leftOverFlow = compute(forwardPer, turnPer, &leftPer, add);
		float rightOverFlow = compute(forwardPer, turnPer, &rightPer, minus);
		leftPer -= rightOverFlow;
		rightPer -= leftOverFlow;
	}
	controlTarget->leftMotoTarget = leftPer;
	controlTarget->rightMotoTarget = rightPer;
	// struct PIDs PID_Result = PID_Set(targetLeftSpeed,targetRightSpeed);
}

float compute(float a, float b, float *result, enum computeMode mode)
{
	if (mode == add)
	{
		*result = a + b;
	}
	else
	{
		*result = a - b;
	}

	float overFlow;
	if (*result > MAX_CONTROL_PER)
	{
		overFlow = *result - MAX_CONTROL_PER;
		*result = MAX_CONTROL_PER;
		return overFlow;
	}
	else if (*result < MIN_CONTROL_PER)
	{
		overFlow = *result - MIN_CONTROL_PER;
		*result = MIN_CONTROL_PER;
		return overFlow;
	}
	return 0.0f;
}

//	PID_Left->targetSpeed  = leftPer   * (float)MaxSpeed;
//	PID_Right->targetSpeed  = rightPer * (float)MaxSpeed;