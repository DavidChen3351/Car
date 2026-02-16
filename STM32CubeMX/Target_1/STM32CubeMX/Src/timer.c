#include "main.h"
#include "Timer.h"
#include <stdbool.h>
#include "motoConfig.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

#define leftMotoPWMHandle htim2
#define leftMotoForCH TIM_CHANNEL_1
#define leftMotoBackCH TIM_CHANNEL_4
#define leftMotoEncoderHandle htim4

#define rightMotoPWMHandle htim9
#define rightMotoForCH TIM_CHANNEL_1
#define rightMotoBackCH TIM_CHANNEL_2
#define rightMotoEncoderHandle htim3

#define PWM_ARR 100

typedef struct Timer_Struct{
    TIM_HandleTypeDef* motoInput_htim;
    TIM_HandleTypeDef* motoEncoder_htim;
    HAL_TIM_ActiveChannel motoInputForCH;
    HAL_TIM_ActiveChannel motoInputBackCH;
}Timer_motoHandle;

Timer_motoHandle motoHandles[MOTO_NUM] = {
    {&leftMotoPWMHandle, &leftMotoEncoderHandle, leftMotoForCH, leftMotoBackCH},
    {&rightMotoPWMHandle, &rightMotoEncoderHandle, rightMotoForCH, rightMotoBackCH}
};

/*
*set PWM for moto by motoIndex and target
*@para motoIndex: index of moto
*@para target should between -1.0 and 1.0
*/
inline void Timer_SetMoto(motoIndex index,float target)
{
    Timer_motoHandle handle = motoHandles[index]; 
    uint16_t counter;
    if(target >= 0)
    {
        counter = (uint16_t)(target * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle.motoInput_htim, handle.motoInputForCH, counter);
        __HAL_TIM_SetCompare(handle.motoInput_htim, handle.motoInputBackCH, 0);
    }else{
        target = target * (-1.0f);
        counter = (uint16_t)(target * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle.motoInput_htim, handle.motoInputForCH, 0);
        __HAL_TIM_SetCompare(handle.motoInput_htim, handle.motoInputBackCH, counter);
    }
}

/*
*return the counter of Moto
*/
inline uint16_t Timer_GetMotoCounter(motoIndex index)
{
    Timer_motoHandle* handle = &motoHandles[index];
    return __HAL_TIM_GetCounter(handle->motoEncoder_htim);
}

/*
*starts PWM and encoder for all motos
*/
void Timer_MotoInit()
{
    Timer_motoHandle* handle;
    for(uint8_t i = 0; i < MOTO_NUM; i++)
    {
        handle = &motoHandles[i];
        HAL_TIM_PWM_Start(handle->motoInput_htim, handle->motoInputForCH);
        HAL_TIM_PWM_Start(handle->motoInput_htim, handle->motoInputBackCH);

				HAL_TIM_Encoder_Start(handle->motoEncoder_htim, TIM_CHANNEL_ALL); 
				__HAL_TIM_SET_COUNTER(handle->motoEncoder_htim, 0);
    }
}