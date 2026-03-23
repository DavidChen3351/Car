#include <cmsis_os2.h>
#include <stdlib.h>
#include <string.h>

#include "UDP.h"
#include "moto.h"
#include "VOFA_JustFloat.h"

#define PORT 3000
#define BUFFER_LEN 180

static int UDP_HandleIndex;
static int index = 0;
void DebugTask();
void DebugCallBack(const uint8_t *buf, uint32_t len);

void DebugInit()
{
    osThreadNew(DebugTask, NULL, NULL);
    UDP_HandleIndex = UDP_GetHandle(PORT,DebugCallBack);
}

void DebugTask()
{  
    Frame f;
    FrameInit(&f);
    float motoSpeed = 0;
    float motoPID_Target = 0;
    float Kp = 0;
    float Kd = 0;
    for (;;)
    {
        bool stasus = UDP_GetBuffer(UDP_HandleIndex,BUFFER_LEN);
        if(stasus == false)
        {
            osDelay(50);
            continue;
        }

        f.fdata[0] = motoSpeed;
        f.fdata[1] = motoPID_Target;
        f.fdata[2] = Kp;
        f.fdata[3] = (float)index;
        UDP_WriteBuffer(UDP_HandleIndex,(uint8_t*)&f,sizeof(Frame));

        UDP_SendData(UDP_HandleIndex);
        osDelay(50);
    }
}

void DebugCallBack(const uint8_t *buf, uint32_t len)
{
	if(len > 20)return;
	(void) len;
	char data[20];
	memcpy(data,(void*)buf,len);
	data[len -1] = 0;
	
	char* ptr = strchr(data,'\n');
	if(ptr != NULL)
	{
		*ptr = 0;
	}
	if(data[0] == 'p')
	{
		//moto->motoPID[0].Kp = (float)atof(&data[1]);
	}else if(data[0] == 'd')
	{
		//moto->motoPID[0].Kd = (float)atof(&data[1]);
	}else if(data[0] == 'm')
    {
        //index = (index + 1 ) % 2;
    }
}

//void DebugSend(void* ptr)