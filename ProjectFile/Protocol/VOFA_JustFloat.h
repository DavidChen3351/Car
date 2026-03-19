#pragma once

#define CH_COUNT 4
typedef struct{
    float fdata[CH_COUNT];
    unsigned char tail[4];
}Frame;
#undef CH_COUNT

void FrameInit(Frame* f);