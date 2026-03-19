#pragma once

#include "rl_net.h"

typedef void (*UDP_CallBack)(const uint8_t *buf, uint32_t len);
void UDP_Ini();
void UDP_RegisterCB(uint16_t port, UDP_CallBack cb);
bool UDP_GetBuffer(uint32_t len);
void UDP_WriteBuffer(uint8_t* ptr,uint32_t len);
void UDP_SendData ();