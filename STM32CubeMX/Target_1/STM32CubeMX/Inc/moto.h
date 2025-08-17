#pragma once
#include "main.h"
#include "stdbool.h"
void motoInit();
void motoControlSet(float forwardPer, float turnPer,bool mode);
void CalculateSpeed();