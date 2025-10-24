#pragma once

#include "main.h"

typedef struct GPIO_Struct{
    TIM_HandleTypeDef* motoInput_htim;
    TIM_HandleTypeDef* motoEncoder_htim;
    HAL_TIM_ActiveChannel motoInputForCH;
    HAL_TIM_ActiveChannel motoInputBackCH;
}GPIO_motoHandle;

//void Encoder_Init(uint8_t encoderID,void* encoderOverFlowCB(TIM_HandleTypeDef *htim));
void GPIO_SetMoto(GPIO_motoHandle* handle,float target);
uint16_t GPIO_GetMotoCounter(GPIO_motoHandle* handle);