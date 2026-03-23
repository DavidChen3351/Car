#include "Encoder.h"
#include "main.h"

#define COUNTER_ARR 65535
inline int32_t calRawDiff(uint16_t current, uint16_t prev);

void EncoderInit(EncoderHandle* handle,TIM_HandleTypeDef* EncoderHtim)
{
	handle->EncoderHtim = EncoderHtim;
	
	handle->lastCounter = 0;
    __HAL_TIM_SET_COUNTER(handle->EncoderHtim, 0);
	handle->EncoderInitStatus = HAL_TIM_Encoder_Start(handle->EncoderHtim, TIM_CHANNEL_ALL); 		
}

inline uint16_t EncoderGetCounter(EncoderHandle* handle)
{
    return __HAL_TIM_GetCounter(handle->EncoderHtim);
}

inline int32_t EncoderGetDiffGain(EncoderHandle* handle)
{
    uint16_t currentCounter = __HAL_TIM_GetCounter(handle->EncoderHtim);
    int32_t counterDiff = calRawDiff(currentCounter,handle->lastCounter);
    handle->lastCounter = currentCounter;
    return counterDiff;
}

inline int32_t calRawDiff(uint16_t current, uint16_t prev)
{
	int32_t delta = (int32_t)current - (int32_t)prev;
	if (delta > (COUNTER_ARR / 2))
	{
		delta -= (COUNTER_ARR + 1); // positive overflow
	}
	else if (delta < -(COUNTER_ARR / 2))
	{
		delta += (COUNTER_ARR + 1); // negative overflow
	}
	return delta;
}
