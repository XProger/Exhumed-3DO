#ifndef __INCLUDEDhitscanh
#define __INCLUDEDhitscanh

sint32 hitScan(Sprite* dontHit, MthXyz* ray, MthXyz* pos, sint32 sector, MthXyz* outPos, sint32* outSector);
sint32 canSee(Sprite* s1, Sprite* s2);
sint32 singleSectorWallHitScan(MthXyz* ray, MthXyz* pos, sint32 sector, MthXyz* outPos, sint32* outSector);
#endif
