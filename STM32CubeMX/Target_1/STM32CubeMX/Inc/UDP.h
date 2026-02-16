#pragma once

#include "rl_net.h"

void UDP_Ini();
uint8_t* UDP_GetBuffer(uint32_t len);
void UDP_SendData (uint8_t* buffer, uint32_t len);