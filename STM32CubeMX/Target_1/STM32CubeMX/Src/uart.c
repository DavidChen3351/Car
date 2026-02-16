#include "main.h"
#include "uart.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdbool.h>
#include <string.h>

#define CONTROLLER_HUART huart3//uart for controller data receiving
#define TTL_HUART huart1 //uart for ttl communication with upper computer

void receiveEventCallBack(struct __UART_HandleTypeDef *huart, uint16_t Pos);
void receiveCompleteCallBack(UART_HandleTypeDef *huart);
void receiveErrorCallBack(UART_HandleTypeDef *huart);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

userReceiveCallBack userRecCB;
uint16_t DMA_BufferSize;
uint8_t* DMA_Buffer; 

void uartIni(userReceiveCallBack callBack,uint8_t* buffer,uint16_t bufferSize)
{
	userRecCB = callBack;
	DMA_BufferSize = bufferSize;
	DMA_Buffer = buffer;
	
  HAL_UART_RegisterRxEventCallback(&CONTROLLER_HUART, receiveEventCallBack);//idle call back
	HAL_UART_RegisterCallback(&CONTROLLER_HUART, HAL_UART_RX_COMPLETE_CB_ID, receiveCompleteCallBack);//dma full
	HAL_UART_RegisterCallback(&CONTROLLER_HUART, HAL_UART_ERROR_CB_ID,receiveErrorCallBack);//error case
	
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART, DMA_Buffer, DMA_BufferSize);
	__HAL_DMA_DISABLE_IT(CONTROLLER_HUART.hdmarx, DMA_IT_HT);
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

	userRecCB(huart,Pos);
	
	// restart uart receving
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART, DMA_Buffer, DMA_BufferSize);
	__HAL_DMA_DISABLE_IT(CONTROLLER_HUART.hdmarx, DMA_IT_HT);
}

void receiveCompleteCallBack(UART_HandleTypeDef *huart)
{
	receiveEventCallBack(huart,DMA_BufferSize);
}

void receiveErrorCallBack(UART_HandleTypeDef *huart)
{
	(void) huart;
	// restart uart receving
	HAL_UARTEx_ReceiveToIdle_DMA(&CONTROLLER_HUART, DMA_Buffer, DMA_BufferSize);
	__HAL_DMA_DISABLE_IT(CONTROLLER_HUART.hdmarx, DMA_IT_HT);
}