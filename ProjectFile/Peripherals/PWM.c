#include <stdbool.h>
#include "main.h"

#include "PWM.h"

void PWM_Init(PWM_Handle* handle,TIM_HandleTypeDef* PWM_Htim,uint32_t CH1,uint32_t CH2,uint16_t PWM_ARR)
{
    handle->PWM_Htim = PWM_Htim;
    handle->CH1 = CH1;
    handle->CH2 = CH2;
    handle->PWM_ARR = PWM_ARR;

    handle->CH1_InitStatus = HAL_TIM_PWM_Start(handle->PWM_Htim,handle->CH1);
    handle->CH2_InitStatus = HAL_TIM_PWM_Start(handle->PWM_Htim,handle->CH2);
}

void PWM_Set(PWM_Handle* handle,float duty)
{
    uint32_t PWM_ARR = handle->PWM_ARR;
    uint16_t counter;
    if(duty >= 0)
    {
        counter = (uint16_t)(duty * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle->PWM_Htim,handle->CH1,counter);
        __HAL_TIM_SetCompare(handle->PWM_Htim,handle->CH2,0);
    }else{
        duty = duty * (-1.0f);
        counter = (uint16_t)(duty * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle->PWM_Htim,handle->CH1, 0);
        __HAL_TIM_SetCompare(handle->PWM_Htim,handle->CH2, counter);
    }
}
