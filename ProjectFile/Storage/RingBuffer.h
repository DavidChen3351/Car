#pragma once

#include "main.h"

#define lengthType uint16_t
typedef struct {
    lengthType read;
    lengthType write;  
    uint8_t* buf;
    lengthType size;
}RingBuffer;

void RingBufferIni(lengthType size,uint8_t* bufferArray,RingBuffer* rb);
void RingBuffer_Write(RingBuffer* rb,lengthType data);
void RingBuffer_WriteBytes(RingBuffer* rb,uint8_t* data,lengthType length);
lengthType RingBuffer_Read(RingBuffer* rb,lengthType index);
void RingBuffer_AddReadIndex(RingBuffer* rb,lengthType index);
lengthType RingBuffer_GetLength(RingBuffer* rb);
lengthType RingBuffer_GetRemain(RingBuffer* rb);

#ifndef RingBuffer_C
#undef lengthType
#endif