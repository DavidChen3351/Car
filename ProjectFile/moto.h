#pragma once

#include "motoConfig.h"

#include "PID.h"
#include "pos.h"
#include "speedCal.h"

#include <stdbool.h>
#include "cmsis_os2.h"

typedef struct {
	float targetPer[MOTO_NUM];//should be between -1 and 1
}motoTarget;

typedef struct {
	speedStruct motoSpeed[MOTO_NUM];
	PID motoPID[MOTO_NUM];
	pos position;
}motoHandle;

osMessageQueueId_t motoInit();
motoHandle* motoReturnHandle();