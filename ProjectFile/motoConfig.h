#pragma once

#include "stdint.h"
#define MOTO_NUM 2

#define MOTO_STOP false
#define MOTO_DISABLE_PID false
#define MOTO_DISABLE_KAL false
#define MOTO_USE_PID true
#define MOTO_USE_KAL true

#define PI 3.1415926f

#define MAX_ACCUM_CAL 6
#define MAX_WHEEL_SPEED 0.6f // in meter per second

#define MOTO_CONTROL_PERIOD_MS 10 //moto control task thread period in ms (int)
#define MOTO_CONTROL_PERIOD_S ((float)MOTO_CONTROL_PERIOD_MS / 1000.0f) //moto control task thread period in seconds (float)

#define COUNTER_ARR 65536 //the max value of counter
#define COUNTER_PER_ROUTE (13 * 30 * 4)//counter per round

#define WHEEL_DISTANCE 0.1646f //distance between two wheels in meters
#define WHEEL_CIRCUMFERENCE (0.064f * PI) //wheel circumference in meters

