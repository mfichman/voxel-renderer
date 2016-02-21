#ifndef qSCENE_H
#define qSCENE_H

#include "common.h"
#include "chunk.h"
#include "shader.h"
#include "camera.h"
#include "vec.h"

#define qSCENE_MAXCHUNKS 1024

typedef struct qScene {
    qChunk* chunk[qSCENE_MAXCHUNKS]; 
    qCamera camera;
} qScene;

qScene* qSceneNew();
void qSceneDraw(qScene* self, qShader* shader);
void qSceneSetPos(qScene* self, qVec3i* pos);
qChunk* qSceneChunk(qScene* self, qVec3i* pos);
qChunk* qSceneChunkLoad(qScene* self, qVec3i* pos, U32 lod);

#endif
