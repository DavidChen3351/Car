#pragma once

typedef struct {
	float distanceGain[2];//distance gain from each moto
	float motoSpeed[2];
	float x;
	float y;
	float speed;
	float angle;
	float theta;//derivative of angle
	float totalDistance;
}positionHandle;

void posInit(positionHandle *pos);
void posSetDistance(positionHandle *pos,float speed[2],float distanceGain[2]);
void posCompute(positionHandle *pos);