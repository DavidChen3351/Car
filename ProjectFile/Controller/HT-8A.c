#include "HT-8A.h"

#define CHANNEL_MIDDLE 992
#define CHANNEL_MAX_GAIN 800;
#define FORWARD_CH 2	   // index of forward channel
#define TURN_CH 0		   // index of turn channel
#define CONTROL_MODE_CH 6 // index of control mode channel

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
	t->mode = getCH_Shift(CH, CONTROL_MODE_CH) > 0 ? ONE_SIDE_TURN : DIFFERENTIAL_TURN;
}


