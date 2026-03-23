#include "HT-8A.h"
#include "SBUS.h"

#include "motoConfig.h"

typedef enum 
{
    ONE_SIDE_TURN = 0, //only one side motor turn,when not forward
    DIFFERENTIAL_TURN = 1 //differential turn,when not forward,no XY movement
} controlMode; 

#define CHANNEL_MIDDLE 992
#define CHANNEL_MAX_GAIN 800

#define LEFT_THREE_STATE_CH 4
#define LEFT_MIDDLE_BINARY_CH 5
#define RIGHT_MIDDLE_BINARY_CH 6
#define RIGHT_THREE_STATE_CH 7

#define FORWARD_CH 2	   // index of forward channel
#define TURN_CH 0		   // index of turn channel
#define CONTROL_MODE_CH RIGHT_MIDDLE_BINARY_CH // index of control mode channel


int16_t getCH_Shift(uint16_t *CH, uint8_t CH_Index);
float getCH_Per(uint16_t *CH, uint8_t CH_Index);
/*
 *return the percentage value of a channel
 */
float getCH_Per(uint16_t *CH, uint8_t CH_Index)
{
	return (float)getCH_Shift(CH, CH_Index) / (float)CHANNEL_MAX_GAIN;
}

/*
 *return the shift value of a channel
 */
int16_t getCH_Shift(uint16_t *CH, uint8_t CH_Index)
{
	if (CH_Index >= 0 && CH_Index <= 15)
	{
		return (int16_t)CH[CH_Index] - CHANNEL_MIDDLE;
	}
	else
	{
		return 0;
	}
}

void controllerProcess(uint16_t *CH, controllerData *t)
{
	t->forwardPer = getCH_Per(CH, FORWARD_CH);
	t->turnPer = getCH_Per(CH, TURN_CH);
	//t->mode = getCH_Shift(CH, CONTROL_MODE_CH) > 0 ? ONE_SIDE_TURN : DIFFERENTIAL_TURN;
	t->frameLost = CH[FRAME_LOST_CH_INDEX] == 0U ? false : true;
	t->failSafe = CH[FAIL_SAFE_CH_INDEX] == 0U ? false : true;

	t->vel = t->forwardPer * MAX_WHEEL_SPEED;
	t->omega = t->turnPer * MAX_WHEEL_SPEED / (WHEEL_DISTANCE / 2.0f);
}


