#ifndef __INCLUDEDwallsh
#define __INCLUDEDwallsh

#define FOCALDIST 160
#define PROJECT(x, z) (((x) * (FOCALDIST >> 4)) / (sint32)((z) >> 4))

void drawWalls(MthMatrix* view);
void drawWallsFinish(void);
void wallRenderSlaveMain(void);
void startSlave(void* slaveMain);
void drawSprites(MthXyz* playerPos, MthMatrix* view, sint32 sector);
void initWallRenderer(void);
void removeLight(Sprite* s);
void addLight(Sprite* s, sint32 r, sint32 g, sint32 b);
void changeLightColor(Sprite* s, sint32 r, sint32 g, sint32 b);

void initWater(void);
void stepWater(void);

extern sint32 nmPolys, nmSlavePolys;
extern Sprite* autoTarget;
extern MthXyz autoTFormedPos;

extern sint16 plaxBBymax, plaxBBxmax, plaxBBymin, plaxBBxmin, mipBase;
#endif
