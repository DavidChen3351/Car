#include "moto.h"
#include "motoConfig.h"

#include "Timer.h"
#include "kalFilter.h"
#include "PID.h"
#include "pos.h"
#include "main.h"
#include "speedCal.h"

#include <stdbool.h>
#include "cmsis_os2.h"

#define MAX_PER 1.0f
#define MIN_PER -1.0f
#define MAX_COUNTER_SPEED 18.0f

#define TIME_INTERVAL 0.001 // 10 ms

int32_t speedCalRawDiff(uint16_t current, uint16_t prev);
void motoControlTask(void* para);

static motoHandle moto;
osThreadId_t motoControlHandle;
static osMessageQueueId_t motoControlQueue;
const osThreadAttr_t motoControl_attr = {
	.priority = osPriorityHigh,
	.stack_size = 4096,
};

/*
 *set default parameters for motoGPIO,PID,KAL and speed structure
 */
void motoIni(osMessageQueueId_t queue)
{
	// Timer init
	Timer_MotoInit();
	// position ini
	posIni(&moto.position);

	for (motoIndex i = 0; i < MOTO_NUM; i++)
	{
		// speed ini
		speedIni(&(moto.motoSpeed[i]));

		// PID ini
		PID_SetDefaultParam(&(moto.motoPID[i]));
	}

	// create moto control task
	motoControlQueue = queue;	
	motoControlHandle = osThreadNew(motoControlTask, NULL, &motoControl_attr);
}

/*
 *@para target* t:pointer to target structure
 *control moto according to target
 */
void motoControl(controlTarget *t)
{
	for (motoIndex i = 0; i < MOTO_NUM; i++)
	{
		float targetCounterSpeed = t->targetPer[i] * MAX_COUNTER_SPEED;

		PID_SetTarget(&(moto.motoPID[i]), targetCounterSpeed);

		speedCal(&(moto.motoSpeed[i]), Timer_GetMotoCounter(i));

		posSetNewDistance(&moto.position,moto.motoSpeed[i].distanceDiff,i);
		PID_SetValue(&(moto.motoPID[i]), moto.motoSpeed[i].currentSpeed);

		#if MOTO_DISABLE_INPUT == 0
			Timer_SetMoto(i, PID_Cal(&(moto.motoPID[i])));
		#else
			Timer_SetMoto(i, 0.0f);
		#endif
	}
	posCompute(&moto.position);
}

/*
 *the thread working for moto control
*/
void motoControlTask(void* para)
{
	osStatus_t status;
	controlTarget target;
	(void)para;
	for(;;)
	{
		status = osMessageQueueGet(motoControlQueue,&target,NULL,0);
		if(status == osOK || status == osErrorResource)
		{
			motoControl(&target);
		}
		osDelay(MOTO_CONTROL_PERIOD_MS);
	}
}

inline motoHandle* motoReturnHandle()
{
	return &moto;
}