#include <cmsis_os2.h>

#include "UDP.h"
#include "moto.h"
#include "VOFA_JustFloat.h"

#define PORT 2000

static motoHandle* moto;
void DebugTask();
void DebugCallBack();

void DebugInit()
{
    moto = motoReturnHandle();
    osThreadNew(DebugTask, NULL, NULL);

}

void DebugTask()
{
    Frame f;
    FrameInit(&f);
    float leftSpeed;
    float rightSpeed;
    float leftTarget;
    for (;;)
    {
        bool stasus = UDP_GetBuffer(180);
        if(stasus == false)
        {
            osDelay(300);
            continue;
        }

        leftSpeed = moto->motoSpeed[0].currentSpeed;
        rightSpeed = moto->motoSpeed[1].currentSpeed;   
        leftTarget = moto->motoPID[0].target;
        float rightTarget = moto->motoPID[1].target;
        f.fdata[0] = leftSpeed;
        f.fdata[1] = leftTarget;
        f.fdata[2] = rightSpeed;
        UDP_WriteBuffer((uint8_t*)&f,sizeof(Frame));

        UDP_SendData();
        osDelay(50);
    }
}

void DebugCallBack(const uint8_t *buf, uint32_t len)
{

}