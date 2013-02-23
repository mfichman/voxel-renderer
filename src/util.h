#ifndef qUTIL_H
#define qUTIL_H

#include "drawbuf.h"

qDrawBuf* qCube();
char const* qFileRead();

void qSimplexNoiseInit();
F64 qSimplexNoise(F64 x, F64 y, F64 z);

#endif
