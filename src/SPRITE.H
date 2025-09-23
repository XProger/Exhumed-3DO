#ifndef __INCLUDEDspriteh
#define __INCLUDEDspriteh

#include "util.h"

#include "object.h"

typedef struct
{
    fix32 yaw;
    fix32 pitch;
    fix32 roll;
} Orient;

struct __sprite;

#define SPRITEFLAG_NOSPRCOLLISION 0x001
#define SPRITEFLAG_NOHITSCAN 0x002
#define SPRITEFLAG_NOSHADOW 0x004
#define SPRITEFLAG_IMATERIAL (SPRITEFLAG_NOSPRCOLLISION | SPRITEFLAG_NOHITSCAN | SPRITEFLAG_NOSHADOW)

#define SPRITEFLAG_FLASH 0x008
#define SPRITEFLAG_UNDERWATER 0x010
#define SPRITEFLAG_IMMOBILE 0x020
#define SPRITEFLAG_32x32 0x040
#define SPRITEFLAG_INVISIBLE 0x080

#define SPRITEFLAG_BBLOCKED 0x100
#define SPRITEFLAG_BWATER 0x200
#define SPRITEFLAG_BWATERBNDRY 0x400
#define SPRITEFLAG_BCLIFF 0x800
#define SPRITEFLAG_BSHORT 0x1000

#define SPRITEFLAG_ONSLIPPERYSLOPE 0x2000
#define SPRITEFLAG_BOUNCY 0x4000
#define SPRITEFLAG_NOSCALE 0x8000
#define SPRITEFLAG_FOOTCLIP 0x10000
#define SPRITEFLAG_LINE 0x20000
#define SPRITEFLAG_THINLINE 0x40000
#define SPRITEFLAG_COLORED 0x80000

typedef struct __sprite
{
    MthXyz pos;
    MthXyz vel;
    fix32 radius, radius2;
    sint32 angle, scale, frame;
    /* for lines, these are position of other point of line */
    struct __sprite* next;
    fix32 friction, gravity;
    Object* owner; /* object that this sprite belongs to */

    sint32 flags;
    sint16 s;
    sint16 sequence; /* this is pushblock number if this thing is a push block */
    sint16 floorSector; /* sector number of floor that we are in contact with
                          or -1 if not in contact with a floor */
    uint16 color; /* gourau shading table for colored sprites */
} Sprite;

extern Sprite sprites[];

void freeSprite(Sprite* o);
void moveSprites(void);
void initSpriteSystem(void);
Sprite* newSprite(sint32 sector, fix32 radius, fix32 friction, fix32 gravity, sint32 sequence, sint32 flags, Object* owner);

sint32 bumpWall(sWallType* wall, Sprite* o, sint32 sector);

void frameAdvance(void);

void updateSavePositions(void);

extern Sprite* sectorSpriteList[MAXNMSECTORS];

void movePushBlock(sint32 block, sint32 dx, sint32 dy, sint32 dz);

/* functions to support ai routines */
void moveCamera(void);

/* in order of priority */
#define COLLIDE_SPRITE (0x8 << 16)
#define COLLIDE_WALL (0x4 << 16)
#define COLLIDE_FLOOR (0x2 << 16)
#define COLLIDE_CEILING (0x1 << 16)

sint32 moveSprite(Sprite* sprite);
void moveSpriteTo(Sprite* sprite, sint32 newSector, MthXyz* newPos);
sint32 spriteAdvanceFrame(Sprite* sprite);
void spriteMakeSound(Sprite* sprite, sint32 sNm);

sint32 findSectorContaining(MthXyz* pos, sint32 guessSector);

#define spriteMakeSound(s, sndNm) posMakeSound((sint32)(s), &(s)->pos, (sndNm))

void updatePushBlockPositions(void);

void shiftSprites(void);
void spriteRandomizeFrame(Sprite* o);

void suckSpriteParams(Sprite* s);

#endif
