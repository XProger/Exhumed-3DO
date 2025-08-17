#include"level.h"
#include"file.h"
#include"util.h"

sVertexType *level_vertex;
sFaceType *level_face;
sWallType *level_wall;
sSectorType *level_sector;
char *level_texture;
unsigned char *level_vertexLight;
unsigned char *level_cutPlane;
sObjectType *level_object;
unsigned char *level_objectParams;
sPBType *level_pushBlock;
sPBVertex *level_PBVert;
short *level_PBWall;
WaveVert *level_waveVert;
WaveFace *level_waveFace;

int level_nmSectors;
int level_nmWalls;
int level_nmObjects;
int level_nmPushBlocks;
int level_nmWaveVert;

int loadLevel(int fd,int tileBase)
{int size;
 int i;
 char *buffer;
 struct sLevelHeader *head;
 assert(fd>=0);
 fs_read(fd,(char *)&size,4);
 dPrint("level size=%d\n",size);
 dPrint("coreleft=%d\n",mem_coreleft(1));
 assert(size>0);
 assert(size<900000);
 buffer=(char *)mem_malloc(1,size);
 assert(buffer);
 fs_read(fd,buffer,size);

 head=(struct sLevelHeader *)buffer;
 level_nmSectors=head->nmSectors;
 level_nmWalls=head->nmWalls;
 level_nmObjects=head->nmObjects;
 level_nmPushBlocks=head->nmPushBlocks;
 level_nmWaveVert=head->nmWaveVert;

 level_sector=(sSectorType *)(buffer+sizeof(struct sLevelHeader));
 level_wall=(sWallType *)(level_sector+head->nmSectors);
#ifndef NDEBUG
 for (i=0;i<level_nmWalls;i++)
    {assert(abs(level_wall[i].normal[0])<=F(1));
     assert(abs(level_wall[i].normal[1])<=F(1));
     assert(abs(level_wall[i].normal[2])<=F(1));
    }
#endif
 level_vertex=(sVertexType *)(level_wall+head->nmWalls);
 level_face=(sFaceType *)(level_vertex+head->nmVerticies);
 level_object=(sObjectType *)(level_face+head->nmFaces);
 level_pushBlock=(sPBType *)(level_object+head->nmObjects);
 level_PBVert=(sPBVertex *)(level_pushBlock+head->nmPushBlocks);
 level_waveVert=(WaveVert *)(level_PBVert+head->nmPBVert);
 level_waveFace=(WaveFace *)(level_waveVert+head->nmWaveVert);
 level_PBWall=(short *)(level_waveFace+head->nmWaveFace);
 level_objectParams=(unsigned char *)(level_PBWall+head->nmPBWalls);
 level_texture=(char *)(level_objectParams+head->nmObjectParams);
 level_vertexLight=level_texture+head->nmTextureIndexes;

 for (i=0;i<head->nmTextureIndexes;i++)
    level_texture[i]+=tileBase;
 for (i=0;i<head->nmFaces;i++)
    level_face[i].tile+=tileBase;

 /* paranoia checks */
 assert(!(((int)level_sector) & 3));
 assert(!(((int)level_wall) & 3));
 assert(!(((int)level_vertex) & 3));
 assert(!(((int)level_face) & 3));
 assert(!(((int)level_object) & 3));

 return 1;
}
