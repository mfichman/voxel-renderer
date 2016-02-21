#ifndef qTIMER_H
#define qTIMER_H

#include <sys/time.h>
#include "common.h"

typedef struct qTimer {
    struct timeval start;
} qTimer;

void qTimerInit(qTimer* self);
void qTimerStart(qTimer* self);
F64 qTimerLap(qTimer* self);
F64 qTimerElapsed(qTimer* self);


#endif
