#include "main.h"

//#define CONTROLLER_HUART &huart3
//#define TRANSMIT_HUART &huart1

uint8_t controllerBuf[];
uint8_t dataBuf[];

void uartInit()
{
    
	//HAL_UART_RegisterCallback(&TRANSMIT_UART_HANDLE, HAL_UART_RX_COMPLETE_CB_ID, UART_Recieve_Complete);
	//HAL_UART_Receive_DMA(&CONTROLLER_UART_HANDLE, &, 1);
    //HAL_UART_Receive_DMA(&TRANSMIT_UART_HANDLE, &REC_data, 1);
	
}

HAL_UART_StateTypeDef uartState()
{
	return HAL_UART_GetState(&TRANSMIT_HUART);
}

