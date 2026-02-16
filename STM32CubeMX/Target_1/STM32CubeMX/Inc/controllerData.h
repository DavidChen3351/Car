#pragma once
#include "main.h"
#include <stdbool.h>
#include "cmsis_os2.h"

#define ringBufferReady 0x00000001U
void controllerDataIni(osMessageQueueId_t queue);