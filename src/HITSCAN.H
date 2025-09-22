#ifndef __INCLUDEDhitscanh
#define __INCLUDEDhitscanh

int hitScan(Sprite* dontHit, MthXyz* ray, MthXyz* pos, int sector, MthXyz* outPos, int* outSector);
int canSee(Sprite* s1, Sprite* s2);
int singleSectorWallHitScan(MthXyz* ray, MthXyz* pos, int sector, MthXyz* outPos, int* outSector);
#endif
