#include "main.h"
#include "GPIO.h"
#include <stdbool.h>

//#define TOTAL_PWM_COMBINE 2
//#define CHANNEL_PER_COMBINE 2
#define CHANNEL_NUM 2
#define PWM_ARR 100
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/*
enum HAL_TIM_ActiveChannel MotoCH_For[TOTAL_MOTO]  = {TIM_CHANNEL_1,TIM_CHANNEL_1};
enum HAL_TIM_ActiveChannel MotoCH_Back[TOTAL_MOTO] = {TIM_CHANNEL_4,TIM_CHANNEL_2};
TIM_HandleTypeDef MotoEncoder_Handle[TOTAL_ENCODER] = {htim3,htim4};
*/
//void Encoder_Init(uint8_t encoderID,void* encoderOverFlowCB(TIM_HandleTypeDef *htim))
//{
//    HAL_TIM_RegisterCallback(&MotoEncoder_Handle[encoderID], HAL_TIM_PERIOD_ELAPSED_CB_ID,encoderOverFlowCB);
//	HAL_TIM_Encoder_Start(&MotoEncoder_Handle[encoderID], TIM_CHANNEL_ALL);
//}

/*
*set PWM for moto
*@para target should between -1.0 and 1.0
*/
void GPIO_SetMoto(GPIO_motoHandle* handle,float target)
{
    uint16_t counter;
    if(target >= 0)
    {
        counter = (uint16_t)(target * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle->motoInput_htim,handle->motoInputForCH ,counter);
        __HAL_TIM_SetCompare(handle->motoInput_htim,handle->motoInputBackCH,0);
    }else{
        target = target * (-1.0f);
        counter = (uint16_t)(target * (float)PWM_ARR);

        __HAL_TIM_SetCompare(handle->motoInput_htim,handle->motoInputForCH ,0);
        __HAL_TIM_SetCompare(handle->motoInput_htim,handle->motoInputBackCH,counter);
    }
}

/*
*return the counter of Moto
*/
uint16_t inline GPIO_GetMotoCounter(GPIO_motoHandle* handle)
{
    return __HAL_TIM_GetCounter(handle->motoEncoder_htim);
}

void GPIO_MotoInit(GPIO_motoHandle* handle)
{
    HAL_TIM_PWM_Start(handle->motoInput_htim, handle->motoInputForCH);
    HAL_TIM_PWM_Start(handle->motoInput_htim, handle->motoInputBackCH);

	HAL_TIM_Encoder_Start(handle->motoEncoder_htim, TIM_CHANNEL_ALL); 
	__HAL_TIM_SET_COUNTER(handle->motoEncoder_htim, 0);
}