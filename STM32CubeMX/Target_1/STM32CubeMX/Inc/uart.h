#pragma once
#include "main.h"
#include <stdbool.h>

typedef void (*userReceiveCallBack)(UART_HandleTypeDef *huart, uint16_t Pos);

bool uartTTL_SendData(char *pData);
void uartIni(userReceiveCallBack callBack,uint8_t* buffer,uint16_t bufferSize);