#include "stdint.h"
#include <stdbool.h>

typedef struct 
{
    float forwardPer;
    float turnPer;
    float vel;
    float omega;
    bool frameLost;
    bool failSafe;
}controllerData;

void controllerProcess(uint16_t *CH, controllerData *t);