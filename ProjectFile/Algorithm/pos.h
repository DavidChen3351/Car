#pragma once

#include "motoConfig.h"

typedef struct {
	float d[MOTO_NUM];//distance gain from each moto
	float motoSpeed[MOTO_NUM];
	float x;
	float y;
	float speed;
	float angle;
	float theta;//derivative of angle
	float totalDistance;
}pos;

void posIni(pos *position);
void posSetDistance(pos *position,float diff,float speed,motoIndex i);
void posCompute(pos *position);