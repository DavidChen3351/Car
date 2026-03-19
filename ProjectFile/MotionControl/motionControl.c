#include "motionControl.h"
#include "moto.h"
#include "turn.h"
#include "receiver.h"
#include "HT-8A.h"

#include "cmsis_os2.h"

typedef void (*handleFunc)();
typedef enum 
{
	CH_WAITING = 0,
	UPPER_UDP_waiting,
	MODE_SIZE
}dataForwardMode;

typedef struct 
{
	dataForwardMode mode;
	handleFunc handleFuncList[MODE_SIZE] ;
}dataForwardHandle;

//get 
//process
//send

static osMessageQueueId_t receiverQueue;
static osMessageQueueId_t controlTargetQueue;

void CH_Waiting();
void motionProcessTask(void *argument);
void motionIni(osMessageQueueId_t receiver, osMessageQueueId_t controlTarget)
{
	receiverQueue = receiver;
	controlTargetQueue = controlTarget;
	osThreadNew(motionProcessTask, NULL, NULL);
}

void motionProcessTask(void *argument)
{
	(void)argument;
	dataForwardHandle dataHandle;
    for(;;)
    {
		//dataHandle.handleFuncList[dataHandle.mode]();
		CH_Waiting();
    }
}

void CH_Waiting()
{
	controllerData controller;
	turnHandle turn;
    if (osMessageQueueGet(receiverQueue, &controller, NULL, osWaitForever) == osOK)
    {
        turn.forwardPer = controller.forwardPer;
		turn.turnPer = controller.turnPer;
		if(controller.mode == ONE_SIDE_TURN)
		{
			oneSideTurn(&turn);
		}else if(controller.mode == DIFFERENTIAL_TURN)
		{
			differentialTurn(&turn);
		}else
		{
			turn.targetPer[0] = 0.0f;
			turn.targetPer[1] = 0.0f;
		}

		motoTarget moto;
		moto.targetPer[0] = turn.targetPer[0];
		moto.targetPer[1] = turn.targetPer[1];
		osMessageQueuePut(controlTargetQueue,&moto, 0, 0);
    }
}
