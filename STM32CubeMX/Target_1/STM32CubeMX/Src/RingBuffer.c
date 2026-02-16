#include "main.h"
#include "RingBuffer.h"
#include <string.h>

/*
*@para size size should be byte length of buffer minus one
*for example: uint8_t buffer[100]; size = 100-1; 
*@para bufferArray the array build by user 
*@para rb contains RingBufferInformation
*@para size: the max index of ringbuffer PLUS one
*/
void RingBufferIni(uint8_t size,uint8_t* bufferArray,RingBuffer* rb)
{
    rb->size = size;
    rb->buf = bufferArray;
    rb->read  = 0;
    rb->write = 0;
}

/*
*write a byte data into RingBuffer
*does NOT provide protection if write more data than remain
*space of RingBuffer
*/
inline void RingBuffer_Write(RingBuffer* rb,uint8_t data)
{
    rb->buf[rb->write] = data;
    rb->write++;
    rb->write = rb->write % rb->size;
}

inline void RingBuffer_WriteBytes(RingBuffer* rb,uint8_t* data,uint8_t length)
{
    if(rb->write + length <= rb->size)
        memcpy(rb->buf + rb->write, data, length);
    else
    {
        memcpy(rb->buf + rb->write, data, rb->size - rb->write);
        memcpy(rb->buf, data + (rb->size - rb->write), length - (rb->size - rb->write));
    }
    rb->write = (rb->write + length) % rb->size;
}

/*
*return a byte at RingBuffer
*does NOT move read index
*@para index is the index of return byte,index begin at read index
*/
inline uint8_t RingBuffer_Read(RingBuffer* rb,uint8_t index)
{
    uint8_t readIndex = rb->read + index;
    readIndex = readIndex % rb->size;
    return rb->buf[readIndex];
}

/*
*add read of RingBuffer
*does NOT provide protection if index adding is more than data length of RingBuffer 
*/
inline void RingBuffer_AddReadIndex(RingBuffer* rb,uint8_t index)
{
    rb->read = (rb->read + index) % rb->size; 
}

/*
*get number of data(bytes) stored in ringbuffer
*/
inline uint8_t RingBuffer_GetLength(RingBuffer* rb) 
{
    return (rb->write + rb->size - rb->read) % rb->size;
}

/*
*get length of remain space of ringbuffer
*/
inline uint8_t RingBuffer_GetRemain(RingBuffer* rb)
{
    return rb->size - RingBuffer_GetLength(rb) -1;
}

