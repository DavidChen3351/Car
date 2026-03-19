#pragma once
#include <stdbool.h>

typedef void (*userReceiveCallBack)(uint8_t* buffPtr,uint16_t len);

bool uartTTL_SendData(char *pData);
void uartIni(userReceiveCallBack callBack);