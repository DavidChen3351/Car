#include "main.h"

struct kals {
    float kalGain;
    float vEstimate;
    float vPredict;
    float vVariance;

    float accVariance;

    float r;//measure variance

    float p;//process variance
};

void kalInit(struct kals *kal,float initV,float initVar,float r,float qAcc);
void kalPredict(struct kals* kal,float acc);
void kalUpdate(struct kals *kal,float newV);