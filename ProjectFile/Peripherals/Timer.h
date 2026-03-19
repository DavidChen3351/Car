#pragma once

#include "main.h"
#include "stdbool.h"
#include "moto.h"

void Timer_SetMoto(motoIndex index,float target);

float Timer_GetMotoGain(motoIndex index);
uint16_t Timer_GetMotoCounter(motoIndex index);
int32_t Timer_GetMotoDiff(motoIndex index);
void Timer_MotoInit();