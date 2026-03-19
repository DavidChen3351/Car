#pragma once

#include "stdint.h"
#define MOTO_NUM 2
typedef uint8_t motoIndex;

#define MOTO_DISABLE_INPUT false
#define MOTO_DISABLE_PID false
#define MOTO_DISABLE_KAL false

#define PI 3.1415926f

#define MAX_ACCUM_CAL 6

#define MOTO_CONTROL_PERIOD_MS 10 //moto control task thread period in ms
#define MOTO_CONTROL_PERIOD_S 0.01f //moto control task thread period in seconds

#define COUNTER_ARR 65536 //the max value of counter
#define COUNTER_PER_ROUTE (13 * 30 * 4)//counter per round

#define WHEEL_DISTANCE 0.1646f //distance between two wheels in meters
#define WHEEL_CIRCUMFERENCE (0.064f * PI) //wheel circumference in meters

