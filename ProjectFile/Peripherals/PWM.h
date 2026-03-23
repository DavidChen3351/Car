#pragma once

#include <stm32f4xx_hal_tim.h>
#include <stdint.h>

typedef struct {
    TIM_HandleTypeDef* PWM_Htim;
    uint32_t CH1;
    uint32_t CH2;
    HAL_StatusTypeDef CH1_InitStatus;
    HAL_StatusTypeDef CH2_InitStatus;
    uint16_t PWM_ARR;
} PWM_Handle;

void PWM_Init(PWM_Handle* handle,TIM_HandleTypeDef* PWM_Htim,uint32_t CH1,uint32_t CH2,uint16_t PWM_ARR);
void PWM_Set(PWM_Handle* handle, float duty);
