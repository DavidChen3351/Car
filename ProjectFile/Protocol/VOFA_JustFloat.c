/*
*This is the protocol used by vofa JustFloat
*https://www.vofa.plus/plugin_detail/?name=justfloat
*little Endian
*/
#include "VOFA_JustFloat.h"

void FrameInit(Frame* f)
{
    f->tail[0] = 0x00;
    f->tail[1] = 0x00;
    f->tail[2] = 0x80;
    f->tail[3] = 0x7f;
}