#include "chunk.h"
#include "common.h"
#include "scene.h"
#include "timer.h"
#include "shader.h"
#include "drawbuf.h"
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lualib.h>
#include <luajit-2.0/lauxlib.h>

static struct lua_State* lua = 0;
static qShader* shader = 0;
static GLFWwindow* window = 0;


void qWinOnClose(GLFWwindow* window) {
    exit(0);
}

void qWinOnSize(GLFWwindow* window, int width, int height) {
}

void qConfigLoad() {
    lua = luaL_newstate();
    luaL_openlibs(lua);
    if (!lua) {
        fprintf(stderr, "couldn't start lua\n");
        exit(1);
    }
    if (luaL_loadfile(lua, "config.lua")) {
        fprintf(stderr, "failed to open file\n");
        fprintf(stderr, "%s\n", lua_tostring(lua, -1));
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


void qWinInit() {
    GLfloat width = qConfigGetf("width");
    GLfloat height = qConfigGetf("height");
    if (!glfwInit()) {
        fprintf(stderr, "failed to initialize glfw\n");
        exit(1);
    }
    window = glfwCreateWindow(width, height, "", 0, 0);
    if (!window) {
        fprintf(stderr, "failed to open window\n");
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, qWinOnSize);
    glfwSetWindowCloseCallback(window, qWinOnClose);

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
    qMatrixPerspective(&proj, 60.f, width/height, 1., 800.);
    qShaderProj(shader, &proj);

}

void qUpdate(F64 delta) {
    // Update the scene
}

extern U32 vertices;

int main() {
    qConfigLoad();
    qWinInit();

    // Everything below here is just for messing around
    glClearColor(.2, .2, .2, 1);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    qMatrix view;
    qVec3i eye = { 100, 60, 1 };
    qVec3i at = { 0, -100, 0 };//-120 };
    qVec3i up = { 0, 1, 0 };
    qVec3f eyef = qVec3iToVec3f(&eye);
    qVec3f atf = qVec3iToVec3f(&at);
    qVec3f upf = qVec3iToVec3f(&up);
    qCamera cam = { eye, at, up };

    qScene* scene = qSceneNew();
    qSceneSetPos(scene, &cam.eye);
    scene->camera = cam;

    qMatrixLookAt(&view, &eyef, &atf, &upf);
    qShaderView(shader, &view);

    printf("%d vertices\n", vertices);
    F64 accum = 0.;
    F64 step = 1./60.;
    S32 frames = 0;
    qTimer perf;
    qTimerStart(&perf);
    qTimer time;
    qTimerStart(&time);

    while (1) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        qSceneDraw(scene, shader);
        glfwSwapBuffers(window);
        
        accum += qTimerLap(&time);
        while (accum > step) {
            accum -= step;
            qUpdate(step);

            // <hack>
             
            // </hack>
        }
        ++frames;
        if (frames >= 100) {
            F64 elapsed = qTimerLap(&perf);
            printf("%f fps\n", frames/elapsed);
            frames = 0;
        }
    }

    return 0;
}

struct qTestStruct {
    int a;
    int b;
};

void qTest(struct qTestStruct* str) {
    printf("hello %d %d\n", str->a, str->b);
}


