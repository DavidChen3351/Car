#pragma once
#include "main.h"
#include "stdbool.h"
bool isDataReady();
void uartInit();
void dataProcessTask(void*para);

int16_t getCH_Shift(uint8_t CH_Index);
float getCH_Per(uint8_t CH_Index);

void controllerDataIni();
void controllerDataProcess();
void sendData(char* pData);
bool canSendData();
HAL_UART_StateTypeDef uartState();