#pragma once

#include "rl_net.h"
#include <stdbool.h>

typedef void (*UDP_UserCallBack)(const uint8_t *buf, uint32_t len);
void UDP_Ini();
int UDP_GetHandle(uint16_t userPort,UDP_UserCallBack cb);
bool UDP_GetBuffer(int i, uint32_t len);
bool UDP_WriteBuffer(int i, uint8_t *ptr, uint32_t len);
bool UDP_SendData(int i);