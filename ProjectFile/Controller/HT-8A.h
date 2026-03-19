#include "stdint.h"

typedef enum 
{
    ONE_SIDE_TURN = 0, //only one side motor turn,when not forward
    DIFFERENTIAL_TURN = 1 //differential turn,when not forward,no XY movement
} controlMode; 

typedef struct 
{
    float forwardPer;
    float turnPer;
    controlMode mode;
}controllerData;

void controllerProcess(uint16_t *CH, controllerData *t);