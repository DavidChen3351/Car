#pragma once

#define bufferPara RingBuffer* rb
#define bufferReadByte(x) RingBuffer_Read(rb, x)

#include <stdbool.h>
#include "RingBuffer.h"
#include "stdint.h"

bool SBUS_ValidFrame(bufferPara);
void SBUS_Process(bufferPara, uint16_t *CH);
#define FRAME_LOST_CH_INDEX 0x16U
#define FAIL_SAFE_CH_INDEX 0x17U
#define SBUS_FRAME_BYTE_LENGTH 25U
#define SBUS_TOTAL_CH 0x16U
#define SBUS_RESULT_CH_NUM (SBUS_TOTAL_CH + 2U) 

#ifndef SBUS_c
#undef bufferIniPara
#undef bufferReadByte
#undef bufferRemoveBytes
#undef bufferGetLength
#endif
