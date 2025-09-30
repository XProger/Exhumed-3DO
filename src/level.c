#include "level.h"
#include "file.h"
#include "util.h"

sVertexType* level_vertex;
sFaceType* level_face;
sWallType* level_wall;
sSectorType* level_sector;
uint8* level_texture;
uint8* level_vertexLight;
sObjectType* level_object;
uint8* level_objectParams;
sPBType* level_pushBlock;
sPBVertex* level_PBVert;
sint16* level_PBWall;
WaveVert* level_waveVert;
WaveFace* level_waveFace;
uint8* level_cutPlane;

sint32 level_nmSectors;
sint32 level_nmWalls;
sint32 level_nmObjects;
sint32 level_nmPushBlocks;
sint32 level_nmWaveVert;
sint32 level_nmVertex;

static uint8 level_data[512 * 1024];

#define LOADPART(array, type, number)   \
    array = (type*)ptr;                 \
    ptr += number * sizeof(type);

sint32 loadLevel(sint32 fd, sint32 tileBase)
{
    sint32 size;
    sint32 i;
    struct sLevelHeader* head;
    uint8 *ptr = level_data;

    assert(fd >= 0);
    fs_read(fd, (sint8*)&size, 4);

    size = FS_INT(&size);

    dPrint("level size=%d\n", size);
    assert(size < sizeof(level_data));

    fs_read(fd, ptr, size);

    LOADPART(head, struct sLevelHeader, 1);

    head->nmSectors = FS_INT(&head->nmSectors);
    head->nmWalls = FS_INT(&head->nmWalls);
    head->nmVerticies = FS_INT(&head->nmVerticies);
    head->nmFaces = FS_INT(&head->nmFaces);
    head->nmTextureIndexes = FS_INT(&head->nmTextureIndexes);
    head->nmLightValues = FS_INT(&head->nmLightValues);
    head->nmObjects = FS_INT(&head->nmObjects);
    head->nmObjectParams = FS_INT(&head->nmObjectParams);
    head->nmPushBlocks = FS_INT(&head->nmPushBlocks);
    head->nmPBWalls = FS_INT(&head->nmPBWalls);
    head->nmPBVert = FS_INT(&head->nmPBVert);
    head->nmWaveVert = FS_INT(&head->nmWaveVert);
    head->nmWaveFace = FS_INT(&head->nmWaveFace);
    head->nmCutSectors = FS_INT(&head->nmCutSectors);

    level_nmSectors = head->nmSectors;
    level_nmWalls = head->nmWalls;
    level_nmObjects = head->nmObjects;
    level_nmPushBlocks = head->nmPushBlocks;
    level_nmWaveVert = head->nmWaveVert;
    level_nmVertex = head->nmVerticies;

    LOADPART(level_sector, sSectorType, head->nmSectors);
    LOADPART(level_wall, sWallType, head->nmWalls);
    LOADPART(level_vertex, sVertexType, head->nmVerticies);
    LOADPART(level_face, sFaceType, head->nmFaces);
    LOADPART(level_object, sObjectType, head->nmObjects);
    LOADPART(level_pushBlock, sPBType, head->nmPushBlocks);
    LOADPART(level_PBVert, sPBVertex, head->nmPBVert);
    LOADPART(level_waveVert, WaveVert, head->nmWaveVert);
    LOADPART(level_waveFace, WaveFace, head->nmWaveFace);
    LOADPART(level_PBWall, sint16, head->nmPBWalls);
    LOADPART(level_objectParams, uint8, head->nmObjectParams);
    LOADPART(level_texture, uint8, head->nmTextureIndexes);
    LOADPART(level_vertexLight, uint8, head->nmLightValues);
    LOADPART(level_cutPlane, uint8, head->nmCutSectors);

    for (i = 0; i < head->nmSectors; i++)
    {
        sSectorType *v = level_sector + i;

        v->center[0] = FS_SHORT(&v->center[0]);
        v->center[1] = FS_SHORT(&v->center[1]);
        v->center[2] = FS_SHORT(&v->center[2]);
        v->floorLevel = FS_SHORT(&v->floorLevel);
        v->firstWall = FS_SHORT(&v->firstWall);
        v->lastWall = FS_SHORT(&v->lastWall);
        v->light = FS_SHORT(&v->light);
        v->flags = FS_SHORT(&v->flags);
    }

    for (i = 0; i < head->nmWalls; i++)
    {
        sWallType *v = level_wall + i;

        v->normal[0] = FS_INT(&v->normal[0]);
        v->normal[1] = FS_INT(&v->normal[1]);
        v->normal[2] = FS_INT(&v->normal[2]);
        v->d = FS_INT(&v->d);
        v->flags = FS_SHORT(&v->flags);
        v->textures = FS_SHORT(&v->textures);
        v->firstFace = FS_SHORT(&v->firstFace);
        v->lastFace = FS_SHORT(&v->lastFace);
        v->firstVertex = FS_SHORT(&v->firstVertex);
        v->lastVertex = FS_SHORT(&v->lastVertex);
        v->v[0] = FS_SHORT(&v->v[0]);
        v->v[1] = FS_SHORT(&v->v[1]);
        v->v[2] = FS_SHORT(&v->v[2]);
        v->v[3] = FS_SHORT(&v->v[3]);
        v->nextSector = FS_SHORT(&v->nextSector);
        v->firstLight = FS_SHORT(&v->firstLight);
        v->pixelLength = FS_SHORT(&v->pixelLength);
    }


    for (i = 0; i < head->nmVerticies; i++)
    {
        sVertexType *v = level_vertex + i;

        v->x = FS_SHORT(&v->x);
        v->y = FS_SHORT(&v->y);
        v->z = FS_SHORT(&v->z);
    }

    for (i = 0; i < head->nmFaces; i++)
    {
        sFaceType *v = level_face + i;

        v->v[0] = FS_SHORT(&v->v[0]);
        v->v[1] = FS_SHORT(&v->v[1]);
        v->v[2] = FS_SHORT(&v->v[2]);
        v->v[3] = FS_SHORT(&v->v[3]);
    }

    for (i = 0; i < head->nmObjects; i++)
    {
        sObjectType *v = level_object + i;

        v->type = FS_SHORT(&v->type);
        v->firstParam = FS_SHORT(&v->firstParam);
    }

    for (i = 0; i < head->nmPushBlocks; i++)
    {
        sPBType *v = level_pushBlock + i;

        v->enclosingSector = FS_SHORT(&v->enclosingSector);
        v->startWall = FS_SHORT(&v->startWall);
        v->endWall = FS_SHORT(&v->endWall);
        v->startVertex = FS_SHORT(&v->startVertex);
        v->endVertex = FS_SHORT(&v->endVertex);
        v->floorSector = FS_SHORT(&v->floorSector);
        v->dx = FS_SHORT(&v->dx);
        v->dy = FS_SHORT(&v->dy);
        v->dz = FS_SHORT(&v->dz);
    }

    for (i = 0; i < head->nmPBVert; i++)
    {
        sPBVertex *v = level_PBVert + i;

        v->vStart = FS_SHORT(&v->vStart);
    }

    for (i = 0; i < head->nmWaveVert; i++)
    {
        WaveVert *v = level_waveVert + i;

        v->pos = FS_INT(&v->pos);
        v->vel = FS_INT(&v->vel);
        v->connect[0] = FS_SHORT(&v->connect[0]);
        v->connect[1] = FS_SHORT(&v->connect[1]);
        v->connect[2] = FS_SHORT(&v->connect[2]);
        v->connect[3] = FS_SHORT(&v->connect[3]);
    }

    for (i = 0; i < head->nmWaveFace; i++)
    {
        WaveFace *v = level_waveFace + i;

        v->connect[0] = FS_SHORT(&v->connect[0]);
        v->connect[1] = FS_SHORT(&v->connect[1]);
        v->connect[2] = FS_SHORT(&v->connect[2]);
        v->connect[3] = FS_SHORT(&v->connect[3]);
    }

    for (i = 0; i < head->nmPBWalls; i++)
    {
        sint16 *v = level_PBWall + i;

        *v = FS_SHORT(v);
    }

    for (i = 1; i < head->nmTextureIndexes; i += 2)
        level_texture[i] += tileBase;

    for (i = 0; i < head->nmFaces; i++)
        level_face[i].tile += tileBase;

    /* paranoia checks */
#ifdef TODO // fix alignment in the data
    assert(!(((sint32)level_sector) & 3));
    assert(!(((sint32)level_wall) & 3));
    assert(!(((sint32)level_vertex) & 3));
    assert(!(((sint32)level_face) & 3));
    assert(!(((sint32)level_object) & 3));
#endif

    return 1;
}
