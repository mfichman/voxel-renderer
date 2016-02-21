#include "vec.h"
#include "math.h"

qVec3i qVec3iSub(qVec3i* self, qVec3i* other) {
    qVec3i out = { self->x-other->x, self->y-other->y, self->z-other->z };
    return out;
}

qVec3i qVec3iAdd(qVec3i* self, qVec3i* other) {
    qVec3i out = { self->x+other->x, self->y+other->y, self->z+other->z };
    return out;
}

qVec3f qVec3iToVec3f(qVec3i* self) {
    qVec3f out = { self->x, self->y, self->z };
    return out;
}

qVec3f qVec3fSub(qVec3f* self, qVec3f* other) {
    qVec3f out = { self->x-other->x, self->y-other->y, self->z-other->z };
    return out;
}

qVec3f qVec3fAdd(qVec3f* self, qVec3f* other) {
    qVec3f out = { self->x+other->x, self->y+other->y, self->z+other->z };
    return out;
}

qVec3f qVec3fUnit(qVec3f* self) {
    F32 len = qVec3fLen(self);
    qVec3f out = { self->x/len, self->y/len, self->z/len };
    return out;
}

qVec3f qVec3fCross(qVec3f* self, qVec3f* other) {
    qVec3f out = { 
        self->y*other->z-self->z*other->y,
        self->z*other->x-self->x*other->z,
        self->x*other->y-self->y*other->x 
    };
    return out;
}

F32 qVec3fLen(qVec3f* self) {
    return sqrtf(qVec3fLen2(self));
}

F32 qVec3fLen2(qVec3f* self) {
    return qVec3fDot(self, self);
}

F32 qVec3fDot(qVec3f* self, qVec3f* other) {
    return self->x*other->x+self->y*other->y+self->z*other->z;
}
