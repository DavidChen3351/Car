#pragma once

#define bufferPara RingBuffer* rb
#define bufferReadByte(x) RingBuffer_Read(rb, x)

#include <stdbool.h>
#include "RingBuffer.h"
#include "stdint.h"

bool SBUS_ValidFrame(bufferPara);
void SBUS_Process(bufferPara, uint16_t *CH);
#define SBUS_FRAME_LENGTH 25U

#ifndef SBUS_c
#undef bufferIniPara
#undef bufferReadByte
#undef bufferRemoveBytes
#undef bufferGetLength
#endif
