#include "drawbuf.h"
#include "shader.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

qDrawBuf* qDrawBufNew() {
    qDrawBuf* self = malloc(sizeof(qDrawBuf));
    self->size = 0;
    self->capacity = 1024;
    self->data = malloc(self->capacity);
    glGenBuffers(1, &self->id);
    return self;
}

void qDrawBuf3f(qDrawBuf* self, GLfloat x, GLfloat y, GLfloat z) {
    qDrawBufReserve(self, self->size+8*sizeof(GLfloat));
    *(GLfloat*)(self->data+self->size) = x;
    self->size += sizeof(GLfloat);
    *(GLfloat*)(self->data+self->size) = y;
    self->size += sizeof(GLfloat);
    *(GLfloat*)(self->data+self->size) = z;
    self->size += sizeof(GLfloat);

    *(GLfloat*)(self->data+self->size) = 0;
    self->size += sizeof(GLfloat);
    *(GLfloat*)(self->data+self->size) = 0;
    self->size += sizeof(GLfloat);
    *(GLfloat*)(self->data+self->size) = 0;
    self->size += sizeof(GLfloat);
    // Normals

    *(GLfloat*)(self->data+self->size) = 0;
    self->size += sizeof(GLfloat);
    *(GLfloat*)(self->data+self->size) = 0;
    self->size += sizeof(GLfloat);
    // Texcoords
}

void qDrawBufVertex(qDrawBuf* self, qVertex* vertex) {
    qDrawBufReserve(self, self->size+sizeof(qVertex));
    memcpy(self->data+self->size, vertex, sizeof(qVertex));
    self->size += sizeof(qVertex);
}

void qDrawBufReserve(qDrawBuf* self, U32 size) {
    if (size >= self->capacity) {
        self->capacity *= 2;
        self->data = realloc(self->data, self->capacity);
    }
}

void qDrawBufCommit(qDrawBuf* self) {
    glBindBuffer(GL_ARRAY_BUFFER, self->id);
    glBufferData(GL_ARRAY_BUFFER, self->size, self->data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void qDrawBufDraw(qDrawBuf* self) {
    assert(self->id);
    
    U32 stride = 8 * sizeof(GLfloat);
    void* pos = (void*)0;
    void* normal = (void*)(3 * sizeof(GLfloat));
    void* tex = (void*)(6 * sizeof(GLfloat));
    glBindBuffer(GL_ARRAY_BUFFER, self->id);
    glVertexAttribPointer(qShaderVERTEX, 3, GL_FLOAT, GL_FALSE, stride, pos); 
    glVertexAttribPointer(qShaderNORMAL, 3, GL_FLOAT, GL_FALSE, stride, normal); 
    glVertexAttribPointer(qShaderTEXTURE, 2, GL_FLOAT, GL_FALSE, stride, tex); 
    glDrawArrays(GL_QUADS, 0, self->size / 8 / sizeof(GLfloat));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void qDrawBufDel(qDrawBuf* self) {
    glDeleteBuffers(1, &self->id);
    free(self->data);
    free(self);
}
