#include "ros.h"

#include "moto.h"
#include "UDP.h"
#include <stdio.h>
#include <stdbool.h>

#include "cmsis_os2.h"

static motoHandle *moto;
static void rosSendPosition();

void rosIni()
{
    moto = motoReturnHandle();
    //osThreadNew(rosSendPosition, NULL, NULL);
}

void rosSendPosition()
{
    //uint32_t time;
    float x;
    float y;
    float speed;
    float angle;
    float theta;
    for (;;)
    {
        //bool stasus = UDP_GetBuffer(180);
        if(stasus == false)
        {
            osDelay(300);
            continue;
        }

        /*
        x = moto->position.x;
        sprintf(src,"%4.4f ",x);
        strncat((char*)sendbuf,src,9);
        
        y = moto->position.y;
        sprintf(src,"%4.4f ",y);
        strncat((char*)sendbuf,src,9);

        angle = moto->position.angle; 
        sprintf(src,"%4.4f ",angle);
        strncat((char*)sendbuf,src,9);

        time = osKernelGetTickCount() % 1000000;
        sprintf(src,"%06u\n",time);
        strncat((char*)sendbuf,src,7);
        */

        x = moto->position.x;
        y = moto->position.y;
        speed = moto->position.speed;

        angle = moto->position.angle; 
        theta = moto->position.theta; 
        //time = osKernelGetTickCount();
        //UDP_WriteBuffer((uint8_t*)&x,sizeof(x));
        //UDP_WriteBuffer((uint8_t*)&y,sizeof(y));
        //UDP_WriteBuffer((uint8_t*)&speed,sizeof(speed));
        //UDP_WriteBuffer((uint8_t*)&angle,sizeof(angle));
        //UDP_WriteBuffer((uint8_t*)&theta,sizeof(theta));
        //UDP_WriteBuffer((uint8_t*)&time,sizeof(time));

        //UDP_SendData();
        osDelay(30);
    }
}