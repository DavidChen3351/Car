#pragma once

#include "main.h"
#include "stdbool.h"
#include "moto.h"

void Timer_SetMoto(motoIndex index,float target);
uint16_t Timer_GetMotoCounter(motoIndex index);
void Timer_MotoInit();