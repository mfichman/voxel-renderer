#ifndef qVEC_H
#define qVEC_H

#include <stdint.h>
#include "common.h"

typedef struct qVec3i {
    S64 x;
    S64 y;
    S64 z;
} qVec3i;

typedef struct qVec3f {
    F32 x;
    F32 y;
    F32 z; 
} qVec3f;

qVec3f qVec3fSub(qVec3f* self, qVec3f* other);
qVec3f qVec3fAdd(qVec3f* self, qVec3f* other);
qVec3f qVec3fUnit(qVec3f* self);
qVec3f qVec3fCross(qVec3f* self, qVec3f* other);
F32 qVec3fLen(qVec3f* self);
F32 qVec3fLen2(qVec3f* self);
F32 qVec3fDot(qVec3f* self, qVec3f* other);

#endif
