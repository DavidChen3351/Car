#pragma once
#include "main.h"
#include "stdbool.h"
bool isDataReady();
void uartInit();
void dataProcess();

int16_t getCH_Shift(uint16_t CH);
float   getCH_Per(uint16_t CH);