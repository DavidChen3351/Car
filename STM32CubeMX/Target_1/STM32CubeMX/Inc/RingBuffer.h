#include "main.h"
typedef struct RingBufferStruct{
    uint8_t read;
    uint8_t write;
    uint8_t* buf;
    uint8_t size;
}RingBuffer;

void RingBufferIni(uint8_t size,uint8_t* bufferArray,RingBuffer* rb);
void RingBuffer_Write(RingBuffer* rb,uint8_t data);
uint8_t RingBuffer_Read(RingBuffer* rb,uint8_t index);
void RingBuffer_AddReadIndex(RingBuffer* rb,uint8_t index);
uint8_t RingBuffer_GetLength(RingBuffer* rb);
uint8_t RingBuffer_GetRemain(RingBuffer* rb);
