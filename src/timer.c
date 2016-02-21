#include "timer.h"
#include <string.h>

void qTimerInit(qTimer* self) {
    memset(self, 0, sizeof(qTimer));
}

void qTimerStart(qTimer* self) {
    gettimeofday(&self->start, 0); 
}

F64 qTimerLap(qTimer* self) {
    struct timeval now = { 0, 0 };
    gettimeofday(&now, 0);  
    S64 usec = now.tv_usec-self->start.tv_usec;
    S64 sec = now.tv_sec-self->start.tv_sec;
    self->start = now;
    return sec+(F64)usec/1000000.;
}

F64 qTimerElapsed(qTimer* self) {
    struct timeval now = { 0, 0 };
    gettimeofday(&now, 0);  
    S64 usec = now.tv_usec-self->start.tv_usec;
    S64 sec = now.tv_sec-self->start.tv_sec;
    return sec+(F64)usec/1000000.;
}
