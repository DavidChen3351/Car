#include "motionControl.h"
#include "moto.h"
#include "differentialTurn.h"
#include "receiver.h"

#include "cmsis_os2.h"

typedef void (*handleFunc)();
typedef enum
{
	CH_WAITING = 0,
	UPPER_UDP_waiting,
	MODE_SIZE
} dataForwardMode;

typedef struct
{
	dataForwardMode mode;
	handleFunc handleFuncList[MODE_SIZE];
} dataForwardHandle;

// get
// process
// send

static osMessageQueueId_t receiverQueue;
static osMessageQueueId_t controlTargetQueue;
static positionHandle pos;

void CH_Waiting();
void motionProcessTask(void *argument);
void motionGetSpeed(float speed[MOTO_NUM],float distanceGain[MOTO_NUM]);

void motionIni(osMessageQueueId_t receiver, osMessageQueueId_t controlTarget)
{
	receiverQueue = receiver;
	controlTargetQueue = controlTarget;

	motoRegisterSpeedUpdate(motionGetSpeed);
	osThreadNew(motionProcessTask, NULL, NULL);
}

void motionProcessTask(void *argument)
{
	(void)argument;
	// dataForwardHandle dataHandle;
	for (;;)
	{
		// dataHandle.handleFuncList[dataHandle.mode]();
		CH_Waiting();
	}
}

void stopMoto()
{
	motoTarget moto;
	moto.targetPer[0] = 0;
	moto.targetPer[1] = 0;
	osMessageQueuePut(controlTargetQueue, &moto, 0, 0);
}

void CH_Waiting()
{
	controllerData controller;
	if (osMessageQueueGet(receiverQueue, &controller, NULL, 30) == osOK)
	{
		if (controller.failSafe == false)
		{
			motoTarget moto;
			differentialTurn(controller.vel, controller.omega,&moto.targetSpeed[0]);
			osMessageQueuePut(controlTargetQueue, &moto, 0, 0);
		}
		else
		{
			stopMoto();
		}
	}
	else
	{
		stopMoto();
	}
}

void motionGetSpeed(float speed[MOTO_NUM],float distanceGain[MOTO_NUM])
{
	posSetDistance(&pos,speed,distanceGain);
	posCompute(&pos);
}
