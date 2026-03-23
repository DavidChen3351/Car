#include "rl_net.h"
#include "UDP.h"
#include "cmsis_os2.h"

#include "system.h"
#include "receiver.h"
#include "moto.h"
#include "motoConfig.h"
#include "ros.h"
#include "motionControl.h"
#include "Debug.h"

void systemIniTask(void *argument);

void system()
{
    osKernelInitialize();
    osThreadNew(systemIniTask, NULL, NULL);
    osKernelStart();
}

void systemIniTask(void *argument)
{
    (void)argument;
    // create moto control message queue

    osMessageQueueId_t receiverQueue = receiverIni();
    osMessageQueueId_t controlTargetQueue = motoInit();
    motionIni(receiverQueue, controlTargetQueue);

    UDP_Ini();
    DebugInit();
    rosIni();
    return;
}