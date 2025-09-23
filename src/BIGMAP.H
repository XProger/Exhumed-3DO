#ifndef __INCLUDEDbigmaph
#define __INCLUDEDbigmaph

/* returns next level to load */
sint32 runMap(sint32 currentLevel);
char* getLevelName(sint32 lNm);
sint32 getMapLink(sint32 fromLevel, sint32 direction);

#endif
