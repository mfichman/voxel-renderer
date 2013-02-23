#ifndef qSHADER_H
#define qSHADER_H

#include "common.h"
#include "matrix.h"

typedef enum qShaderAttrib {
    qShaderVERTEX = 0,
    qShaderNORMAL,
    qShaderTEXTURE,
} qShaderAttrib;

typedef struct qShader {
    GLint frag;
    GLint vert;
    GLint prog;
    GLint model;
    GLint view;
    GLint proj;
} qShader;

qShader* qShaderNew(char const* frag, char const* vert);
void qShaderBind(qShader* self);
void qShaderDel(qShader* self);
void qShaderPosLoc(qShader* self);
void qShaderModel(qShader* self, qMatrix* mat);
void qShaderView(qShader* self, qMatrix* mat);
void qShaderProj(qShader* self, qMatrix* mat);

#endif
