#include "main.h"
#include <string.h>

#define RingBuffer_C
#include "RingBuffer.h"
/*
*@para size size should be byte length of buffer 
*@para bufferArray the array build by user 
*@para rb contains RingBufferInformation
*@para size: the max index of ringbuffer PLUS one
*note: ringbuffer can only store size - 1 bytes data
*/
void RingBufferIni(lengthType size,uint8_t* bufferArray,RingBuffer* rb)
{
    size --;
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
inline void RingBuffer_Write(RingBuffer* rb,lengthType data)
{
    rb->buf[rb->write] = data;
    rb->write++;
    rb->write = rb->write % rb->size;
}

inline void RingBuffer_WriteBytes(RingBuffer* rb,uint8_t* data,lengthType length)
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
*@para index The index of wanted byte in ringbuffer,begin at 0
*/
inline lengthType RingBuffer_Read(RingBuffer* rb,lengthType index)
{
    lengthType readIndex = rb->read + index;
    readIndex = readIndex % rb->size;
    return rb->buf[readIndex];
}

/*
*add read of RingBuffer
*does NOT provide protection if index adding is more than data length of RingBuffer 
*/
inline void RingBuffer_AddReadIndex(RingBuffer* rb,lengthType index)
{
    rb->read = (rb->read + index) % rb->size; 
}

/*
*get number of data(bytes) stored in ringbuffer
*/
inline lengthType RingBuffer_GetLength(RingBuffer* rb) 
{
    return (rb->write + rb->size - rb->read) % rb->size;
}

/*
*get length of remain space of ringbuffer
*/
inline lengthType RingBuffer_GetRemain(RingBuffer* rb)
{
    return rb->size - RingBuffer_GetLength(rb) -1;
}

