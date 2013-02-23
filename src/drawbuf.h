#ifndef qDRAWBUF_H
#define qDRAWBUF_H

#include "common.h"

typedef struct qVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
    GLfloat u;
    GLfloat v;    
} qVertex;

typedef struct qDrawBuf {
    U32 size;
    U32 capacity;
    U8* data;
    GLuint id;
} qDrawBuf;

qDrawBuf* qDrawBufNew();
void qDrawBuf3f(qDrawBuf* self, GLfloat x, GLfloat y, GLfloat z);
void qDrawBufVertex(qDrawBuf* self, qVertex* vertex);
void qDrawBufCommit(qDrawBuf* self);
void qDrawBufReserve(qDrawBuf*, U32 size);
void qDrawBufDraw(qDrawBuf* self);
void qDrawBufDel(qDrawBuf* self);

#endif
