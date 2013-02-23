#include "scene.h"
#include <stdlib.h>
#include <stdio.h>

qScene* qSceneNew() {
    qScene* self = calloc(sizeof(qScene), 1);
    return self; 
}

void qSceneDraw(qScene* self, qShader* shader) {
    for(U32 i = 0; i < qSCENE_MAXCHUNKS; ++i) {
        qChunk* chunk = self->chunk[i];
        if (chunk) {
            qMatrix model;
            qMatrixTranslate(&model, chunk->pos.x, chunk->pos.y, chunk->pos.z);
            qShaderModel(shader, &model);
            qChunkDraw(chunk);
        } 
    }
}

void qSceneSetPos(qScene* self, qVec3i* pos) {
    // Compute the 9 nearest blocks that are in visual range.  If those blocks
    // are not in the cache, then load them and evict another block.
    self->pos = *pos; 
    qVec3i rpos;
    rpos.x = pos->x/qBLOCK_SIZE*qBLOCK_SIZE;
    rpos.y = pos->y/qBLOCK_SIZE*qBLOCK_SIZE;
    rpos.z = pos->z/qBLOCK_SIZE*qBLOCK_SIZE;
    
    for (U32 i = 0; i < qSCENE_MAXCHUNKS; ++i) {
        qChunk* chunk = self->chunk[i];
        if (chunk) {
            chunk->state = qChunkINACTIVE;
        }
    }

    S32 radius = 4;
    S32 depth = 2;
    for (S32 x = -radius; x < radius; ++x) {
        for (S32 z = -radius; z < radius; ++z) {
            for (S32 y = 1-depth; y <= 0; y++) { 
               qVec3i offset = { x*qCHUNK_SIZE, y*qCHUNK_SIZE, z*qCHUNK_SIZE };
               qVec3i cpos = { offset.x+rpos.x, offset.y+rpos.y, offset.z+rpos.z };
               qChunk* chunk = qSceneChunk(self, &cpos); 
               if (chunk) {
                   chunk->state = qChunkACTIVE;
               }
            }
        }
    }

    for (S32 x = -radius; x < radius; ++x) {
        for (S32 z = -radius; z < radius; ++z) {
            for (S32 y = 1-depth; y <= 0; y++) { 
                qVec3i offset = { x*qCHUNK_SIZE, y*qCHUNK_SIZE, z*qCHUNK_SIZE };
                qVec3i cpos = { offset.x+rpos.x, offset.y+rpos.y, offset.z+rpos.z };
                qChunk* chunk = qSceneChunkLoad(self, &cpos); 
                chunk->state = qChunkACTIVE;
            }
        }
    }
}

qChunk* qSceneChunk(qScene* self, qVec3i* pos) {
    for (U32 i = 0; i < qSCENE_MAXCHUNKS; ++i) {
        qChunk* c = self->chunk[i];
        if (c && c->pos.x == pos->x && c->pos.y == pos->y && c->pos.z == pos->z) {
            return c;
        }
    }
    return 0;
}

qChunk* qSceneChunkLoad(qScene* self, qVec3i* pos) {
    U32 index = 0;
    for (U32 i = 0; i < qSCENE_MAXCHUNKS; ++i) {
        qChunk* c = self->chunk[i];
        if (!c || c->state != qChunkACTIVE) {
            index = i;
            break;
        } 
    }
    qChunk* chunk = self->chunk[index];
    if (!chunk) {
        self->chunk[index] = qChunkNew();
        chunk = self->chunk[index];
    }
    qChunkInit(chunk, pos);
    qChunkGenMesh(chunk, 0);
    return chunk;
}



