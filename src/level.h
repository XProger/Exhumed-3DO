#ifndef __INCLUDEDlevelh
#define __INCLUDEDlevelh

#include "slevel.h"

extern sVertexType* level_vertex;
extern sFaceType* level_face;
extern sWallType* level_wall;
extern sSectorType* level_sector;
extern uint8* level_texture;
extern uint8* level_cutPlane;
extern uint8* level_vertexLight;
extern uint8* level_objectParams;
extern sObjectType* level_object;
extern sPBType* level_pushBlock;
extern sPBVertex* level_PBVert;
extern sint16* level_PBWall;
extern WaveVert* level_waveVert;
extern WaveFace* level_waveFace;
extern sint32 level_nmSectors;
extern sint32 level_nmWalls;
extern sint32 level_nmObjects;
extern sint32 level_nmPushBlocks;
extern sint32 level_nmWaveVert;
extern sint32 level_nmVertex;

sint32 loadLevel(sint32 tileBase);

#endif
