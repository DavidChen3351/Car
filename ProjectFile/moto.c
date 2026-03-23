#include "moto.h"
#include "motoConfig.h"

#include "PWM.h"
#include "Encoder.h"
#include "kalFilter.h"
#include "PID.h"
#include "main.h"
#include "speedCal.h"

#include <stdbool.h>
#include "cmsis_os2.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

#define LEFT_PWM_HTIM &htim2
#define LEFT_PWM_FORWARD_CH TIM_CHANNEL_1
#define LEFT_PWM_BACKWARD_CH TIM_CHANNEL_4
#define LEFT_ENCODER_HTIM &htim4

#define RIGHT_PWM_HTIM &htim9
#define RIGHT_PWM_FORWARD_CH TIM_CHANNEL_1
#define RIGHT_PWM_BACKWARD_CH TIM_CHANNEL_2
#define RIGHT_ENCODER_HTIM &htim3
#define PWM_ARR 100

//#define TIME_INTERVAL 0.001 // 10 ms
#define MOTO_CONTROL_QUEUE_SIZE 3
#define REGISTER_SIZE 4

typedef struct {
	speedHandle speed[MOTO_NUM];
	PID_Handle PID[MOTO_NUM];
	EncoderHandle encoder[MOTO_NUM];
	PWM_Handle PWM[MOTO_NUM];
}motoHandle;

speedReturn registerList[REGISTER_SIZE];
typedef uint8_t motoIndex;

motoHandle moto;
osThreadId_t motoControlTaskId;
static osMessageQueueId_t motoControlQueue;
const osThreadAttr_t motoControl_attr = {
	.priority = osPriorityHigh,
	.stack_size = 4096,
};

void motoControlTask(void* para);
void updateCall();

/*
*@return osMessageQueueId_t :control queueID for moto control
 *set default parameters for motoGPIO,PID_Handle,KAL and speed structure
 */
osMessageQueueId_t motoInit()
{
	EncoderInit(&moto.encoder[0], LEFT_ENCODER_HTIM);
	PWM_Init(&moto.PWM[0], LEFT_PWM_HTIM, LEFT_PWM_FORWARD_CH, LEFT_PWM_BACKWARD_CH, PWM_ARR);
	EncoderInit(&moto.encoder[1], RIGHT_ENCODER_HTIM);
	PWM_Init(&moto.PWM[1], RIGHT_PWM_HTIM, RIGHT_PWM_FORWARD_CH, RIGHT_PWM_BACKWARD_CH, PWM_ARR);

	for (motoIndex i = 0; i < MOTO_NUM; i++)
	{
		speedIni(&moto.speed[i]);
		PID_SetDefaultParam(&moto.PID[i]);
	}

	for(int i=0;i<REGISTER_SIZE;i++)
	{
		registerList[i] = NULL;
	}
	// thread and queue ini
	motoControlQueue = osMessageQueueNew(MOTO_CONTROL_QUEUE_SIZE, sizeof(motoTarget), NULL);
	motoControlTaskId = osThreadNew(motoControlTask, NULL, &motoControl_attr);

	osMessageQueuePut(motoControlQueue, &(motoTarget){.targetPer = {0.0f, 0.0f}},0,0);

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
		float targetSpeed = t->targetSpeed[i];
		float wheelGain = EncoderGetDiffGain(&moto.encoder[i]) /(float)COUNTER_PER_ROUTE * WHEEL_CIRCUMFERENCE; 
		float wheelSpeed = wheelGain / MOTO_CONTROL_PERIOD_S;
		static float PWM_Duty;

		speedCal(&moto.speed[i],wheelGain,PWM_Duty);

		#if MOTO_USE_PID == true
			PID_SetTarget(&moto.PID[i],targetSpeed);
			PWM_Duty = PID_Cal(&moto.PID[i],wheelSpeed);
		#else
			PWM_Duty = targetSpeed / MAX_WHEEL_SPEED;
		#endif

		#if MOTO_STOP == true
			PWM_Duty = 0;
		#endif
		
		PWM_Set(&moto.PWM[i],PWM_Duty);
	}

	updateCall();
}

/*
 *the thread working for moto control
*/
void motoControlTask(void* para)
{
	motoTarget target;
	(void)para;
	for(;;)
	{
		osStatus_t status = osMessageQueueGet(motoControlQueue,&target,NULL,0);
		if(status == osOK || status == osErrorResource)
		{
			motoControl(&target);
		}
		osDelay(MOTO_CONTROL_PERIOD_MS);
	}
}

bool motoRegisterSpeedUpdate(speedReturn f)
{
	for(int i=0;i<REGISTER_SIZE;i++)
	{
		if(registerList[i] == NULL) {
			registerList[i] = f;
			return true;
		}
	}
	return false;
}

inline void updateCall()
{
	for(int i=0;i<REGISTER_SIZE;i++)
	{
		if(registerList[i] == NULL) continue;
		float speed[MOTO_NUM] = {0,0};
		float distanceGain[MOTO_NUM] = {0,0};
		for(int motoIndex = 0;motoIndex < MOTO_NUM;motoIndex++)
		{
			speed[motoIndex] = moto.speed[motoIndex].currentSpeed;
			distanceGain[motoIndex] = moto.speed[motoIndex].distanceDiff;
		}
		registerList[i](speed,distanceGain);
	}
}
