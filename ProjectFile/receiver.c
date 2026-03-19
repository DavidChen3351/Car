#include "cmsis_os2.h"
#include <stdbool.h>
#include <string.h>

#include "RingBuffer.h"
#include "uart.h"
#include "moto.h"
#include "SBUS.h"
#include "receiver.h"
#include "HT-8A.h"

#define BUFFER_SIZE 50
#define ringBufferReady 0x00000001U

const osThreadAttr_t receiverProcess_attr = {
	.priority = osPriorityHigh,
	.stack_size = 8192,
};
static osEventFlagsId_t controlTargetFlags;
static osMessageQueueId_t receiverQueue;
uint16_t CH[16];
RingBuffer rb;
uint8_t buffer[BUFFER_SIZE];

void Receiver_Process(RingBuffer *rb, uint16_t *CH);
void RxEventCallback(uint8_t *buffPtr, uint16_t len);
void receiverProcessTask();

osMessageQueueId_t receiverIni() // need to be called once at the begining
{
	RingBufferIni(BUFFER_SIZE, &buffer[0], &rb);

	uartIni(RxEventCallback);

	controlTargetFlags = osEventFlagsNew(NULL);
	receiverQueue = osMessageQueueNew(4,sizeof(controllerData),NULL);	
	osThreadNew(receiverProcessTask, NULL, &receiverProcess_attr);
	return receiverQueue;
}

/*
 *the thread of handle uart data from receiver
 */
void receiverProcessTask()
{
	controllerData c;
	for (;;)
	{
		osEventFlagsWait(controlTargetFlags, ringBufferReady, osFlagsWaitAny, osWaitForever);
		Receiver_Process(&rb, &CH[0]);
		controllerProcess(&CH[0], &c);

		osMessageQueuePut(receiverQueue, &c, NULL, osWaitForever);
	}
}

/*
 *when uart is not receiving,function will be called
 */
void RxEventCallback(uint8_t *buffPtr, uint16_t len)
{
	// move data from dma_buffer to ringbuffer
	uint16_t bufferSpace = RingBuffer_GetRemain(&rb);
	uint16_t length = (bufferSpace < len) ? bufferSpace : len;
	RingBuffer_WriteBytes(&rb, buffPtr, length);

	// tell program RingBuffer should be handlled
	unsigned int result = osEventFlagsSet(controlTargetFlags, ringBufferReady);
	if(result == 0xA0000000U)
	{
		//error
		static int errorCount = 0;
		errorCount++;
	}
}


/*
 *deals with receiver SBUS data send to UART.
 *decode SBUS data into channel
 */
void Receiver_Process(RingBuffer *rb, uint16_t *CH)
{
	uint8_t buffLength = RingBuffer_GetLength(rb);
	while (buffLength >= SBUS_FRAME_LENGTH)
	{
		// validate frame start and frame end
		if (SBUS_ValidFrame(rb))
		{
			SBUS_Process(rb,CH);
			RingBuffer_AddReadIndex(rb, SBUS_FRAME_LENGTH);
			return;
		}
		else
		{ // if not validate,RingBuffer add read index
			RingBuffer_AddReadIndex(rb, 0x01);
			buffLength --;
		}
	}
	// program reach here means no valid frame start and end found
	return;
}
