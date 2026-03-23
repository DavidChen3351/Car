#include "ros.h"

#include "moto.h"
#include "UDP.h"
#include <stdio.h>
#include <stdbool.h>

#include "cmsis_os2.h"
#define PORT 2000

typedef struct
{
        float x;
        float y;
        float speed;
        float angle;
        float theta;
} posSend;

static int UDP_HandleIndex;
static void rosSendPosition();
void RosUdpCallBack(const uint8_t *buf, uint32_t len);

void rosIni()
{
        osThreadNew(rosSendPosition, NULL, NULL);
        UDP_HandleIndex = UDP_GetHandle(PORT, RosUdpCallBack);
}

void rosSendPosition()
{
        posSend p;
        for (;;)
        {
                bool status = UDP_GetBuffer(UDP_HandleIndex, 180);
                if (status == false)
                {
                        osDelay(30);
                        continue;
                }

                /*
                time = osKernelGetTickCount() % 1000000;
                sprintf(src,"%06u\n",time);
                strncat((char*)sendbuf,src,7);
                */

//								p.x = moto->position.x;
//								p.y = moto->position.y;
//								p.speed = moto->position.speed;

//								p.angle = moto->position.angle;
//								p.theta = moto->position.theta;
                // time = osKernelGetTickCount();
                UDP_WriteBuffer(UDP_HandleIndex, (uint8_t *)&p, sizeof(p));
                UDP_SendData(UDP_HandleIndex);
                osDelay(30);
        }
}

void RosUdpCallBack(const uint8_t *buf, uint32_t len)
{
        (void)buf;
	(void)len;
}