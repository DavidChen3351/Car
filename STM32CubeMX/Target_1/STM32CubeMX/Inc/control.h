#pragma once
#include "main.h"
#include "stdbool.h"
bool isDataReady();
void uartInit();
void dataProcess();
int16_t getShift(uint16_t CHNUM);
