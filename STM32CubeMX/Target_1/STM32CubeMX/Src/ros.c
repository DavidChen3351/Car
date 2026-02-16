#include "ros.h"

#include "moto.h"
#include "UDP.h"
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"

motoHandle *moto;
static void rosSendPosition();

void rosIni()
{
    moto = motoReturnHandle();
    osThreadNew(rosSendPosition, NULL, NULL);
}

void rosSendPosition()
{
    uint32_t time;
    uint8_t* sendbuf;
    float x;
    float y;
    float angle;
    for (;;)
    {
        sendbuf = UDP_GetBuffer(180);
        if(sendbuf == NULL)
        {
            osDelay(100);
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
        angle = moto->position.angle;   
        time = osKernelGetTickCount();
        memcpy(sendbuf,&x,sizeof(float));
        memcpy(sendbuf+sizeof(float),&y,sizeof(float));
        memcpy(sendbuf+2*sizeof(float),&angle,sizeof(float));
        memcpy(sendbuf+3*sizeof(float),&time,sizeof(uint32_t));
        UDP_SendData(sendbuf, 3*sizeof(float) + sizeof(uint32_t));
        osDelay(500);
    }
}