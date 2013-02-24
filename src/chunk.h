#ifndef qCHUNK_H
#define qCHUNK_H

#include "common.h"
#include "drawbuf.h"
#include "vec.h"

typedef enum qBlockType {
    qBlockEMPTY = 0,
    qBlockROCK,
    qBlockWATER,
    qBlockDIRT,
} qBlockType;

typedef enum qBlockFace {
    qBlockRIGHT = 0,
    qBlockTOP,
    qBlockFRONT,
    qBlockLEFT,
    qBlockBOTTOM,
    qBlockBACK,
} qBlockFace;

typedef U8 qBlock;

#define qCHUNK_SIZE 32
#define qBLOCK_SIZE 1

typedef enum qChunkState {
    qChunkINACTIVE,
    qChunkACTIVE,
} qChunkState;


typedef struct qChunk {
    qBlock block[qCHUNK_SIZE*qCHUNK_SIZE*qCHUNK_SIZE];
    qChunkState state;
    qVec3i pos;
    U32 lod;
    qDrawBuf* buf;
} qChunk; 

typedef qBlock qPlane[qCHUNK_SIZE][qCHUNK_SIZE];

qChunk* qChunkNew();
void qChunkInit(qChunk* self, qVec3i* pos);
void qChunkGenMesh(qChunk* self, U32 lod);
void qChunkGenMeshX(qChunk* self);
void qChunkGenMeshY(qChunk* self);
void qChunkGenMeshZ(qChunk* self);
void qChunkDraw(qChunk* self);
void qChunkSetBlock(qChunk* self, qVec3i* vec, qBlock block);
qBlock qChunkBlock(qChunk* self, qVec3i* vec);
qBlock qChunkAdjBlock(qChunk* self, qVec3i* vec, qBlockFace i);

#endif
