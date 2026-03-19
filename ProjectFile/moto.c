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
#define MAX_WHEEL_SPEED 0.6f // in meter per second

#define TIME_INTERVAL 0.001 // 10 ms
#define MOTO_CONTROL_QUEUE_SIZE 3

void motoControlTask(void* para);

motoHandle moto;
osThreadId_t motoControlHandle;
static osMessageQueueId_t motoControlQueue;
const osThreadAttr_t motoControl_attr = {
	.priority = osPriorityHigh,
	.stack_size = 4096,
};

/*
*@return osMessageQueueId_t :control queueID for moto control
 *set default parameters for motoGPIO,PID,KAL and speed structure
 */
osMessageQueueId_t motoInit()
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

	// thread and queue ini
	motoControlQueue = osMessageQueueNew(MOTO_CONTROL_QUEUE_SIZE, sizeof(motoTarget), NULL);
	motoControlHandle = osThreadNew(motoControlTask, NULL, &motoControl_attr);

	osMessageQueuePut(motoControlQueue, &(motoTarget){.targetPer = {0.0f, 0.0f}}, 0, osWaitForever);

	return motoControlQueue;
}

/*
 *@para target* t:pointer to target structure
 *control moto according to target
 */
void motoControl(motoTarget *t)
{
	for (motoIndex i = 0; i < MOTO_NUM; i++)
	{
		float targetCounterSpeed = t->targetPer[i] * MAX_WHEEL_SPEED;

		PID_SetTarget(&(moto.motoPID[i]), targetCounterSpeed);

		#if MOTO_DISABLE_PID == 1
			static float lastTarget[MOTO_NUM] = {0.0f};
			speedCal(&(moto.motoSpeed[i]),Timer_GetMotoGain(i),lastTarget[i]);
			lastTarget[i] = t->targetPer[i];
		#else
			speedCal(&(moto.motoSpeed[i]),Timer_GetMotoGain(i),moto.motoPID[i].PID_Output);
		#endif

		posSetDistance(&moto.position,moto.motoSpeed[i].distanceDiff,moto.motoSpeed[i].currentSpeed,i);
		PID_SetValue(&(moto.motoPID[i]), moto.motoSpeed[i].currentSpeed);

		PID_Cal(&(moto.motoPID[i]));
		#if MOTO_DISABLE_INPUT == 0
			#if MOTO_DISABLE_PID == 1
				Timer_SetMoto(i,t->targetPer[i]);
				lastTarget[i] = targetCounterSpeed;
			#else
				Timer_SetMoto(i,moto.motoPID[i].PID_Output);
			#endif
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
	motoTarget target;
	(void)para;
	for(;;)
	{
		status = osMessageQueueGet(motoControlQueue,&target,NULL,0);
		if(status == osOK)
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
