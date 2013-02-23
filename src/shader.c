#include "shader.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

qShader* qShaderNew(char const* frag, char const* vert) {
    qShader* self = malloc(sizeof(qShader));
    self->frag = glCreateShader(GL_FRAGMENT_SHADER);
    self->vert = glCreateShader(GL_VERTEX_SHADER);
    self->prog = glCreateProgram();


    GLchar const* fragsrc = qFileRead(frag);
    GLint fraglen = strlen(fragsrc);
    GLchar const* vertsrc = qFileRead(vert);
    GLint vertlen = strlen(vertsrc);

    glShaderSource(self->frag, 1, &fragsrc, &fraglen);
    glCompileShader(self->frag);
    glShaderSource(self->vert, 1, &vertsrc, &vertlen);
    glCompileShader(self->vert);
    glAttachShader(self->prog, self->frag);
    glAttachShader(self->prog, self->vert);
    glBindAttribLocation(self->prog, qShaderVERTEX, "pos");
    glBindAttribLocation(self->prog, qShaderNORMAL, "normal");
    glBindAttribLocation(self->prog, qShaderTEXTURE, "tex");
    glLinkProgram(self->prog);

    GLint status = 0;
    glGetProgramiv(self->prog, GL_LINK_STATUS, &status);
    if (!status) {
        GLint len = 0;
        GLchar* buf = 0;

        glGetProgramiv(self->prog, GL_INFO_LOG_LENGTH, &len);
        buf = realloc(buf, len);
        glGetProgramInfoLog(self->prog, len, &len, buf);
        fprintf(stderr, "%*s", len, buf);

        glGetShaderiv(self->frag, GL_INFO_LOG_LENGTH, &len);
        buf = realloc(buf, len);
        glGetShaderInfoLog(self->frag, len, &len, buf); 
        fprintf(stderr, "%*s", len, buf); 

        glGetShaderiv(self->vert, GL_INFO_LOG_LENGTH, &len);
        buf = realloc(buf, len);
        glGetShaderInfoLog(self->vert, len, &len, buf); 
        fprintf(stderr, "%*s", len, buf); 

        free(buf);
        exit(1);  
    }

    self->model = glGetUniformLocation(self->prog, "model");
    self->view = glGetUniformLocation(self->prog, "view");
    self->proj = glGetUniformLocation(self->prog, "proj");

    return self;
}

void qShaderBind(qShader* self) {
    glUseProgram(self->prog);
}

void qShaderDel(qShader* self) {
    glDeleteProgram(self->prog);
    glDeleteProgram(self->vert);
    glDeleteProgram(self->frag);
}

void qShaderModel(qShader* self, qMatrix* mat) {
    glUniformMatrix4fv(self->model, 1, 0, mat->data); 
}

void qShaderView(qShader* self, qMatrix* mat) {
    glUniformMatrix4fv(self->view, 1, 0, mat->data); 
}

void qShaderProj(qShader* self, qMatrix* mat) {
    glUniformMatrix4fv(self->proj, 1, 0, mat->data); 
}


