#ifndef __INCLUDEDmaph
#define __INCLUDEDmaph

void initMap(void);
void drawMap(sint32 cx, sint32 cy, sint32 cz, sint32 yaw, sint32 currentSector);
void setMark(fix32 x, fix32 y);
void mapScaleUp(void);
void mapScaleDn(void);
void revealMap(void);

#endif
