#include "main.h"
#include "controllerData.h"
#include "RingBuffer.h"
#include "uart.h"
#include "moto.h"

#include "cmsis_os2.h"

#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 50
#define DMA_BUFFER_SIZE 55

#define BYTE_LENGTH (uint8_t)8	   // a byte is 8 bit
#define FRAME_LENGTH (uint8_t)25   // number of bytes a frame contains
#define FRAME_TOTAL_CH (uint8_t)16 // total channels of a frame
#define CH_LENGTH (uint8_t)11	   // each channel contains 11 bits
#define FRAME_START_BYTE (uint8_t)0x0f  // first byte in frame in hex
#define FRAME_END_BYTE   (uint8_t)0x00	// last byte in frame in hex

#define FORWARD_CH (uint8_t)2 // index of forward channel
#define TURN_CH (uint8_t)0   // index of turn channel
#define CONTROL_MODE_CH (uint8_t)6 // index of control mode channel
#define CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN 1 //only one side motor turn,when not forward
#define CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN 0 //differential turn,when not forward,no XY movement

#define MAX_COUNTER_SPEED 6
#define MAX_CONTROL_PER 1.0f
#define MIN_CONTROL_PER -1.0f

enum computeMode 
{
	add = 0,
	minus
};


void receiverProcess(RingBuffer* rb,uint16_t* CH);
void controllerHandle(uint16_t* CH,controlTarget* t);
void RxEventCallback(UART_HandleTypeDef *huart, uint16_t Pos);
void controllerDataProcessTask();
float compute(float a, float b, float *result, enum computeMode mode);
int16_t getCH_Shift(uint16_t *CH,uint8_t CH_Index);

const int16_t MIDDLE = 992;
const uint16_t CH_Total = 800;
uint16_t CH[16];

uint8_t buffer[BUFFER_SIZE];
uint8_t DMAbuffer[DMA_BUFFER_SIZE];
RingBuffer rb;

const osThreadAttr_t control_attr = {
	.priority = osPriorityHigh,
	.stack_size = 8192,
};
osThreadId_t dataProcessHandle;
osMessageQueueId_t controlTargetQueue;
osEventFlagsId_t controlTargetFlags;

void controllerDataIni(osMessageQueueId_t queue) // need to be called once at the begining
{
	RingBufferIni(BUFFER_SIZE, &buffer[0], &rb);
	
	uartIni(RxEventCallback, DMAbuffer, DMA_BUFFER_SIZE);

	controlTargetQueue = queue;
	controlTargetFlags = osEventFlagsNew(NULL);
	dataProcessHandle = osThreadNew(controllerDataProcessTask, NULL, &control_attr);
}

/*
 *the thread of handle data from controller
 */
void controllerDataProcessTask()
{
	controlTarget target;
	for(;;)
	{
		osEventFlagsWait(controlTargetFlags,ringBufferReady,osFlagsWaitAny, osWaitForever);
		receiverProcess(&rb, &CH[0]);
		controllerHandle(&CH[0], &target);
		osMessageQueuePut(controlTargetQueue,&target,0,0);
	}	
}

/*
 *when uart is not receiving,function will be called
 */
void RxEventCallback(UART_HandleTypeDef *huart, uint16_t Pos)
{
	(void)huart;
	// move data from dma_buffer to ringbuffer
	uint8_t bufferSpace = RingBuffer_GetRemain(&rb);
	for (int i = 0; i < bufferSpace && i < Pos; i++)
	{
		RingBuffer_Write(&rb, DMAbuffer[i]);
	}
	// tell program RingBuffer should be handlled
	osEventFlagsSet(controlTargetFlags, ringBufferReady);
}

/*
 *return the shift value of a channel
 */
inline int16_t getCH_Shift(uint16_t *CH,uint8_t CH_Index)
{
	if (CH_Index >= 0 && CH_Index <= 15)
	{
		return (int16_t)CH[CH_Index] - MIDDLE;
	}
	else
	{
		return 0;
	}
}

/*
 *return the percentage value of a channel
 */
float getCH_Per(uint16_t* CH,uint8_t CH_Index)
{
	return (float)getCH_Shift(CH,CH_Index) / (float)CH_Total;
}

/*
 *this fun process data from controller to controller Channel
 *fun should be called when idle event callback is triggered AND
 *callback fun has moved data from DMA buffer to RingBuffer
 */
void receiverProcess(RingBuffer* rb,uint16_t* CH)
{
	uint8_t length = RingBuffer_GetLength(rb);
	if (length >= FRAME_LENGTH)
	{
		for (int i = 0; i < length - FRAME_LENGTH + 1; i++)
		{
			// validate frame start and frame end
			if (RingBuffer_Read(rb, 0) == FRAME_START_BYTE &&
				RingBuffer_Read(rb, FRAME_LENGTH - 1) == FRAME_END_BYTE)
			{
				// compute frame data into Channel data
				uint8_t taken = 0;
				uint8_t CH_Index = 0;
				uint8_t bufferIndex = 1;//index 0 is the start frame
				for (int i = 0; i < FRAME_TOTAL_CH; i++)
				{
					uint8_t leftTakeNUM;
					uint8_t rightTakeNUM;

					leftTakeNUM = BYTE_LENGTH - taken;
					rightTakeNUM = CH_LENGTH - leftTakeNUM;

					uint16_t leftTake;
					uint16_t rightTake;

					if (rightTakeNUM > BYTE_LENGTH)
					{
						uint16_t middleTake;
						rightTakeNUM -= BYTE_LENGTH;

						leftTake = (uint16_t)RingBuffer_Read(rb, bufferIndex) >> (BYTE_LENGTH - leftTakeNUM);
						bufferIndex++;
						middleTake = (uint16_t)RingBuffer_Read(rb, bufferIndex) << (leftTakeNUM);
						bufferIndex++;
						rightTake = (uint16_t)RingBuffer_Read(rb, bufferIndex) << (leftTakeNUM + BYTE_LENGTH);

						CH[CH_Index] = (leftTake | middleTake | rightTake)& 0x7ff;
					}
					else
					{
						leftTake = (uint16_t)RingBuffer_Read(rb, bufferIndex) >> (BYTE_LENGTH - leftTakeNUM);
						bufferIndex++;
						rightTake = (uint16_t)RingBuffer_Read(rb, bufferIndex) << leftTakeNUM;
						
						CH[CH_Index] = (leftTake | rightTake )& 0x7ff ;
					}

					taken = rightTakeNUM;
					CH_Index++;
				}
				//move read index of RingBuffer
				RingBuffer_AddReadIndex(rb, FRAME_LENGTH);
				return;
			}
			else
			{ // if not validate,RingBuffer add read index
				RingBuffer_AddReadIndex(rb, 0x01);
			}
		}
	}
	//program reach here means no valid frame start and end found
	return;
}

/*
*a fun used to help compute left and right moto target percentage
*/
inline float compute(float a, float b, float *result, enum computeMode mode)
{
	if (mode == add)
	{
		*result = a + b;
	}
	else
	{
		*result = a - b;
	}

	float overFlow;
	if (*result > MAX_CONTROL_PER)
	{
		overFlow = *result - MAX_CONTROL_PER;
		*result = MAX_CONTROL_PER;
		return overFlow;
	}
	else if (*result < MIN_CONTROL_PER)
	{
		overFlow = *result - MIN_CONTROL_PER;
		*result = MIN_CONTROL_PER;
		return overFlow;
	}
	return 0.0f;
}

/*
*ths fun get control target from controller data
*/
void controllerHandle(uint16_t* CH,controlTarget* t)
{
	float CH_forwardPer = getCH_Per(CH,FORWARD_CH);
	float CH_turnPer = getCH_Per(CH,TURN_CH);
	bool controlMode = getCH_Shift(CH,CONTROL_MODE_CH) == CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN ? 
	CONTROLLER_CONTROL_MODE_ONE_SIDE_TURN : CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN;
	float leftPer;
	float rightPer;

	if (controlMode == CONTROLLER_CONTROL_MODE_DIFFERENTIAL_TURN)
	{
		if (CH_forwardPer != 0)
		{
			if (CH_turnPer > 0)
			{
				leftPer = CH_forwardPer;
				rightPer = CH_forwardPer * (MAX_CONTROL_PER - CH_turnPer);
			}
			else
			{
				CH_turnPer = -CH_turnPer;
				leftPer = (MAX_CONTROL_PER - CH_turnPer) * CH_forwardPer;
				rightPer = CH_forwardPer;
			}
		}
		else
		{
			leftPer = CH_turnPer;
			rightPer = -CH_turnPer;
		}
	}
	else
	{
		float leftOverFlow = compute(CH_forwardPer, CH_turnPer, &leftPer, add);
		float rightOverFlow = compute(CH_forwardPer, CH_turnPer, &rightPer, minus);
		leftPer -= rightOverFlow;
		rightPer -= leftOverFlow;
	}
	t->targetPer[0] = leftPer;
	t->targetPer[1] = rightPer;
}
