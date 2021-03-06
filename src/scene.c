#include "scene.h"
#include <math.h>
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
            qChunkDraw(chunk, &self->camera);
        } 
    }
}

void qSceneSetPos(qScene* self, qVec3i* pos) {
    // Compute the 9 nearest blocks that are in visual range.  If those blocks
    // are not in the cache, then load them and evict another block.
    qVec3i rpos;
    rpos.x = pos->x/qCHUNK_SIZE;
    rpos.y = pos->y/qCHUNK_SIZE;
    rpos.z = pos->z/qCHUNK_SIZE;
    
    for (U32 i = 0; i < qSCENE_MAXCHUNKS; ++i) {
        qChunk* chunk = self->chunk[i];
        if (chunk) {
            chunk->state = qChunkINACTIVE;
        }
    }

    S32 radius = 11;
    S32 depth = 3;
    for (S32 x = -radius; x <= radius; ++x) {
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

    for (S32 x = -radius; x <= radius; ++x) {
        for (S32 z = -radius; z <= radius; ++z) {
            for (S32 y = 1-depth; y <= 0; y++) { 
                U32 lod = 0;//sqrtf(x*x+y*y)/4;
                // Calculate LOD as distance from current position
                qVec3i offset = { x*qCHUNK_SIZE, y*qCHUNK_SIZE, z*qCHUNK_SIZE };
                qVec3i cpos = { offset.x+rpos.x, offset.y+rpos.y, offset.z+rpos.z };
                qChunk* chunk = qSceneChunkLoad(self, &cpos, lod); 
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

qChunk* qSceneChunkLoad(qScene* self, qVec3i* pos, U32 lod) {
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
        qChunkInit(chunk, pos);
        qChunkGenMesh(chunk, lod);
    } else if (chunk->pos.x != pos->x || chunk->pos.y != pos->y) {
        qChunkInit(chunk, pos);
        qChunkGenMesh(chunk, lod);
    } else if (chunk->lod != lod) {
        qChunkGenMesh(chunk, lod);
    }
    return chunk;
}



