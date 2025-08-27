#include "main.h"
#include "controllerData.h"
#include "stdbool.h"
#include "string.h"
#include "control.h"	

#include"cmsis_os2.h"
#include"RTE_Components.h"
#include  CMSIS_device_header

#define CONTROLLER_UART huart3
#define PORT_TRANSMIT huart1

#define frameReadyFlag 0x00000001U 

extern UART_HandleTypeDef CONTROLLER_UART;
extern UART_HandleTypeDef PORT_TRANSMIT;

extern osThreadId_t dataProcessHandle;

uint8_t Frame_data[25];
uint8_t REC_data;
uint8_t datapoi = 0;
enum REC_Status_t REC_Status;
uint16_t CH[16];

const uint8_t Frame_Start = 0x0f;
const uint8_t Frame_End = 0x00;
const uint8_t StartPOI = 0;
const uint8_t EndPOI = 24;

const uint16_t totaltake = 11;
const uint16_t totalMask = 0x7ff;
const int bytelength = 8;

const int16_t MIDDLE = 992;
const uint16_t CH_Total = 800;

enum REC_Status_t
{
	REC_Start = 0,
	REC_Going
};

void CHprocess();
void UART_Recieve_Complete(UART_HandleTypeDef *huart);

void setFrameReadyFlag()
{
	osEventFlagsSet(dataProcessHandle,frameReadyFlag);
}

void uartInit()
{
	HAL_UART_RegisterCallback(&CONTROLLER_UART, HAL_UART_RX_COMPLETE_CB_ID, UART_Recieve_Complete);
	HAL_UART_Receive_IT(&CONTROLLER_UART, &REC_data, 1);
	REC_Status = REC_Start;
}

void UART_Recieve_Complete(UART_HandleTypeDef *huart)
{
	switch (REC_Status)
	{
	case REC_Start:

		if (REC_data == Frame_Start)
		{
			REC_Status = REC_Going;
			Frame_data[0] = REC_data;
			datapoi++;
		}
		HAL_UART_Receive_IT(huart, &REC_data, 1);
		break;
	case REC_Going:
		Frame_data[datapoi] = REC_data;
		if (datapoi < EndPOI)
		{
			datapoi++;
		}
		else if (datapoi >= EndPOI)
		{
			if (Frame_data[EndPOI] == Frame_End)
			{
				setFrameReadyFlag();
			}
			REC_Status = REC_Start;
			datapoi = 0;
		}
		HAL_UART_Receive_IT(huart, &REC_data, 1);
		break;
	}
}

void dataProcessTask(void* para)
{
	while(true)
	{
		CHprocess();
		controlFlagReady();
		osThreadFlagsWait(frameReadyFlag,osFlagsWaitAny, osWaitForever);
	}
}

int16_t getCH_Shift(uint16_t which_CH)
{
	if (which_CH >= 0 && which_CH <= 15)
	{
		return (int16_t)CH[which_CH] - MIDDLE;
	}
	else
	{
		return 0;
	}
}

float getCH_Per(uint16_t which_CH)
{
	return (float)getCH_Shift(which_CH) / (float)CH_Total;
}

void CHprocess()
{

	bool takethree;

	uint16_t lefttake;
	uint16_t righttake;

	lefttake = 8;

	righttake = 3;
	takethree = 0;
	for (int CHi = 0, datai = 1; CHi < 17; CHi++)
	{
		if (takethree == 0)
		{
			CH[CHi] = ((((uint16_t)Frame_data[datai]) >> (bytelength - lefttake)) | (((uint16_t)Frame_data[datai + 1]) << lefttake));
			datai++;
		}
		else
		{
			CH[CHi] = (((uint16_t)Frame_data[datai] >> (bytelength - lefttake)) | ((uint16_t)Frame_data[datai + 2] << (bytelength + lefttake)) | ((uint16_t)Frame_data[datai + 1] << lefttake));
			datai += 2;
		}
		CH[CHi] = CH[CHi] & totalMask;
		lefttake = bytelength - righttake;
		if (totaltake - lefttake > 8)
		{
			righttake = totaltake - lefttake - 8;
			takethree = 1;
		}
		else
		{
			righttake = totaltake - lefttake;
			takethree = 0;
		}
	}
}

HAL_UART_StateTypeDef uartState()
{
	return HAL_UART_GetState(&PORT_TRANSMIT);
}

void sendData(char *pData)
{
	uint16_t len = strlen(pData);
	HAL_UART_Transmit_DMA(&PORT_TRANSMIT, (uint8_t *)pData, len);
}

bool canSendData()
{
	if (HAL_UART_GetState(&PORT_TRANSMIT) == HAL_UART_STATE_READY)
	{
		return true;
	}
	else
	{
		return false;
	}
}