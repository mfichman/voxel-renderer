#include "matrix.h"
#include <math.h>
#include <string.h>

void qMatrixIdentity(qMatrix* self) {
    memset(self, 0, sizeof(qMatrix));
    self->data[0] = 1;
    self->data[5] = 1;
    self->data[10] = 1;
    self->data[15] = 1;
}

void qMatrixFrustum(qMatrix* self, F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
    self->data[0] = (2.0f*near)/(right-left);
    self->data[1] = 0; 
    self->data[2] = 0;
    self->data[3] = 0;

    self->data[4] = 0;
    self->data[5] = (2.0f*near)/(top-bottom);
    self->data[6] = 0;
    self->data[7] = 0;

    self->data[8] = (right+left)/(right-left);
    self->data[9] = (top+bottom)/(top-bottom);
    self->data[10] = (-far-near)/(far-near);
    self->data[11] = -1.0f;

    self->data[12] = 0;
    self->data[13] = 0;
    self->data[14] = (-2.0f*far*near)/(far-near);
    self->data[15] = 0;
}

void qMatrixPerspective(qMatrix* self, F32 fovy, F32 aspect, F32 near, F32 far) {
    // OpenGL selfrices are column-major.  The first 4 elements make up the
    // first column of the selfrix.
    F32 rad = fovy*M_PI/180.f;
    F32 f = 1.0f/tan(rad/2.0f);

    self->data[0] = f / aspect;
    self->data[1] = 0; 
    self->data[2] = 0;
    self->data[3] = 0;

    self->data[4] = 0;
    self->data[5] = f;
    self->data[6] = 0;
    self->data[7] = 0;

    self->data[8] = 0;
    self->data[9] = 0;
    self->data[10] = (far+near)/(near-far);
    self->data[11] = -1.0f;

    self->data[12] = 0;
    self->data[13] = 0;
    self->data[14] = (2.0f*far*near)/(near-far);
    self->data[15] = 0;
}

void qMatrixTranslate(qMatrix* self, F32 x, F32 y, F32 z) {
    qMatrixIdentity(self);
    self->data[12] = x;
    self->data[13] = y;
    self->data[14] = z;
}

void qMatrixRotate(qMatrix* self, qVec3f* x, qVec3f* y, qVec3f* z) {
    self->data[0] = x->x;
    self->data[1] = y->x;
    self->data[2] = -z->x;
    self->data[3] = 0.0f;

    self->data[4] = x->y;
    self->data[5] = y->y;
    self->data[6] = -z->y;
    self->data[7] = 0.0f;

    self->data[8] = x->z;
    self->data[9] = y->z;
    self->data[10] = -z->z;
    self->data[11] = 0.0f;

    self->data[12] = 0.0f;
    self->data[13] = 0.0f;
    self->data[14] = 0.0f;
    self->data[15] = 1.0f;
}

void qMatrixLookAt(qMatrix* self, qVec3f* pos, qVec3f* at, qVec3f* sky) {
    qVec3f sub = qVec3fSub(at, pos);  
    qVec3f forward = qVec3fUnit(&sub);
    qVec3f unit = qVec3fUnit(sky);
    qVec3f right = qVec3fCross(&forward, &unit);
    qVec3f up = qVec3fCross(&right, &forward);
    qMatrix tmp;
    qMatrixRotate(self, &right, &up, &forward);
    qMatrixTranslate(&tmp, -pos->x, -pos->y, -pos->z);
    qMatrixMul(self, &tmp);
}

void qMatrixMul(qMatrix* self, qMatrix* other) {
    qMatrix out;

    out.data[0] = other->data[0]*self->data[0]+other->data[1]*self->data[4]+other->data[2]*self->data[8]+other->data[3]*self->data[12];
    out.data[1] = other->data[0]*self->data[1]+other->data[1]*self->data[5]+other->data[2]*self->data[9]+other->data[3]*self->data[13];
    out.data[2] = other->data[0]*self->data[2]+other->data[1]*self->data[6]+other->data[2]*self->data[10]+other->data[3]*self->data[14];
    out.data[3] = other->data[0]*self->data[3]+other->data[1]*self->data[7]+other->data[2]*self->data[11]+other->data[3]*self->data[15];
    
    out.data[4] = other->data[4]*self->data[0]+other->data[5]*self->data[4]+other->data[6]*self->data[8]+other->data[7]*self->data[12];
    out.data[5] = other->data[4]*self->data[1]+other->data[5]*self->data[5]+other->data[6]*self->data[9]+other->data[7]*self->data[13];
    out.data[6] = other->data[4]*self->data[2]+other->data[5]*self->data[6]+other->data[6]*self->data[10]+other->data[7]*self->data[14];
    out.data[7] = other->data[4]*self->data[3]+other->data[5]*self->data[7]+other->data[6]*self->data[11]+other->data[7]*self->data[15];
    
    out.data[8] = other->data[8]*self->data[0]+other->data[9]*self->data[4]+other->data[10]*self->data[8]+other->data[11]*self->data[12];
    out.data[9] = other->data[8]*self->data[1]+other->data[9]*self->data[5]+other->data[10]*self->data[9]+other->data[11]*self->data[13];
    out.data[10] = other->data[8]*self->data[2]+other->data[9]*self->data[6]+other->data[10]*self->data[10]+other->data[11]*self->data[14];
    out.data[11] = other->data[8]*self->data[3]+other->data[9]*self->data[7]+other->data[10]*self->data[11]+other->data[11]*self->data[15];
    
    out.data[12] = other->data[12]*self->data[0]+other->data[13]*self->data[4]+other->data[14]*self->data[8]+other->data[15]*self->data[12];
    out.data[13] = other->data[12]*self->data[1]+other->data[13]*self->data[5]+other->data[14]*self->data[9]+other->data[15]*self->data[13];
    out.data[14] = other->data[12]*self->data[2]+other->data[13]*self->data[6]+other->data[14]*self->data[10]+other->data[15]*self->data[14];
    out.data[15] = other->data[12]*self->data[3]+other->data[13]*self->data[7]+other->data[14]*self->data[11]+other->data[15]*self->data[15];

    memcpy(self, &out, sizeof(out));
}
