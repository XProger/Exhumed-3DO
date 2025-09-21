#ifndef __INCLUDEDbuph
#define __INCLUDEDbuph

#include "gamestat.h"

void bup_initialProc(void);
void bup_initCurrentGame(void);
int bup_newGame(int slot);
void bup_saveGame(void);
int bup_canSaveGame(void);
SaveState* bup_getGameData(int slot);

int bup_loadGame(int slot);
int bup_canLoadGame(void);

#endif
