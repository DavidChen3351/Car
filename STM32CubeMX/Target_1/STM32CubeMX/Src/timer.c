#include "main.h"
#include "stdbool.h"
#include "timer.h"
#define timer htim5
extern TIM_HandleTypeDef timer;

bool timerUp;

bool isTimerUp()
{
	return timerUp;
}
void ResetTimerUp()
{
	timerUp = false;
}
void timerARRCallBack()
{
	timerUp = true;
}
void timerIni()
{
  HAL_TIM_RegisterCallback(&timer,HAL_TIM_PERIOD_ELAPSED_CB_ID,timerARRCallBack);
	HAL_TIM_Base_Start_IT(&timer);
}
