#include "main.h"
#include "uart.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdbool.h>
#include <string.h>

#define CONTROLLER_HUART huart3//uart for controller data receiving
#define TTL_HUART huart1 //uart for ttl communication with upper computer

#define DMA_BUFFER_SIZE 60

void receiveEventCallBack(struct __UART_HandleTypeDef *huart, uint16_t Pos);
void receiveCompleteCallBack(UART_HandleTypeDef *huart);
void receiveErrorCallBack(UART_HandleTypeDef *huart);
void receiveHalfCompleteCallBack(UART_HandleTypeDef *huart);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

userReceiveCallBack userRecCB;
static uint16_t lastPos;
uint8_t DMA_Buffer[DMA_BUFFER_SIZE];

void uartIni(userReceiveCallBack callBack)
{
	userRecCB = callBack;
	lastPos = 0;

  	HAL_UART_RegisterRxEventCallback(&CONTROLLER_HUART, receiveEventCallBack);//idle call back
	HAL_UART_RegisterCallback(&CONTROLLER_HUART, HAL_UART_RX_COMPLETE_CB_ID, receiveCompleteCallBack);//dma full
	HAL_UART_RegisterCallback(&CONTROLLER_HUART, HAL_UART_RX_HALFCOMPLETE_CB_ID,receiveHalfCompleteCallBack);//half dma full
	HAL_UART_RegisterCallback(&CONTROLLER_HUART, HAL_UART_ERROR_CB_ID,receiveErrorCallBack);//error case
	
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART, DMA_Buffer, DMA_BUFFER_SIZE);
	//__HAL_DMA_DISABLE_IT(CONTROLLER_HUART.hdmarx, DMA_IT_HT);
}

bool uartTTL_SendData(char *pData)
{
	uint16_t len = (uint16_t)strlen(pData);
	HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&TTL_HUART, (uint8_t *)pData, len);
	return (status == HAL_OK);
}
	
void receiveEventCallBack(struct __UART_HandleTypeDef *huart, uint16_t Pos)
{
	(void)huart;
	if (Pos != lastPos)
	{
		if(Pos > lastPos)
		{
			/*
             * Processing is done in "linear" mode.
             *
             * Application processing is fast with single data block,
             * length is simply calculated by subtracting pointers
             *
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
			userRecCB(&DMA_Buffer[lastPos], Pos - lastPos);
		}
		else
		{
			/*
             * Processing is done in "overflow" mode..
             *
             * The application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
			userRecCB(&DMA_Buffer[lastPos], DMA_BUFFER_SIZE - lastPos);
			if (Pos > 0)
			{
				userRecCB(&DMA_Buffer[0], Pos);
			}
		}
		lastPos = Pos;
	}
}

void receiveCompleteCallBack(UART_HandleTypeDef *huart)
{
	receiveEventCallBack(huart,DMA_BUFFER_SIZE);
}

void receiveHalfCompleteCallBack(UART_HandleTypeDef *huart)
{
	receiveEventCallBack(huart,DMA_BUFFER_SIZE/2);
}

void receiveErrorCallBack(UART_HandleTypeDef *huart)
{
	(void) huart;
	// restart uart receving
	lastPos = 0;
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART, DMA_Buffer, DMA_BUFFER_SIZE);
}