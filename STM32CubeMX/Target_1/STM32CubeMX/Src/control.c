

#include "main.h"
#include "control.h"
#include "stdbool.h"


extern TIM_HandleTypeDef htim3;
void UART_Recieve_Complete(UART_HandleTypeDef *huart);



void CHprocess();

uint8_t Frame_data[25];
uint8_t REC_data;
uint8_t datapoi = 0;

enum REC_Status_t{
	REC_Start =0,
	REC_Going
}; 
enum REC_Status_t REC_Status;
const uint8_t Frame_Start= 0x0f;
const uint8_t Frame_End = 0x00;
const uint8_t StartPOI = 0;
const uint8_t EndPOI = 24;

bool dataReady;

bool isDataReady()
{
	return dataReady;
}
//extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
void uartInit(){
	HAL_UART_RegisterCallback(&huart3,HAL_UART_RX_COMPLETE_CB_ID,UART_Recieve_Complete);
	HAL_UART_Receive_IT(&huart3,&REC_data,1);
	
	REC_Status = REC_Start;
}

void UART_Recieve_Complete(UART_HandleTypeDef *huart)
{
	switch(REC_Status){
		case REC_Start:
			
			if(REC_data == Frame_Start)
			{
				REC_Status = REC_Going;
				Frame_data[0] = REC_data;
				datapoi++;
			}
			HAL_UART_Receive_IT(huart,&REC_data,1);
			break;
		case REC_Going:
			Frame_data[datapoi] = REC_data;
			if(datapoi < EndPOI)
			{
				datapoi++;
			}
			else if(datapoi == EndPOI )
			{
				if(Frame_data[EndPOI] == Frame_End)
				{
					dataReady = 1;
				}else
				{
					dataReady = 0;
				}
				REC_Status = REC_Start;
				datapoi = 0;
			}
			HAL_UART_Receive_IT(huart,&REC_data,1);
			break;
	}
}
uint16_t CH[16];
const int16_t MIDDLE = 992;
const uint16_t factor = 45;

int16_t shift;
void dataProcess()
{
	if(dataReady == 1)
	{
		
		
		CHprocess();
		dataReady = 0;
		
	}
	
}

int16_t getShift(uint16_t CHNUM)
{
	if(CHNUM >=0 && CHNUM <= 15)
	{
		return (int16_t)CH[CHNUM] - MIDDLE;
	}else
	{
		return 0;
	}
	
}


const uint16_t totaltake = 11;
const uint16_t totalMask = 0x7ff;
const int bytelength = 8;

void CHprocess()
{
	
	bool takethree;

	uint16_t lefttake;
	uint16_t righttake;

	
	lefttake = 8;
	
	righttake = 3;
	takethree = 0;
	for(int CHi = 0,datai = 1;CHi<17;CHi++)
	{
		if(takethree ==0 ){
			CH[CHi] = ((((uint16_t)Frame_data[datai] ) >> (bytelength - lefttake)) | (((uint16_t)Frame_data[datai+1] ) << lefttake) );
			datai++;
		}
		else{
			CH[CHi] = (((uint16_t)Frame_data[datai] >> (bytelength - lefttake)) | ((uint16_t)Frame_data[datai+2]  << (bytelength+lefttake)) | ((uint16_t)Frame_data[datai+1] << lefttake)) ;
			datai +=2;
		}
		CH[CHi] = CH[CHi] & totalMask;
		lefttake = bytelength - righttake ;
		if(totaltake - lefttake > 8)
		{
			righttake = totaltake - lefttake - 8;
			takethree = 1;
		}else
		{
			righttake = totaltake - lefttake;
			takethree = 0;
		}
	}
}