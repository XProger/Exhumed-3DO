#ifndef __INCLUDEDpich
#define __INCLUDEDpich

#include "picset.h"

enum Class
{
    TILE16BPP,
    TILE8BPP,
    TILEVDP,
    TILESMALL16BPP,
    TILESMALL8BPP,
#ifdef JAPAN
    TILEJCHAR,
#endif
    NMCLASSES
};

//#define DUMP_PICS

#ifdef DUMP_PICS
void save_tga(sint32 id, uint8 *data, uint16 *pal16, sint32 pic_class);
#endif

sint32 initPicSystem(sint32 _picNmBase, sint32* classSizes);

sint32 addPic(enum Class bpp, void* data, void* pal, sint32 lock);
/* if a pic is locked, its memory may be free'd after this call */

sint32 mapPic(sint32 picNm);

void pic_nextFrame(sint32* swaps, sint32* used);

void loadTiles(void);
sint32 loadWeaponTiles(void);

sint32 getPicClass(sint32 p);
void displayVDP2Pic(sint32 picNm, sint32 xo, sint32 yo);
void updateVDP2Pic(void);
void dontDisplayVDP2Pic(void);
void delay_dontDisplayVDP2Pic(void);
void resetPics(void);

void setDrawModeBit(sint32 class, sint32 bit, sint32 onOff);

void markAnimTiles(void);
void advanceWallAnimations(void);

sint32 loadPicSetAsPics(sint32 class);

sint32 createMippedPics(void);

#endif
