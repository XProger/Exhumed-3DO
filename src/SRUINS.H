#ifndef __INCLUDEDsruinsh
#define __INCLUDEDsruinsh

#include "sprite.h"

#ifdef __INCLUDEDaih
extern PlayerObject* player;
#endif

extern Sprite* camera;
extern sint32 quitRequest;
extern sint32 invisibleCounter;
extern sint32 weaponPowerUpCounter;

void playerHurt(sint32 hpLost);
void playerLongHurt(sint32 lava);
void playerDYChange(fix32 dvel);
/* returns 1 if object accepted, 0 otherwise */
sint32 playerGetObject(sint32 objectType);
void playerGetCamel(sint32 toLevel);
void playerHitTeleport(sint32 toLevel);
void switchPlayerMotion(sint32 state);
void setEarthQuake(sint32 richter);
void stunPlayer(sint32 ticks);
void changeColorOffset(sint32 r, sint32 g, sint32 b, sint32 rate);
void addColorOffset(sint32 r, sint32 g, sint32 b);
void redrawBowlDots(void);
sint32 getKeyMask(void);
void changeMessage(char* message);
void playerGotEntombedWithRamses(void);
sint32 getEarthQuake(void);

extern Orient playerAngle;
#endif
