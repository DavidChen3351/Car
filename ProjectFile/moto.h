#pragma once

#include "motoConfig.h"

#include "PID.h"
#include "pos.h"
#include "speedCal.h"

#include <stdbool.h>
#include "cmsis_os2.h"

typedef struct {
	float targetPer[MOTO_NUM];//should be between -1 and 1
	float targetSpeed[MOTO_NUM];
}motoTarget;

typedef void (*speedReturn)(float speed[MOTO_NUM],float distanceGain[MOTO_NUM]);

osMessageQueueId_t motoInit();
bool motoRegisterSpeedUpdate(speedReturn f);