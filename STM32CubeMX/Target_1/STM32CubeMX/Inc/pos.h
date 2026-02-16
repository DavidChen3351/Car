#pragma once

#include "motoConfig.h"

typedef struct {
	float d[MOTO_NUM];
	float x;
	float y;
	float angle;
	float totalDistance;
}pos;

void posIni(pos *position);
void posSetNewDistance(pos *position,float diff,motoIndex i);
void posCompute(pos *position);