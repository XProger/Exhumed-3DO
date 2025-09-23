#ifndef __INCLUDEDbuph
#define __INCLUDEDbuph

#include "gamestat.h"

void bup_initialProc(void);
void bup_initCurrentGame(void);
sint32 bup_newGame(sint32 slot);
void bup_saveGame(void);
sint32 bup_canSaveGame(void);
SaveState* bup_getGameData(sint32 slot);

sint32 bup_loadGame(sint32 slot);
sint32 bup_canLoadGame(void);

#endif
