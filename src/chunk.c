#include "chunk.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>


U32 vertices = 0;

static U32 min(U32 x, U32 y) {
    return x < y ? x : y;
}

qChunk* qChunkNew() {
    qChunk* self = calloc(sizeof(qChunk), 1);
    self->state = qChunkINACTIVE;
    self->lod = 0;
    self->buf = qDrawBufNew();
    self->pos.x = 0;
    self->pos.y = 0;
    self->pos.z = 0;
    return self;
}

void qChunkInit(qChunk* self, qVec3i* pos) {
    // Generate a chunk using 3D Simplex noise.
    static U32 init = 0;
    if (!init) { 
        qSimplexNoiseInit();
        init = 1;
    }
    U32 island = 0;
    self->pos = *pos;
    for (U32 x = 0; x < qCHUNK_SIZE; ++x) {
        for (U32 y = 0; y < qCHUNK_SIZE; ++y) {
            for (U32 z = 0; z < qCHUNK_SIZE; ++z) {
                S64 xabs = x+pos->x;
                S64 yabs = y+pos->y;
                S64 zabs = z+pos->z; 
                F64 xs = (xabs)/48.;
                F64 ys = (yabs+island)/16.;
                F64 zs = (zabs)/48.;
                F64 val = qSimplexNoise(xs, ys, zs);
                F64 hfact = (F64)(y+pos->y)/(F64)qCHUNK_SIZE;
                if (pos->y >= 0) {
                    val += hfact*2;//+.15;
                }
                //if (pos->y >= -1) {
                //    val += hfact*2+.3;
                //} 
                //F64 t = xabs*xabs+zabs*zabs;
                //F64 rfact = 3*exp(-t/3000);
                //val -= rfact;
                //val += 1;
/*
                S64 xabs = x+pos->x;
                S64 yabs = y+pos->y;
                S64 zabs = z+pos->z; 
                F64 xs = (xabs)/48.;
                F64 ys = (yabs+island)/32.;
                F64 zs = (zabs)/48.;
                F64 val = qSimplexNoise(xs, ys, zs);
                F64 hfact = (F64)(y+pos->y)/(F64)qCHUNK_SIZE;
                if (pos->y >= -1) {
                    val += hfact*2 - .15;
                } 
*/
                qVec3i vec = { x, y, z };
                //if (val > 0.4) {
                if (val > 0.5) {
                    qChunkSetBlock(self, &vec, 0);
                } else {
                    qChunkSetBlock(self, &vec, 1);
                } 
            }
        }
    }
}

void qChunkDel(qChunk* self) {
    qDrawBufDel(self->buf);
    free(self);
}

void qChunkGenMesh(qChunk* self, U32 lod) {
    // Generate a mesh for this chunk.
    self->lod = lod;
    qChunkGenMeshX(self);
    qChunkGenMeshY(self);
    qChunkGenMeshZ(self);
    qDrawBufCommit(self->buf);
}

void qChunkGenFace(qChunk* self, qVec3i* pos, qBlockFace i, U32 width, U32 height) {
    qVec3f normals[] = {
        { 1, 0, 0 }, // Right
        { 0, 1, 0 }, // Top
        { 0, 0, 1 }, // Front
        { -1, 0, 0 }, // Left
        { 0, -1, 0 }, // Bottom
        { 0, 0, -1 }, // Back
    };
    qVec3f faces[][4] = {
        { { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 0, 0, 1 }, }, // Right
        { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 0, 0 }, }, // Top
        { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, { 0, 1, 0 }, }, // Top
        { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 }, }, // Left
        { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, }, // Bottom
        { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, }, // Back
    };
    U32 sx = 1, sy = 1, sz = 1;
    switch (i) {
    case qBlockLEFT:
    case qBlockRIGHT:
        sy = width;
        sz = height;
        break;
    case qBlockFRONT:
    case qBlockBACK:
        sx = width;
        sy = height;
        break;
    case qBlockTOP:
    case qBlockBOTTOM:
        sx = width;
        sz = height;
        break;
    default:
        assert(!"Invalid face type");
    }
    assert(i <= 6);
    
    for (U32 j = 0; j < 4; ++j) {
        qVec3f* vec = &faces[i][j];
        qVec3f* normal = &normals[i];
        qVertex v;
        v.x = (pos->x+vec->x*sx);
        v.y = (pos->y+vec->y*sy);
        v.z = (pos->z+vec->z*sz);
        v.nx = normal->x;
        v.ny = normal->y;
        v.nz = normal->z;
        v.u = 0;
        v.v = 0;
        qDrawBufVertex(self->buf, &v);
    }
    vertices += 4;
}

void qChunkGenPlane(qChunk* self, qPlane plane, qBlockFace face, U32 k) {
    // Coalesce adjacent blocks in the current plane, and render faces for the
    // adjacent blocks.
    U32 stride = min(qCHUNK_SIZE, 1 << self->lod);
    for (U32 i = 0; i < qCHUNK_SIZE; i += stride) {
        for (U32 j = 0; j < qCHUNK_SIZE; j += stride) {
            qBlock cur = plane[i][j];
            if (!cur) { continue; }
            qVec3i pos = { 0, 0, 0 };
            switch (face) {
            case qBlockLEFT:
            case qBlockRIGHT: pos.x = k; pos.y = i; pos.z = j; break;
            case qBlockFRONT:
            case qBlockBACK: pos.x = i; pos.y = j; pos.z = k; break;
            case qBlockTOP:
            case qBlockBOTTOM: pos.x = i; pos.y = k; pos.z = j; break;
            default:
                assert(!"Invalid face type");
            }
            U32 width = stride;
            U32 height = stride; 
            U32 fail = 0;
            plane[i][j] = 0;
growHeight:
            if (fail <= 1 && width >= height && (j+height < qCHUNK_SIZE)) {
                // Grow the height of the face.  Check all the blocks that
                // would be coalseced to make sure they are of the same type.
                // If not, then escape.  Example:
                // +:(i+0,j+0) +:(i+0,j+1)
                // *:(i+1,j+0) *:(i+1,j+1)
                // Check the *'s to see if they are the same type. 
                for (U32 di = i; di < i+width; di += stride) {
                    qBlock grow = plane[di][j+height];
                    if (grow != cur) { ++fail; goto growWidth; }
                }
                for (U32 di = i; di < i+width; di += stride) {
                    plane[di][j+height] = 0;
                }
                fail = 0;
                height += stride;
                goto growWidth;
            }
growWidth:
            if (fail <= 1 && width <= height && (i+width < qCHUNK_SIZE)) {
                // Grow the width fo the face
                for (U32 dj = j; dj < j+height; dj += stride) {
                    qBlock grow = plane[i+width][dj];
                    if (grow != cur) { ++fail; goto growHeight; }
                }
                for (U32 dj = j; dj < j+height; dj += stride) {
                    plane[i+width][dj] = 0;
                }
                fail = 0;
                width += stride;
                goto growHeight;
            } 
            qChunkGenFace(self, &pos, face, width, height);
        } 
    }
}

void qChunkGenMeshX(qChunk* self) {
    // Sweep through the mesh and generate non-culled faces
    U32 stride = min(qCHUNK_SIZE, 1 << self->lod);
    qBlock prev[qCHUNK_SIZE][qCHUNK_SIZE];
    memset(&prev, 0, sizeof(prev)); // Clear 'prev' array
     
    for (U32 x = 0; x < qCHUNK_SIZE+1; x += stride) {
        qBlock left[qCHUNK_SIZE][qCHUNK_SIZE];
        qBlock right[qCHUNK_SIZE][qCHUNK_SIZE];
        for (U32 y = 0; y < qCHUNK_SIZE; y += stride) {
            for (U32 z = 0; z < qCHUNK_SIZE; z+= stride) {
                qVec3i pos = { x, y, z };
                qBlock cur = qChunkBlock(self, &pos);
                if (cur && !prev[y][z]) {
                    right[y][z] = 0;
                    left[y][z] = cur; 
                } else if (!cur && prev[y][z]) {
                    right[y][z] = prev[y][z]; 
                    left[y][z] = 0;
                } else {
                    right[y][z] = 0;
                    left[y][z] = 0;
                }
                prev[y][z] = cur;
            }
        } 
        qChunkGenPlane(self, left, qBlockLEFT, x);
        qChunkGenPlane(self, right, qBlockRIGHT, x);
    }
}

void qChunkGenMeshY(qChunk* self) {
    U32 stride = min(qCHUNK_SIZE, 1 << self->lod);
    qBlock prev[qCHUNK_SIZE][qCHUNK_SIZE];
    memset(&prev, 0, sizeof(prev)); // Clear 'prev' array
     
    for (U32 y = 0; y < qCHUNK_SIZE+1; y += stride) {
        qBlock bottom[qCHUNK_SIZE][qCHUNK_SIZE];
        qBlock top[qCHUNK_SIZE][qCHUNK_SIZE];
        for (U32 x = 0; x < qCHUNK_SIZE; x += stride) {
            for (U32 z = 0; z < qCHUNK_SIZE; z+= stride) {
                qVec3i pos = { x, y, z };
                qBlock cur = qChunkBlock(self, &pos);
                if (cur && !prev[x][z]) {
                    top[x][z] = 0;
                    bottom[x][z] = cur; 
                } else if (!cur && prev[x][z]) {
                    top[x][z] = prev[x][z]; 
                    bottom[x][z] = 0;
                } else {
                    top[x][z] = 0;
                    bottom[x][z] = 0;
                }
                prev[x][z] = cur;
            }
        } 
        qChunkGenPlane(self, bottom, qBlockBOTTOM, y);
        qChunkGenPlane(self, top, qBlockTOP, y);
    }
}

void qChunkGenMeshZ(qChunk* self) {
    U32 stride = min(qCHUNK_SIZE, 1 << self->lod);
    qBlock prev[qCHUNK_SIZE][qCHUNK_SIZE];
    memset(&prev, 0, sizeof(prev)); // Clear 'prev' array
     
    for (U32 z = 0; z < qCHUNK_SIZE+1; z += stride) {
        qBlock back[qCHUNK_SIZE][qCHUNK_SIZE];
        qBlock front[qCHUNK_SIZE][qCHUNK_SIZE];
        for (U32 x = 0; x < qCHUNK_SIZE; x += stride) {
            for (U32 y = 0; y < qCHUNK_SIZE; y += stride) {
                qVec3i pos = { x, y, z };
                qBlock cur = qChunkBlock(self, &pos);
                if (cur && !prev[x][y]) {
                    front[x][y] = 0;
                    back[x][y] = cur; 
                } else if (!cur && prev[x][y]) {
                    front[x][y] = prev[x][y]; 
                    back[x][y] = 0;
                } else {
                    front[x][y] = 0;
                    back[x][y] = 0;
                }
                prev[x][y] = cur;
            }
        } 
        qChunkGenPlane(self, back, qBlockBACK, z);
        qChunkGenPlane(self, front, qBlockFRONT, z);
    }
}

void qChunkDraw(qChunk* self) {
    qDrawBufDraw(self->buf);
}

void qChunkSetBlock(qChunk* self, qVec3i* vec, qBlock block) {
    assert(vec->x < qCHUNK_SIZE);
    assert(vec->y < qCHUNK_SIZE);
    assert(vec->z < qCHUNK_SIZE);
    self->block[vec->x+qCHUNK_SIZE*vec->y+qCHUNK_SIZE*qCHUNK_SIZE*vec->z] = block;
}

qBlock qChunkBlock(qChunk* self, qVec3i* vec) {
    if (vec->x >= qCHUNK_SIZE || vec->y >= qCHUNK_SIZE || vec->z >= qCHUNK_SIZE) {
        return 0;
    }
    return self->block[vec->x+qCHUNK_SIZE*vec->y+qCHUNK_SIZE*qCHUNK_SIZE*vec->z];
}

qBlock qChunkAdjBlock(qChunk* self, qVec3i* vec, qBlockFace i) {
    // Gets the ith adjacent block of the block at 'vec', where 0 <= i < 6
    // (corner neighbors do not count). If there is no adjacent block in a
    // direction, returns 0;
    qVec3i off = { 0, 0, 0 };
    switch (i) {
    case qBlockRIGHT: off.x = qBLOCK_SIZE; off.y = 0; off.z = 0; break;
    case qBlockTOP: off.x = 0; off.y = qBLOCK_SIZE; off.z = 0; break;
    case qBlockFRONT: off.x = 0; off.y = 0; off.z = qBLOCK_SIZE; break;
    case qBlockLEFT: off.x = -qBLOCK_SIZE; off.y = 0; off.z = 0; break;
    case qBlockBOTTOM: off.x = 0; off.y = -qBLOCK_SIZE; off.z = 0; break;
    case qBlockBACK: off.x = 0; off.y = 0; off.z = -qBLOCK_SIZE; break;
    default: assert(!"invalid");
    }  
    off.x += vec->x;
    off.y += vec->y;
    off.z += vec->z;
    return qChunkBlock(self, &off);
}

