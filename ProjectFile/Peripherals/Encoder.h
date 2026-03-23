#include "main.h"
#include <stdint.h>

typedef struct{
    TIM_HandleTypeDef* EncoderHtim;
    HAL_StatusTypeDef EncoderInitStatus;
    uint16_t lastCounter;
    //uint32_t counterARR;
}EncoderHandle;

void EncoderInit(EncoderHandle* handle,TIM_HandleTypeDef* EncoderHtim);
uint16_t EncoderGetCounter(EncoderHandle* handle);
int32_t EncoderGetDiffGain(EncoderHandle* handle);