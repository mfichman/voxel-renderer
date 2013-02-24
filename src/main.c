#include "chunk.h"
#include "common.h"
#include "scene.h"
#include "math.h"
#include "shader.h"
#include "drawbuf.h"
#include "util.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lualib.h>
#include <luajit-2.0/lauxlib.h>

struct lua_State* lua = 0;

int GLFWCALL qWinOnClose() {
    exit(0);
    return 1;
}

void GLFWCALL qWinOnSize(int width, int height) {
}

void qConfigLoad() {
    lua = luaL_newstate();
    if (!lua) {
        fprintf(stderr, "couldn't start lua\n");
        exit(1);
    }
    if (luaL_loadfile(lua, "config.lua")) {
        fprintf(stderr, "failed to open file\n");
        exit(1);
    }
    if (lua_pcall(lua, 0, 0, 0)) {
        fprintf(stderr, "%s\n", lua_tostring(lua, -1));
        exit(1);
    }
}

GLfloat qConfigGetf(char const* name) {
    lua_getglobal(lua, name);
    GLfloat val = lua_tonumber(lua, -1);
    lua_settop(lua, 0);
    return val; 
}


qShader* shader = 0;

void qWinInit() {
    GLfloat width = qConfigGetf("width");
    GLfloat height = qConfigGetf("height");
    if (!glfwInit()) {
        fprintf(stderr, "failed to initialize glfw\n");
        exit(1);
    }
    if (!glfwOpenWindow(width, height, 8, 8, 8, 8, 24, 0, GLFW_WINDOW)) {
        fprintf(stderr, "failed to open window\n");
        exit(1);
    }
    glfwSetWindowTitle("");
    glfwSetWindowSizeCallback(qWinOnSize);
    glfwSetWindowCloseCallback(qWinOnClose);

    glViewport(0, 0, width, height);
    glEnableVertexAttribArray(qShaderVERTEX);
    glEnableVertexAttribArray(qShaderNORMAL);
    glEnableVertexAttribArray(qShaderTEXTURE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClearDepth(1.0);

    shader = qShaderNew("frag.glsl", "vert.glsl");
    qShaderBind(shader);


    // Set up cameras
    qMatrix proj;
    qMatrixPerspective(&proj, 60.f, width/height, 1., 400.);
    qShaderProj(shader, &proj);

}

extern U32 vertices;

int main() {
    qConfigLoad();
    qWinInit();

    // Everything below here is just for messing around
    glClearColor(.2, .2, .2, 1);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

/*
    qChunk* chunk = qChunkNew();
    qVec3i pos1 = { 0, 0, 0 };
    qChunkSetBlock(chunk, &pos1, 1);
    qVec3i pos2 = { 0, 1, 0 };
    qChunkSetBlock(chunk, &pos2, 1);
    qVec3i pos3 = { 0, 0, 1 };
    qChunkSetBlock(chunk, &pos3, 1);
    qVec3i pos4 = { 0, 1, 1 };
    qChunkSetBlock(chunk, &pos4, 1);
    qVec3i pos5 = { 1, 0, 0 };
    qChunkSetBlock(chunk, &pos5, 1);
    qVec3i pos = { 0, 0, 0 };
    qChunkInit(chunk, &pos);
    qChunkGenMesh(chunk, 0);
*/

    qScene* scene = qSceneNew();
    qVec3i spos = { 0, 0, 0, };
    qSceneSetPos(scene, &spos);

    qMatrix view;
    //qVec3f eye = { -220, 80, -220 };
    //qVec3f eye = { 20, 80, 200 };
    //qVec3f eye = { 32, 32, 32 };
    //qVec3f at = { 0, 0, 0  };
    qVec3f eye = { 0, 30, 0 };
    qVec3f at = { -70, 20, -70 };
    qVec3f up = { 0, 1, 0 };
    qMatrixLookAt(&view, &eye, &at, &up);
    qShaderView(shader, &view);

    printf("%d vertices\n", vertices);
    S32 frames = 0;
    struct timeval last = { 0, 0 };
    gettimeofday(&last, 0);
    while (1) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        qSceneDraw(scene, shader);
/*
        qMatrix model;
        qMatrixIdentity(&model);
        qShaderModel(shader, &model);
        qChunkDraw(chunk);
*/
        glfwSwapBuffers();
        ++frames;
        if (frames >= 100) {
            struct timeval now = { 0, 0 };
            gettimeofday(&now, 0);
            F64 elapsed = (now.tv_usec-last.tv_usec) / 1000000.f;
            elapsed += now.tv_sec-last.tv_sec;
            printf("%f fps\n", frames/elapsed);
            frames = 0;
            last = now;
        }
    }

    return 0;
}
