#ifndef qMATRIX_H
#define qMATRIX_H

#include "common.h"
#include "vec.h"

typedef struct qMatrix {
    F32 data[16];
} qMatrix;

void qMatrixIdentity(qMatrix* self);
void qMatrixFrustum(qMatrix* self, F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far); 
void qMatrixPerspective(qMatrix* self, F32 fovy, F32 aspect, F32 near, F32 far);
void qMatrixTranslate(qMatrix* self, F32 x, F32 y, F32 z);
void qMatrixRotate(qMatrix* self, qVec3f* x, qVec3f* y, qVec3f* z);
void qMatrixLookAt(qMatrix* self, qVec3f* pos, qVec3f* at, qVec3f* sky);
void qMatrixMul(qMatrix* self, qMatrix* other);
//void qMatrixOrtho();
//void qMatrixScale();
//void qMatrixLook();

#endif
