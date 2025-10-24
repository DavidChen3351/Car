#include "main.h"
#include "controllerData.h"
#include "stdbool.h"
#include "string.h"
#include "control.h"	
#include "RingBuffer.h"

#include"cmsis_os2.h"
#include"RTE_Components.h"
#include  CMSIS_device_header

#define CONTROLLER_HUART huart3
#define TRANSMIT_HUART huart1

#define BUFFER_SIZE 50
#define DMA_BUFFER_SIZE 50

#define BYTE_LENGTH  (uint8_t)8   //a byte is 8 bit
#define FRAME_LENGTH (uint8_t)25  //number of bytes a frame contains
#define FRAME_TOTAL_CH 	(uint8_t)16  //total channels of a frame
#define CH_LENGTH    (uint8_t)11  //bit length of a channel
#define FRAME_START  (uint8_t)0x0f//start byte of frame
#define FRAME_END    (uint8_t)0x00//end byte of frame

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

extern osThreadId_t     dataProcessHandle;
extern osEventFlagsId_t controlTargetFlags;

const int16_t MIDDLE = 992;
const uint16_t CH_Total = 800;

void CHprocess();
void RxEventCallback(UART_HandleTypeDef *huart,uint16_t Pos);

uint16_t CH[16];

volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t DMAbuffer[DMA_BUFFER_SIZE];
static RingBuffer rb;

void controllerDataIni() // need to be called once at the begining
{
	//HAL_UART_RegisterCallback(CONTROLLER_HUART, HAL_UART_RX_COMPLETE_CB_ID, UART_Recieve_Complete);//need change
	HAL_UART_RegisterRxEventCallback(&CONTROLLER_HUART,RxEventCallback);
	RingBufferIni(BUFFER_SIZE,&buffer,&rb);
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART,&DMAbuffer,DMA_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(CONTROLLER_HUART.hdmarx, DMA_IT_HT);
}

/*
*the process of handle data from controller
*/
void controllerDataProcess()
{
	CHprocess();
	//HAL_UARTEx_ReceiveToIdle_DMA(CONTROLLER_HUART,DMAbuffer,DMA_BUFFER_SIZE);
}

/*
*when uart is not receiving,function will be called
*/
void RxEventCallback(UART_HandleTypeDef *huart,uint16_t Pos)
{
	(void)huart;
	//move data from dma_buffer to ringbuffer
	uint8_t bufferSpace = RingBuffer_GetRemain(&rb);
	for(int i=0;i<bufferSpace && i<Pos;i++)
	{
		RingBuffer_Write(&rb,DMAbuffer[i]);
	}  

	//tell program RingBuffer should be handlled
	osEventFlagsSet(controlTargetFlags,controllerDataIdle);
}

/*
*return the shift value of a channel
*/
inline int16_t getCH_Shift(uint8_t CH_Index)
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
float getCH_Per(uint8_t CH_Index)
{
	return (float)getCH_Shift(CH_Index) / (float)CH_Total;
}

/*
*this fun process data from controller to controller Channel
*fun should be called when idle event callback is triggered AND
*callback fun has moved data from DMA buffer to RingBuffer
*/
void CHprocess()
{
	uint8_t length = RingBuffer_GetLength(&rb);
	if(length > FRAME_LENGTH)
	{
		for(int i=0;i<length - FRAME_LENGTH;i++)
		{
			//validate frame start and frame end
			if(RingBuffer_Read(&rb,0) 				== FRAME_START && 
			   RingBuffer_Read(&rb,FRAME_LENGTH - 1) == FRAME_END)
			{
				break;
			}else
			{//if not validate,RingBuffer add read index
				RingBuffer_AddReadIndex(&rb,0x01);
			}
		}
		//no validate frame start and end
		return;
	}else
	{
		//no enough data   
		return;
	}
	
	//compute frame data into Channel data
	for(int i = 0;i< FRAME_TOTAL_CH;i++)
	{
		uint8_t bufferIndex = 0;
		uint8_t CH_Index;
		uint8_t taken;
		uint8_t leftTakeNUM;
		uint8_t rightTakeNUM;
		
		leftTakeNUM  = BYTE_LENGTH - taken;
		rightTakeNUM = CH_LENGTH - leftTakeNUM;
		
		uint16_t leftTake;
		uint16_t rightTake;

		if(rightTakeNUM > BYTE_LENGTH)
		{
			uint16_t middleTake;
			rightTakeNUM -= BYTE_LENGTH;

			leftTake   = (uint16_t)RingBuffer_Read(&rb,bufferIndex) >> (CH_LENGTH - leftTakeNUM);
			bufferIndex ++;
			middleTake = (uint16_t)RingBuffer_Read(&rb,bufferIndex) >> (rightTakeNUM);
			bufferIndex ++;
			rightTake  = (uint16_t)RingBuffer_Read(&rb,bufferIndex) << (BYTE_LENGTH - rightTakeNUM);
		
			CH[CH_Index] = leftTake || middleTake || rightTake;
		}else
		{
			leftTake  = (uint16_t)RingBuffer_Read(&rb,bufferIndex) >> (CH_LENGTH - leftTakeNUM);
			bufferIndex ++;
			rightTake = (uint16_t)RingBuffer_Read(&rb,bufferIndex) << (BYTE_LENGTH - rightTakeNUM);

			CH[CH_Index] = leftTake || rightTake;
		}

		taken = rightTake;
		CH_Index ++;
	}
}

void sendData(char *pData)
{
	uint16_t len = strlen(pData);
	HAL_UART_Transmit_DMA(&TRANSMIT_HUART, (uint8_t *)pData, len);
}

bool canSendData()
{
	if (HAL_UART_GetState(&TRANSMIT_HUART) == HAL_UART_STATE_READY)
	{
		return true;
	}
	else
	{
		return false;
	}
}