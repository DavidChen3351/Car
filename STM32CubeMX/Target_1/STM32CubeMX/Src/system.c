#include "system.h"
#include "controllerData.h"
#include "moto.h"
#include "motoConfig.h"
#include "ros.h"

#include "rl_net.h"
#include "UDP.h"
#include "cmsis_os2.h"
void systemIniTask(void * argument);

void system()
{
    osKernelInitialize();
    osThreadNew(systemIniTask,NULL,NULL);
    osKernelStart();
}
void systemIniTask(void * argument)
{
	(void)argument;
    // create moto control message queue
    osMessageQueueId_t motoControlQueue = osMessageQueueNew(MAX_CONTROL_TARGET_QUEUE_NUM,sizeof(controlTarget),NULL);	
    controllerDataIni(motoControlQueue);
    motoIni(motoControlQueue);

    // initialize network
    netInitialize();
    UDP_Ini();
    rosIni();
    return;
}