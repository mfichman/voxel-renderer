#include "camera.h"

qVec3i qCameraDir(qCamera* self) {
    return qVec3iSub(&self->at, &self->eye);
}
