#include "main.h"
#include "control.h"
#include "controllerData.h"
#include "moto.h"
#include <stdio.h>
#include"cmsis_os2.h"
#include"RTE_Components.h"

#include  CMSIS_device_header

#define CH_Forward (uint8_t)2
#define CH_Turn (uint8_t)0
#define CH_ControlMode (uint8_t)6
#define CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN 1
#define CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN 0

#define MAX_COUNTER_SPEED 6
#define MAX_CONTROL_PER 1.0f
#define MIN_CONTROL_PER -1.0f

#define INIT_V 0.0f
#define INIT_VAR_OF_V 0.1f
#define MEASURE_VARIANCE 0.1f // 0.05
#define ACC_VARIANCE 0.0002f  		// 0.03

#define DEFAULT_Kp 0.1f
#define DEFAULT_Ki 0.1f
#define DEFAULT_Kd 0.1f
#define MAX_I_Size 0.5f
#define MAX_PID_SIZE 1.0f

enum computeMode 
{
	add = 0,
	minus
};

target controlTarget;
extern osEventFlagsId_t controlTargetFlags;

float compute(float a, float b, float *result, enum computeMode mode);
void getTargetFromController(target* controlTarget);

float	consolePidOut;
float	consoleTarget; 
float	consoleSpeed ;
motoHandle *moto;
char data[100];

//void controlFlagReady()
//{
//	osEventFlagsSet(controlFlags,controlFlag);
//}

void controlIni()
{
	motoIni();
	controllerDataIni();
}

void controlTargetTask(void *para)
{
	(void)para;
	for(;;)
	{
		osEventFlagsWait(controlTargetFlags,controllerDataIdle,osFlagsWaitAny, osWaitForever);
		controllerDataProcess();
		getTargetFromController(&controlTarget);
	}
}

void controlTask(void * para)
{
	(void)para;
	moto = getMotoStruct(0);
	for(;;)
	{
		motoControl(&controlTarget);
		
		if(canSendData())
		{
			sprintf(data,"%2.8f,%2.8f,%2.8f\n",consolePidOut,consoleTarget,consoleSpeed);
			sendData(data);
		}
		
		osDelay(10);
	}
}

/*
*a fun used to help compute left and right moto target percentage
*/
inline float compute(float a, float b, float *result, enum computeMode mode)
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

/*
*ths fun get control target from controller data
*/
void getTargetFromController(target* controlTarget)
{
	float CH_forwardPer = getCH_Per(CH_Forward);
	float CH_turnPer = getCH_Per(CH_Turn);
	bool controlMode = getCH_Shift(CH_ControlMode) == CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN ? 
	CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN : CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN;
	float leftPer;
	float rightPer;

	if (controlMode == CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN)
	{
		if (CH_forwardPer != 0)
		{
			if (CH_turnPer > 0)
			{
				leftPer = CH_forwardPer;
				rightPer = CH_forwardPer * (MAX_CONTROL_PER - CH_turnPer);
			}
			else
			{
				CH_turnPer = -CH_turnPer;
				leftPer = (MAX_CONTROL_PER - CH_turnPer) * CH_forwardPer;
				rightPer = CH_forwardPer;
			}
		}
		else
		{
			leftPer = CH_turnPer;
			rightPer = -CH_turnPer;
		}
	}
	else
	{
		float leftOverFlow = compute(CH_forwardPer, CH_turnPer, &leftPer, add);
		float rightOverFlow = compute(CH_forwardPer, CH_turnPer, &rightPer, minus);
		leftPer -= rightOverFlow;
		rightPer -= leftOverFlow;
	}
	#if MOTO_NUM == 2
		controlTarget->targetPer[0] = leftPer;
		controlTarget->targetPer[1] = rightPer;
	#endif
	
}
