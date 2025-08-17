#ifndef __INCLUDEDbigmaph
#define __INCLUDEDbigmaph

/* returns next level to load */
int runMap(int currentLevel,char **outFileName);
char *getLevelName(int lNm);
int getMapLink(int fromLevel,int direction);

#endif
