#pragma once

#include "main.h"
#include "moto.h"

void controlIni();
void controlTargetTask(void *para);
void controlTask(void *para);

void controlTargetFlagReady();
void controlFlagReady();