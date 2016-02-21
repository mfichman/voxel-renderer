#ifndef qCAMERA_H
#define qCAMERA_H

#include "vec.h"

typedef struct qCamera {
    qVec3i eye;
    qVec3i at;
    qVec3i up;
} qCamera;

qVec3i qCameraDir(qCamera* self);

#endif
